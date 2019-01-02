#!/bin/bash
#SBATCH -p test          # partition
#SBATCH -n 1           # number of cores
#SBATCH --time=01:00:00 # time allocation
#SBATCH --mem=8GB      # memory pool for all cores

module load GCC/6.3.0-2.27

g++ bubbleSortSequential.cpp -o bsortseq -std=c++11
./bsortseq 10000

