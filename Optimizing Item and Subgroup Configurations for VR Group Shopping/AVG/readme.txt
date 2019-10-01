============ CONTENT IN THE FOLDER ============

1. Source codes of the proposed AVG, AVG-D, and IP.
2. Timik sampled data. Note that the whole dataset is not uploaded due to space limit.

============ EXPERIMENT ENVIRONMENT ============

CentOS 7
gcc 4.8.5
OpenMP 3.1
ampl 20160704 (Linux x86\_64)
Gurobi 8.0
ampl-C++ interface
ampl-Gurobi interface
Python 3.5 (for data parsers)
All algorithms are implemented in an HP DL580 Gen 9 server with four 3.0 GHz Intel CPUs and 1TB RAM. The code is parallelized with 150 cores in runtime.

============ MAKEFILES and PROGRAM EXECUTION ============

1. Replace the ampl path in the makefile first
2. Compile commands:
make
make random
make derandom
make ilp
3. AVG execution:
./random.exe <input name> <mode> <thread#> <timelimit> <filter_size> <discount> <lambda> <group_size_ub>
eg. ./random.exe data.dat 1 5 100 10 0 0.5 5

** mode 1: precise mode (lp converges when error < 1e-4)
** mode 0: efficient mode (lp converges when error < 1e-1)
** filter_size: prefilter item before lp, retrieve top-[filter_size] liked items for each user
** filter_size should be at least position number
** discount: 0 for aligned problem, > 0 for unaligned problem
** group_size_ub: max subgroup size; should ONLY be used with aligned problem (discount=0)

4. AVG-D execution:
./derandom.exe <input name> <mode> <thread#> <timelimit> <filter_size> <discount> <lambda> <r>
eg. ./derandom.exe data.dat 1 5 100 10 0 0.5 0.25

**parallel computing for derandom.exe is disabled by default.
To setup: 
a) setup env var for OpenMP. In Linux, set “export OMP_THREAD_NUMS=150” for 150 cores.
b) Uncomment the pragma for loop in VRPR_derandomized.cpp
c) Note that this sometimes causes runtime error (related to memory issues)

5. IP execution:
./ilp.exe <input name> <thread#> <timelimit>
eg. ./ilp.exe data.dat 5 100

============ SAMPLE DATA ============

1. data.dat is a small sample used in Section 5.2.
2. timik_data/timik_u15_i5000_<n>.txt is the nth timik sample with n=15, m=5000, and k=10. 
3. The the third line ("param pos:= 10;") in each sample specifies the number of k. Any integer ranging from 1 to 5000 is legal.
