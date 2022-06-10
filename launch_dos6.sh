#!/bin/bash

mkdir -p out/dos6

mpirun -n 64 --hostfile o64_s1.par time -p ./build/par.out 2>> out/dos6/1_64_64.out 
mpirun -n 64 --hostfile o64_s2.par time -p ./build/par.out 2>> out/dos6/2_32_64.out
mpirun -n 64 --hostfile o64_s4.par time -p ./build/par.out 2>> out/dos6/4_16_64.out

mpirun -n 128 --hostfile o64_s2.par time -p ./build/par.out 2>> out/dos6/2_64_128.out
mpirun -n 128 --hostfile o64_s4.par time -p ./build/par.out 2>> out/dos6/4_32_128.out

mpirun -n 256 --hostfile o64_s4.par time -p ./build/par.out 2>> out/dos6/4_64_256.out
