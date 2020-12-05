#include <stdio.h> // Standard IO
#include <stdlib.h> // Standard Library
#include <string.h>
#include <mpi.h>


void calculate_bandwidth(double average_time, int data_size) {
    double bandwidth;
    char *unit = "B/s";
//    printf("%lf\n", average_time);
    bandwidth = (double) data_size / average_time;
    if (bandwidth >= 1024.) {
        bandwidth /= 1024.;
        unit = "KB/s";
        if (bandwidth >= 1024.) {
            bandwidth /= 1024.;
            unit = "MB/s";
            if (bandwidth >= 1024.) {
                bandwidth /= 1024.;
                unit = "GB/s";
            }
        }
    }
    printf("%lf %s", bandwidth, unit);
}

int main(int argc, char *argv[]) {


    int n, i, j, ratio_count = argc, my_rank, nprocs;
    int data_size, *data_sizes, ntimes = 1000, read_times, write_times;
    double *ratios, ratio;
    double start_time, end_time, time_cost, average_time;
    char *p;// dynamically allocate memory

    MPI_Init(&argc, &argv); // init mpi environment
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs); // get the number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);// get the local rank
    MPI_Status status;
    for (n = 1; n < argc; n++) {
        if (strcmp(argv[n], "-p") == 0) {
            ratio_count = n - 1;
            break;
        }
    }
    if (my_rank == 0) {
        printf("World size %d\n", nprocs);
    }
    ratios = malloc(ratio_count * sizeof(double));
    data_sizes = malloc((argc - ratio_count - 2) * sizeof(int));
    for (n = 1; n < argc; n++) {
        if (n < ratio_count + 1) {
            ratios[n - 1] = atof(argv[n]);
        } else if (n > ratio_count + 1) {
            data_sizes[n - ratio_count - 2] = atoi(argv[n]);
        }
    }
    for (i = 0; i < ratio_count; i++) {
        for (j = 0; j < argc - ratio_count - 2; j++) {
            ratio = ratios[i];
            data_size = data_sizes[j] * 1024;
            read_times = (int) (ntimes * ratio);
            write_times = ntimes - read_times;
            MPI_File read_file, write_file;
            p = (char *) malloc(data_size);
            if (!p) {
                printf("Error when allocating %d memory\n", data_size);
                exit(1);
            }
            MPI_File_open(MPI_COMM_WORLD, "read.file", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL,
                          &read_file);
            if (my_rank == 0) {
                printf("Benchmark with reading ratio %f %dkb data\n", ratio, data_size / 1024);
                for (int k = 0; k < nprocs * read_times; k++) {
                    MPI_File_write(read_file, p, data_size, MPI_CHAR, &status);
                }
            }
            MPI_File_close(&read_file);
            MPI_Barrier(MPI_COMM_WORLD);
            start_time = MPI_Wtime(); //get the wall time from MPI
            MPI_File_open(MPI_COMM_WORLD, "read.file", MPI_MODE_RDONLY, MPI_INFO_NULL, &read_file);
            MPI_File_open(MPI_COMM_WORLD, "write.file", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &write_file);
            for (int k = 0; k < read_times; k++) {
                MPI_File_read_ordered(read_file, p, data_size, MPI_CHAR, &status);
            }
            for (int k = 0; k < write_times; k++) {
                MPI_File_write_ordered(write_file, p, data_size, MPI_CHAR, &status);
            }
            MPI_File_close(&read_file);
            MPI_File_close(&write_file);
            MPI_Barrier(MPI_COMM_WORLD);
            end_time = MPI_Wtime();
            time_cost = end_time - start_time;
            free(p);
            if (my_rank == 0) {
                average_time = time_cost / ntimes;
                printf("Average bandwidth ");
                calculate_bandwidth(average_time, data_size);
                printf("\n");
                fflush(stdout);
            }
        }
    }
    free(data_sizes);
    free(ratios);
    MPI_Finalize(); // Terminates MPI execution environment
    return 0;
}