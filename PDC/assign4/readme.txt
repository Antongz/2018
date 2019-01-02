/* Author: Yi-Ting, Hsieh(a1691807	*/
## Included files: 
## bubbleSortSequential.cpp, hostCode.cpp, kernel_code.cl, seq_script.sh, opencl_script.sh and a pdf file.

## For sequential version of bubblesort:
First, compile our bubbleSortSequential.cpp
$ g++ -o seq bubbleSortSequential.cpp

To execute our code:
$ ./seq array_size


## For parallel version of bubblesort:
First, compile our hostCode.cpp
$ g++ -o parallel bubbleSortSequential.cpp -lOpenCL
## Note that, you must install openCL-header beforehand.
## You must execute this program under a device with independent GPU. 
## Otherwise you'll get " No platforms found! Error code: -1001".


To execute our code:
$ ./parallel array_size


## If you want to test the program in Phoenix Supercomputer, once you place above files in the same directory
If you want to run a sequential version of bubblesort:
$ sbatch seq_script.sh

If you want to run a parallel version of bubblesort:
$ sbatch opencl_script.sh

You can also check your stat of your running jobs by:
$ squeue -u axxxxxxx

To cancel a job you own, use the sbatch command followed by the slurm job ID:
$ scancel job_ID

