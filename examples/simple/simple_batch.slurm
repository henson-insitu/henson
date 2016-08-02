#!/bin/bash -l

#SBATCH -p debug
#SBATCH -N 20
#SBATCH -t 00:05:00
#SBATCH -J henson_test
#SBATCH -o henson_test.o%j
#SBATCH -L SCRATCH


srun -n 960 $HENSON_BUILD/henson_chai simple.chai -p world
echo Finished > finished.txt
