#!/bin/bash
#SBATCH -J job-name
#SBATCH -N 1                     # Num of nodes         (<--kaeru)
#SBATCH -n 1                     # Num of MPI processes (<--kaeru)
#SBATCH --ntasks-per-node=1      # Num of MPI processes per node
#SBATCH --ntasks-per-socket=1    # Num of MPI processes per socket
#SBATCH --cpus-per-task=1        # Num of threads per MPI process
#SBATCH -t 1:10:00
module purge
module load intel intelmpi mkl
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
cd $SLURM_SUBMIT_DIR

threads=$(nproc --all)
data=@DATA@
N=@N@
G=@G@

for t in 0.770706 0.735676 0.702238 0.670321 0.639853 0.610771 0.583011 0.556512 0.531218 0.507073 
do
for i in $(seq 1 10)
do
    ./GraphGolf -f $data -g $G -n $N -s $i -w $t -c $t > log.$t.$i.txt &

    while [ $(jobs|wc -l) -ge $threads ]; do
	sleep 1
    done
done
done

wait
echo $SECONDS