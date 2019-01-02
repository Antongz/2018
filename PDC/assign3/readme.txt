/* Author: Yi-Ting, Hsieh(a1691807	*/
## Included files: 
## tsp_sequential.c, readme.txt, sample*.txt
## tsp_parallel.c, report.pdf

## Our program is reading from an file with following content:
Input File:
	N 
	d[1][2] 
	d[1][3] d[2][3] 
	d[1][4] d[2][4] d[3][4] 
	... 
	d[1][N] d[2][N] d[3][N] ... d[N-1][N] 



## First, compile our tsp_sequential.c
$ gcc tsp_sequential.c

## Usage of our program
$ ./a.out file_name

## With log:
$ ./a.out file_name log



## Second, compile our tsp_parallel.c
$ mpicc tsp_parallel.c

## Usage of our program
$ mpirun -np number_of_processes a.out file_name

## Unfortunately, the parallel program doesn't support log functionality


