#!/bin/sh

#
# Compiling script for the Smith Waterman project
#
# "--serial" flag: 	compile the serial code directly without prompt
# "--omp" flag: 	proceed to parallel code compilation without initial prompt
#

LD_SRC="src/"
LD_INC="include/"

if [ "${1}" = "--serial" ]; then
	imp=1
elif [ "${1}" = "--omp" ]; then
	imp=2
else
	echo "Which implementation would you like to compile?"
	echo "1) Serial [default]"
	echo "2) Parallel (OpenMP)"
	read -p "Selection: " imp
	echo
fi

if [ "${imp}" = "1" ]; then
	echo "Which Serial variant would you like to compile?"
	echo "1) Antidiagonal loop approach [default]"
	echo "2) Serial traversal approach"
	read -p "Selection: " sel
	echo

	if [ "${sel}" = "2" ]; then
		lib=""${LD_SRC}"/SERIAL_functions.c"
	else
		lib=""${LD_SRC}"/SERIAL_ANTIDIAG_functions.c"
	fi
fi

if [ "${imp}" = "2" ]; then
	echo "Which method of Parallelization would you like to compile?"
	echo "1) Static: Each thread is assigned a chunk of tasks to work \
[default]"
	echo "2) Dynamic: Each thread is assigned a single task to work"
	echo "3) Guided: Starts as static and reduces size to dynamic based \
on processes left"
	read -p "Selection: " sel
	echo

	if [ "${sel}" = "2" ]; then
		sc="dynamic, 1"
	elif [ "${sel}" = "3" ]; then
		sc="guided, 1"
	else
		sc="static, (int)(q_limit+d_limit)/(2*threads)"
	fi
fi

if [ "${imp}" = "2" ]; then
	gcc -D_GNU_SOURCE -Dsched="${sc}" -I"${LD_INC}" \
		-fopenmp -Wall -ggdb3 \
		"${LD_SRC}"/generic.c "${LD_SRC}"/OMP_functions.c \
		Smith-Waterman_omp.c -o omp
	exit 2
else
	gcc -D_GNU_SOURCE -Wall -ggdb3 -I"${LD_INC}" \
		"${LD_SRC}"/generic.c "${lib}" \
		Smith-Waterman_serial.c -o serial
	exit 1
fi
