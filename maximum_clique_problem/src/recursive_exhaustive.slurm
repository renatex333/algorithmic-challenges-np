#!/bin/bash
#SBATCH --job-name=recursive_exhaustive
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --partition=normal
#SBATCH --mem=3G
#SBATCH --output="outputs/recursive_exhaustive_%j.out"

MAX_CLIQUE_SIZE=$1

echo "Job: $SLURM_JOB_ID"
./recursive_exhaustive "$MAX_CLIQUE_SIZE" --verbose
