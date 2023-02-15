#!/bin/bash
echo "Compiling"
mpiCC test1.cpp my_mpi.cpp -o test1
mpiCC test2.cpp my_mpi.cpp -o test2

seed=0
max_procs=4
max_procs_factorial=$(seq -s "*" 1 $max_procs |bc)
max_n=200 #10000000
step_n=2

output_file1="output/test1.csv"
output_file2="output/test2.csv"

echo "n, total, time" > $output_file1
echo "Running test 1"
for ((i=$max_procs_factorial; i<=$max_n; i*=$step_n))
do
    for ((j=1; j<=$max_procs; j++))
    do
        mpirun -np $j ./test1 $i $seed >> $output_file1
    done
done

echo "n, max, time" > $output_file2
echo "Running test 2"
for ((i=$max_procs_factorial; i<=$max_n; i*=$step_n))
do
    for ((j=1; j<=$max_procs; j++))
    do
        mpirun -np $j ./test2 $i $seed >> $output_file2
    done
done