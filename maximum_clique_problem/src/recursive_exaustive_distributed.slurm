#!/bin/bash
#SBATCH --job-name=recursive_exaustive_distributed
#SBATCH --nodes=2
#SBATCH --ntasks=3
#SBATCH --partition=normal
#SBATCH --mem=3G
#SBATCH --output="outputs/recursive_exaustive_distributed_%j.out"

MAX_CLIQUE_SIZE=$1

echo "Job: $SLURM_JOB_ID"
mpirun ./recursive_exaustive_distributed "$MAX_CLIQUE_SIZE" --verbose
