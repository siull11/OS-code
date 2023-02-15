#!/bin/bash
echo "Compiling"
mpiCC test1.cpp my_mpi.cpp -o test1
mpiCC test2.cpp my_mpi.cpp -o test2

seed=0
max_procs=16
start_n=100000
# max_procs_factorial=$(seq -s "*" 1 $max_procs |bc)
max_n=100000000
step_n=2
runs=10

for t in test1 test2
do
    echo "Running $t"
    for ((i=1; i<=$max_procs; i++)) # i*=2)) 
    do 
        echo "with $i processes"
        echo "n, total, time" > "output/$t-$i.csv"
        n=$(( $start_n / $i ))
        for ((j=$n*$i; j<=$max_n; j*=$step_n))
        do
            for ((r=1; r<=$runs; r++))
            do
                mpirun -np $i ./$t $j $seed >> "output/$t-$i.csv"
            done
        done
    done
done
