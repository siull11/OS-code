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

for t in test1 test2
do
    echo "Running $t"
    for ((i=1; i<=$max_procs; i++))
    do 
        echo "with $i processes"
        echo "n, total, time" > "output/$t-$i.csv"
        for ((j=$max_procs_factorial; j<=$max_n; j*=$step_n))
        do
            mpirun -np $i ./$t $j $seed >> "output/$t-$i.csv"
        done
    done
done
