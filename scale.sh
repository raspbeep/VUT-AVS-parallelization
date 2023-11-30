#!/bin/bash

ml intel-compilers/2022.1.0 CMake/3.23.1-GCCcore-11.3.0 matplotlib
CC=icc CXX=icpc cmake ..

cd build/
make -j --quiet

for i in $(seq 1 5)
do
    echo "Deleting old csv files"
    rm -f /grid_scaling_out.csv
    echo "Measuring $i"
    sh ../scripts/measure_grid_scaling.sh
    echo "generating plots"
    python3 ../scripts/generate_plots.py ./grid_scaling_out.csv grid_scale$i.png grid_scaling
    echo "###############"
done
