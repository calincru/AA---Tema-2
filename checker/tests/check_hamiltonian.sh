#!/bin/bash

c++ -std=c++0x check_hamiltonian.cpp

for i in correctness/input/*; do
    cp $i ./test.in
    printf "$(basename $i) $(./a.out)\n"
done

rm a.out
rm test.in
