#!/bin/bash
#SBATCH --job-name=recursive_exaustive
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --partition=normal
#SBATCH --mem=3G
#SBATCH --output="outputs/recursive_exaustive_%j.out"

MAX_CLIQUE_SIZE=$1

echo "Job: $SLURM_JOB_ID"
./recursive_exaustive "$MAX_CLIQUE_SIZE" --verbose
