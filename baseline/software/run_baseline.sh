#!/bin/bash

full_path=$(pwd)
DP_HLS_ROOT=$("$full_path" | cut -d'/' -f1-4)

MINIMAP_ROOT="${DP_HLS_ROOT}/baseline/software/minimap2"

cd "$DP_HLS_ROOT/baseline/software/build"
make

echo 

echo "Running baseline for global affine"
./global_affine

echo

echo "Running baseline for global linear"
./global_linear

echo

echo "Running baseline for local linear"
./local_linear

echo

echo "Running baseline for local affine"
./local_affine

echo 

echo "Running baseline for banded local linear"
./banded_local_linear

echo 

echo "Running baseline for overlap linear"
./overlap_linear

echo 

echo "Running baseline for semi-global linear"
./semi_global_linear

echo 

echo "Running baseline for two-piece affine"
cd "$MINIMAP_ROOT"

start_time=$(date +%s%6N)

./minimap2 -t 8 ../ont_ref.fa ../ont_query.fa

end_time=$(date +%s%6N)

elapsed_time=$((end_time - start_time))

throughput=$(echo "scale=6; 1000 / ($elapsed_time / 1000000.0)" | bc)

echo "Elapsed time: $elapsed_time microseconds"
echo "Throughput: $throughput alignments/second"
