#!/bin/bash

ml intel-compilers/2022.1.0 CMake/3.23.1-GCCcore-11.3.0 matplotlib
CC=icc CXX=icpc cmake ..

cd build/

echo "Deleting old obj files"
rm -f bun_ref.obj bun_loop.obj bun_tree.obj

make -j --quiet
make_status=$?


if [ $make_status -eq 0 ]; then
    echo "Build succeeded, running the main program..."
    # ./PMC --builder ref ../data/dragon_vrip_res1.pts bun_ref.obj
    # ref_status=$?
    ./PMC --builder loop ../data/dragon_vrip_res1.pts bun_loop.obj
    loop_status=$?
    ./PMC --builder tree ../data/dragon_vrip_res1.pts bun_tree.obj
    tree_status=$?
    
    if [ $ref_status -eq 0 ] && [ $loop_status -eq 0 ] && [ $tree_status -eq 0 ]; then
        # echo "Checking objs LOOP - REF."
        # python3 ../scripts/check_output.py bun_ref.obj bun_loop.obj
        # check_ref_loop_status=$?
        # if [ $check_ref_loop_status -eq 0 ]; then
        #     echo "[OK] LOOP - REF"
        # else
        #     echo "[FAIL] LOOP - REF"
        # fi

        echo "Checking objs TREE - REF."
        python3 ../scripts/check_output.py bun_loop.obj bun_tree.obj
        check_ref_tree_status=$?
        if [ $check_ref_tree_status -eq 0 ]; then
            echo "[OK] TREE - REF"
        else
            echo "[FAIL] TREE - REF"
        fi

    else
        echo "Testing failed, exiting."
    fi

else
    echo "Build failed, not testing."
fi
