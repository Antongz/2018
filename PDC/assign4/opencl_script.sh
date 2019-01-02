#!/bin/bash
#SBATCH -p test          # partition
#SBATCH -n 1           # number of cores
#SBATCH --time=01:00:00 # time allocation
#SBATCH --gres=gpu:1   # request 1 gpu
#SBATCH --mem=8GB      # memory pool for all cores

module load CUDA/8.0.61
module load GCC/6.3.0-2.27

g++ hostCode.cpp -lOpenCL -o bsortpar -std=c++11
./bsortpar 10000

