#!/bin/bash

PAPI_OUTPUT_DIR="/N/u/nsarwar/Quartz/git-repository/High-Performance-Computing/papi_output_row_major"

EXECUTABLE="./exec"  # Change this if necessary

echo "Loop Order, Matrix Size, PAPI_FP_OPS, Cache Misses" > RM_results.txt

# Function to run matrix multiplication and collect results
run_experiment() {
    local size=$1
    echo "Running matrix multiplication for size $size ..."

    timeout 60s srun "$EXECUTABLE" $size $size $size
    exit_code=$?  # Capture exit code of timeout

    if [ $exit_code -eq 124 ]; then
        echo "Matrix size $size took too long. Stopping further tests."
        return 1  # Stop the function and indicate timeout occurred
    fi

    latest_dir=$(ls -td "$PAPI_OUTPUT_DIR"/*/ | head -1)
    latest_json=$(ls -t "$latest_dir"/*.json | head -1)

    for order in "ijk" "ikj" "jik" "jki" "kij" "kji"; do
        PAPI_FP_OPS=$(jq -r ".threads.\"0\".regions | to_entries[] | select(.value.name == \"$order\") | .value.PAPI_FP_OPS" "$latest_json")
        CACHE_MISSES=$(jq -r ".threads.\"0\".regions | to_entries[] | select(.value.name == \"$order\") | .value[\"PERF_COUNT_HW_CACHE_L1D:MISS\"]" "$latest_json")

        echo "$order, $size, $PAPI_FP_OPS, $CACHE_MISSES" >> RM_results.txt
    done
    return 0
}

# Testing for multiples of 10 from 10 to 100
for size in {10..100..10}; do
    run_experiment $size || break
done

# Testing for multiples of 100 from 200 to 1000
for size in {200..1000..100}; do
    run_experiment $size || break
done

# Testing for multiples of 1000 from 2000 onwards
for size in {2000..10000..1000}; do
    run_experiment $size || break
done

echo "Experiment completed. Results saved in RM_results.txt"
