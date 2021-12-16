# Commands to compile and run ring.c program

- qsub -q dssc -l nodes=1:ppn=24 -l walltime=00:30:00 -I
- cd $PBS_O_WORKDIR
- module load  openmpi-4.1.1+gnu-9.3.0
- mpicc ring.c -o ring
- mpirun -np 4 ./ring 2>/dev/null 


# Commands to compile and run sum3Dmatrix.c program

- qsub -q dssc -l nodes=1:ppn=24 -l walltime=01:00:00 -I
- cd $PBS_O_WORKDIR
- module load  openmpi-4.1.1+gnu-9.3.0
- mpicc sum3Dmatrix.c -o sum3Dmatrix
- mpirun -np 24 ./sum3Dmatrix | sort 2>/dev/null 









