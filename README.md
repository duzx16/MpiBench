# 并行计算课程作业

## 作业一

### 编译

```shell
mpiicc point_comm.c -o point.out
```

### 运行

```shell
bsub -I -q q_x86_expr -n node_num ./point.out ntimes data_sizes...
```

比如在3个节点间进行2轮2000次通信测试，每次分别收发2048KB和4096KB数据

```shell
bsub -I -q q_x86_expr -n 3 ./point.out 2000 2048 4096
```

### 期望输出

```
Benchmark with 2048kb data
0 -> 1 Average bandwidth 11.928094 GB/s
0 -> 2 Average bandwidth 14.089582 GB/s
1 -> 0 Average bandwidth 10.587125 GB/s
1 -> 2 Average bandwidth 10.688782 GB/s
2 -> 0 Average bandwidth 10.452579 GB/s
2 -> 1 Average bandwidth 10.436393 GB/s
Benchmark with 4096kb data
0 -> 1 Average bandwidth 11.384110 GB/s
0 -> 2 Average bandwidth 11.413918 GB/s
1 -> 0 Average bandwidth 10.078765 GB/s
1 -> 2 Average bandwidth 10.188000 GB/s
2 -> 0 Average bandwidth 9.941488 GB/s
2 -> 1 Average bandwidth 10.033108 GB/s
```

## 作业二

### 编译

```shell
mpiicc collective_comm.c -o collective.out
```

### 运行

```
bsub -I -q q_x86_expr -n 3 ./collective.out data_size comm_types...
```

比如在3个节点间分别测试bcast和gather通信，每个数据包大小为2048KB

```shell
bsub -I -q q_x86_expr -n 3 ./collective.out 2048 bcast gather
```

### 期望输出

```
Benchmark with bcast communication
Average bandwidth 2.116968 GB/s
Benchmark with gather communication
Average bandwidth 1.552046 GB/s
```

## 作业三（使用MPICH）

#### 编译

```
mpicc mpiio.c -o mpiio 
```

### 运行

```
mpiexec -np num_procs ./mpiio reading_ratios -p data_sizes
```

比如用3个进程测试读占比为1.0和0.0，数据包大小为10KB和20KB

```
mpiexec -np 3 ./mpiio 1.0 0.0 -p 10 20
```

要完成功能3和4的指定节点集和单节点的进程数，可以使用mpiexec的configfile

```
mpiexec -configfile configfile
```

configfile的格式为

```
-n 3 -host host1 ./mpiio 1.0 0.0 -p 10 20
-n 2 -host host2 ./mpiio 1.0 0.0 -p 10 20
-n 3 -host host3 ./mpiio 1.0 0.0 -p 10 20
```

