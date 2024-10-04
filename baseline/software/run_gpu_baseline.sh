#!/usr/bin/bash

CUDA_PATH=$1
ARCH_CODE=$2

if [ -z "$CUDA_PATH" ]; then
	CUDA_PATH="/usr/local/cuda"
fi

if [ -z "$ARCH_CODE" ]; then
	ARCH_CODE="sm_80"
fi

# GASAL2 can do local alignment, global alignment, and banded alignment
cd GASAL2

./configure.sh $CUDA_PATH    # E.g., /usr/local/cuda
make clean
make GPU_SM_ARCH=$ARCH_CODE MAX_QUERY_LEN=256 N_CODE=0x4E    # E.g. GPU_SM_ARCH=sm_80
cd test_prog
make clean
make -j
pwd
echo
# Local  
./test_prog.out -y local ../../data/pbsim2/ont_query.fa ../../data/pbsim2/ont_ref.fa

# Global
./test_prog.out -y global ../../data/pbsim2/ont_query.fa ../../data/pbsim2/ont_ref.fa

# Banded
./test_prog.out -k 64 -y local ../../data/pbsim2/ont_query.fa ../../data/pbsim2/ont_ref.fa
