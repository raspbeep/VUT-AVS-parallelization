#!/bin/bash

cd build/

ml intel-compilers/2022.1.0 CMake/3.23.1-GCCcore-11.3.0 matplotlib
CC=icc CXX=icpc cmake ..
make -j

executable="./PMC"
builder="--builder loop"
builder_ref="--builder ref"
input="../data/bun_zipper_res3.pts"
output_new="new_obj.obj"
output_ref="old_obj.obj"
args="--grid 128"


$executable $builder_ref $input $output_ref $args

runs=20
total_time=0
total_time_stdout=0

for i in $(seq 1 $runs)
do
    start_time=$(date +%s.%N)
    output=$($executable $builder $input $output_new $args)
    run_time_stdout=$(echo "$output" | grep "Elapsed Time:" | awk '{print $3}')
    total_time_stdout=$(echo "$total_time_stdout + $run_time_stdout" | bc -l)

    end_time=$(date +%s.%N)
    run_time=$(echo "$end_time - $start_time" | bc -l)
    total_time=$(echo "$total_time + $run_time" | bc -l)
    echo "Run $i: $run_time_stdout ms"

        echo "Checking objs."
        python3 ../scripts/check_output.py $output_new $output_ref
        check_ref_loop_status=$?
        if [ $check_ref_loop_status -eq 0 ]; then
            echo "[OK] LOOP - REF"
        else
            echo "[FAIL] LOOP - REF"
            exit 1
        fi
        echo "######################################"

done

average_time_stdout=$(echo "scale=2; $total_time_stdout / $runs" | bc -l)
echo "Average time STDOUT: $average_time_stdout ms"

average_time=$(echo "scale=3; $total_time / $runs" | bc -l)
echo "Average time: $average_time seconds"