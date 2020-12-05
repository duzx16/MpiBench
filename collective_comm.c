#include <stdio.h> // Standard IO
#include <stdlib.h> // Standard Library
#include <string.h>
#include <mpi.h>

enum COMM_TYPE {
    bcast, gather, reduce, allreduce, scan, alltoall
};

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


    int n, i, j, k, my_rank, nprocs;
    int data_size = atoi(argv[1]) * 1024, ntimes = 2000;
    double start_time, end_time, time_cost, average_time;
    char *send_buffer, *recv_buffer;// dynamically allocate memory
    enum COMM_TYPE commType;


    MPI_Init(&argc, &argv); // init mpi environment
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs); // get the number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);// get the local rank
    MPI_Status status;
    if (my_rank == 0) {
        for (i = 0; i < argc; i++) {
            printf("%s\n", argv[i]);
        }
    }
    send_buffer = (char *) malloc(data_size);
    if (!send_buffer) {
        printf("Error when allocating %d memory\n", data_size);
        exit(1);
    }
    recv_buffer = (char *) malloc(data_size * nprocs);
    if (!recv_buffer) {
        printf("Error when allocating %d memory\n", data_size * nprocs);
        exit(1);
    }
    for (n = 0; n < argc - 2; n++) {
        if (my_rank == 0) { printf("Benchmark with %s communication\n", argv[2 + n]); }
        if (strcmp(argv[2 + n], "bcast") == 0)
            commType = bcast;
        else if (strcmp(argv[2 + n], "gather") == 0)
            commType = gather;
        else if (strcmp(argv[2 + n], "reduce") == 0)
            commType = reduce;
        else if (strcmp(argv[2 + n], "allreduce") == 0)
            commType = allreduce;
        else if (strcmp(argv[2 + n], "scan") == 0)
            commType = scan;
        else if (strcmp(argv[2 + n], "alltoall") == 0)
            commType = alltoall;
        else {
            printf("Incorrect communication type %s", argv[2 + n]);
        }
        MPI_Barrier(MPI_COMM_WORLD);
        start_time = MPI_Wtime();
        for(i = 0; i < ntimes; i++) {
            if (commType == bcast) {
                MPI_Bcast(send_buffer, data_size, MPI_CHAR, 0, MPI_COMM_WORLD);
            }
            else if (commType == gather) {
                MPI_Gather(send_buffer, data_size, MPI_CHAR, recv_buffer, data_size, MPI_CHAR, 0, MPI_COMM_WORLD);
            }
            else if (commType == reduce) {
                MPI_Reduce(send_buffer, recv_buffer, data_size, MPI_CHAR, MPI_SUM, 0, MPI_COMM_WORLD);
            }
            else if (commType == allreduce) {
                MPI_Allreduce(send_buffer, recv_buffer, data_size, MPI_CHAR, MPI_SUM, MPI_COMM_WORLD);
            }
            else if (commType == scan) {
                MPI_Scan(send_buffer, recv_buffer, data_size, MPI_CHAR, MPI_SUM, MPI_COMM_WORLD);
            }
            else if (commType == alltoall) {
                MPI_Alltoall(send_buffer, data_size, MPI_CHAR, recv_buffer, data_size, MPI_CHAR, MPI_COMM_WORLD);
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
        end_time = MPI_Wtime();
        time_cost = end_time - start_time;
        if (my_rank == 0) {
            average_time = time_cost / ntimes;
            printf("Average bandwidth ");
            calculate_bandwidth(average_time, data_size);
            printf("\n");
            fflush(stdout);
        }
    }
    free(recv_buffer);
    free(send_buffer);

    MPI_Finalize(); // Terminates MPI execution environment
    return 0;
}