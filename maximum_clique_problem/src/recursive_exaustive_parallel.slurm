#!/bin/bash
#SBATCH --job-name=recursive_exaustive_parallel
#SBATCH --nodes=2
#SBATCH --ntasks=3
#SBATCH --partition=normal
#SBATCH --mem=3G
#SBATCH --output="outputs/recursive_exaustive_parallel_%j.out"

MAX_CLIQUE_SIZE=$1

echo "Job: $SLURM_JOB_ID"
./recursive_exaustive_parallel "$MAX_CLIQUE_SIZE" --verbose
