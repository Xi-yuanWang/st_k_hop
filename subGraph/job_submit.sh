#!/bin/bash
#SBATCH --get-user-env
#SBATCH --nodes=1
#SBATCH -c 6
#SBATCH --mail-type=ALL
#SBATCH --mail-user=1800011848@pku.edu.cn
#SBATCH --partition=compute
#SBATCH -J test
#SBATCH -o outonly_bis
#SBATCH -e erroronly_bis
#SBATCH --time=48:00:00
#SBATCH --qos=normal
./only_bisearch > ./bis_exp/webGoogle0