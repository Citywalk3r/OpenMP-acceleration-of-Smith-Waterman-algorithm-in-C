#!/bin/sh

#
# Compiling script for the Smith Waterman project
#

LD_SRC="src/"
LD_INC="include/"

lib0=""${LD_SRC}"generic.c"
lib1=""${LD_SRC}"SERIAL_functions.c"
lib2=""${LD_SRC}"SERIAL_ANTIDIAG_functions.c"
# No need to use OMP_functions, it is the same as SERIAL,
# it stayed just for extensibility of the code
#
#lib3=""${LD_SRC}"/OMP_functions.c"
lib4=""${LD_SRC}"PTHREAD_functions.c"
lib5=""${LD_SRC}"pthread_pool.c"

sc1="static, (int)(q_limit+d_limit)/(2*threads)"
sc2="dynamic, 1"
sc3="guided, 1"

# #
# # Compilation of 2 serial variations.
# #
# # linear traverses the matrix line by line, element by element
# # antidiag traverses the matrix on the antidiagonal (same logic as our 
# #  parallel implementations, for comparison)
# #
# gcc -D_GNU_SOURCE -Wall -ggdb3 -I"${LD_INC}" "${lib0}" "${lib1}" \
# 	Smith-Waterman.c -o serial_linear
# gcc -D_GNU_SOURCE -Wall -ggdb3 -I"${LD_INC}" "${lib0}" "${lib2}" \
# 	Smith-Waterman.c -o serial_antidiag
# 
# # #
# # # Compilation of 3 fine grain variations (differ on schedule) (OMP)
# # #
# # # static assigns big chunks of tasks to each thread
# # # dynamic assigns a one-task-at-a-time to each thread 
# # # guided starts as static and finishes as dynamic, based on tasks left
# # #
# # gcc -D_GNU_SOURCE -Dsched="${sc1}" -I"${LD_INC}" \
# # 	-fopenmp -Wall -ggdb3 "${lib0}" "${lib2}" \
# # 	Smith-Waterman.c -o omp_fine_static
# # gcc -D_GNU_SOURCE -Dsched="${sc2}" -I"${LD_INC}" \
# # 	-fopenmp -Wall -ggdb3 "${lib0}" "${lib2}" \
# # 	Smith-Waterman.c -o omp_fine_dynamic
# # #
# # # Only guided is compiled as the best of the implementations tested. You can
# # # uncomment the rest in here and the run.sh script to enable them.
# # #
# gcc -D_GNU_SOURCE -Dsched="${sc3}" -I"${LD_INC}" \
# 	-fopenmp -Wall -ggdb3 "${lib0}" "${lib2}" \
# 	Smith-Waterman.c -o omp_fine_guided
# 
# #
# # Compilation of coarse grain (OMP)
# #	
# gcc -D_GNU_SOURCE -I"${LD_INC}" -fopenmp -Wall -ggdb3 \
# 	"${lib0}" "${lib2}" Smith-Waterman_coarse.c -o omp_coarse

#
# Compilation of fine grain (PTHREADS) 
#	
gcc -D_GNU_SOURCE -I"${LD_INC}" -lpthread -ggdb3 \
	"${lib0}" "${lib4}" "${lib5}" Smith-Waterman.c -o pthread_fine
