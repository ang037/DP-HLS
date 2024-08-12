#!/bin/bash

full_path=$(pwd)
DP_HLS_ROOT=$(dirname $(dirname "$full_path"))

MINIMAP_ROOT="${DP_HLS_ROOT}/baseline/software/minimap2"

valid_kernels=("local_linear" "local_affine" "global_linear" "global_affine" 
                "banded_local_linear" "two_piece_affine" "overlap_linear" "semiglobal_linear")

kernels=()
kernels_option_specified=false

usage() {
	echo "Usage: $0 [-kernels [local_linear] [local_affine] [global_linear] [global_affine]
					  [banded_local_linear] [two_piece_affine] [overlap_linear] [semiglobal_linear]]"
	exit 1
}

is_valid_kernel() {
  local kernel="$1"
  for valid in "${valid_kernels[@]}"; do
    if [[ "$kernel" == "$valid" ]]; then
      return 0
    fi
  done
  return 1
}

cd "$DP_HLS_ROOT/baseline/software/build"
echo

# Parse command-line options
while [[ "$#" -gt 0 ]]; do 
	case $1 in 
		-kernels)
			shift
			while [[ "$#" -gt 0 && ! "$1" =~ ^- ]]; do
        		if is_valid_kernel "$1"; then
          			kernels+=("$1")
        		else
          			echo "Error: Invalid kernel '$1'. Valid options are: ${valid_kernels[@]}."
          			exit 1
        		fi
        		shift
      		done
			;;
		*)
			usage
			;;
	esac
done
if [[ ${#kernels[@]} == 0 ]]; then
	echo "No kernels specified, running all kernel baselines by default"
	echo "Running baseline for global affine"
	echo "Performing 10 alignment passes over dataset and averaging them..."
	./global_affine	
	echo
	echo "Running baseline for global linear"
	echo "Performing 10 alignment passes over dataset and averaging them..."
	./global_linear
	echo
	echo "Running baseline for local linear"
	echo "Performing 10 alignment passes over dataset and averaging them..."
	./local_linear
	echo
	echo "Running baseline for local affine"
	echo "Performing 10 alignment passes over dataset and averaging them..."
	./local_affine
	echo
	echo "Running baseline for banded local linear"
	echo "Performing 10 alignment passes over dataset and averaging them..."
	./banded_local_linear
	echo		
	echo "Running baseline for overlap linear"
	echo "Performing 10 alignment passes over dataset and averaging them..."
	./overlap_linear
	echo
	echo "Running baseline for semi-global linear"
	echo "Performing 10 alignment passes over dataset and averaging them..."
	./semi_global_linear
	echo
	echo "Running baseline for two-piece affine"
	echo "Performing 10 alignment passes over dataset and averaging them..."
	cd "$MINIMAP_ROOT"
	total_throughput=0
	for i in {1..10}; do
		start_time=$(date +%s%6N)
		./minimap2 -t 8 ../data/pbsim2/ont_ref.fa ../data/pbsim2/ont_query.fa
		end_time=$(date +%s%6N)
		elapsed_time=$((end_time - start_time))
		throughput=$(echo "scale=6; 1000 / ($elapsed_time / 1000000.0)" | bc)
		echo "Elapsed time: $elapsed_time microseconds"
		echo "Throughput: $throughput alignments/second"
		total_throughput=$(echo "$total_throughput + $throughput" | bc)
	done
	echo "Average throughput: $(echo "$total_throughput / 10" | bc) alignments/second"
	echo
else 
	echo "Kernels specified: ${kernels[@]}"
	for kernel in "${kernels[@]}"; do
		if [[ "$kernel" == "global_affine" ]]; then
			echo "Running baseline for global affine"
			echo "Performing 10 alignment passes over dataset and averaging them..."
			./global_affine
			echo
		fi
		if [[ "$kernel" == "global_linear" ]]; then
			echo "Running baseline for global linear"
			echo "Performing 10 alignment passes over dataset and averaging them..."
			./global_linear
			echo
		fi
		if [[ "$kernel" == "local_linear" ]]; then
			echo "Running baseline for local linear"
			echo "Performing 10 alignment passes over dataset and averaging them..."
			./local_linear
			echo
		fi
		if [[ "$kernel" == "local_affine" ]]; then
			echo "Running baseline for local affine"
			echo "Performing 10 alignment passes over dataset and averaging them..."
			./local_affine
			echo
		fi 
		if [[ "$kernel" == "banded_local_linear" ]]; then
			echo "Running baseline for banded local linear"
			echo "Performing 10 alignment passes over dataset and averaging them..."
			./banded_local_linear
			echo 
		fi
		if [[ "$kernel" == "overlap_linear" ]]; then
			echo "Running baseline for overlap linear"
			echo "Performing 10 alignment passes over dataset and averaging them..."
			./overlap_linear
			echo
		fi 
		if [[ "$kernel" == "semiglobal_linear" ]]; then
			echo "Running baseline for semi-global linear"
			echo "Performing 10 alignment passes over dataset and averaging them..."
			./semi_global_linear
			echo
		fi 
		if [[ "$kernel" == "two_piece_affine" ]]; then
			echo "Running baseline for two-piece affine"
			echo "Performing 10 alignment passes over dataset and averaging them..."
			cd "$MINIMAP_ROOT"
			total_throughput=0
			for i in {1..10}; do
				start_time=$(date +%s%6N)
				./minimap2 -t 8 ../data/pbsim2/ont_ref.fa ../data/pbsim2/ont_query.fa
				end_time=$(date +%s%6N)
				elapsed_time=$((end_time - start_time))
				throughput=$(echo "scale=6; 1000 / ($elapsed_time / 1000000.0)" | bc)
				echo "Elapsed time: $elapsed_time microseconds"
				echo "Throughput: $throughput alignments/second"
				total_throughput=$(echo "$total_throughput + $throughput" | bc)
			done
			echo "Average throughput: $(echo "$total_throughput / 10" | bc) alignments/second"

			echo
		fi
	done
fi
echo "Finished running baselines"
