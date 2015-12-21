#!/bin/bash

input_dir=./tests/correctness/input
output_dir=./tests/correctness/output
ref_dir=./tests/correctness/ref

tests_nr=$(ls $input_dir | wc -w)
points_per_test=$((100/tests_nr))

# Prepare function equivalence checker
make -f Makefile.robdd

# Build
make
if [ $? -ne 0 ]; then
    "Makefile failed.  Aborting..."
else
    # Prepare checker
    total_points=0
    mkdir -p $output_dir

    # Run tests
    for ((i=1; i<=$tests_nr; i++)); do
        printf "Test $i: "
        cp $input_dir/test${i}.in ./test.in
        make run &>/dev/null
        if [ $? -eq 0 ]; then
            mv test.out $output_dir/test${i}.out
            equiv=$(./robdd $output_dir/test${i}.out $ref_dir/test${i}.ref)
            if [ "$equiv" == "1" ]; then
                printf "Success"
                total_points=$((total_points+points_per_test))
            else
                printf "Fail"
            fi
        else
            printf "Run target failed"
        fi
        rm ./test.in
        printf "\n"
    done
fi

# Wrap up
echo "TOTAL: " $total_points
make clean
make -f Makefile.robdd clean
