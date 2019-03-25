#!/bin/sh

if [ "$1" == "-d" ]; then
	sc="dynamic, 1"
	shift
else
	echo "Which implementation would you like to compile?"
	echo "1) Serial [default]"
	echo "2) Parallel (OpenMP)"
	read -p "Selection: " imp

	if [ "${imp}" == "2" ]; then
		echo "Which method of Parallelization would you like to compile?"
		echo "1) Static: Each thread is assigned a chunk of tasks to work \
[default]"
		echo "2) Dynamic: Each thread is assigned a single task to work"
		echo "3) Guided: Starts as static and reduces size to dynamic based \
on processes left"
		read -p "Selection: " sel

		if [ "${sel}" == "2" ]; then
			sc="dynamic, 1"
		elif [ "${sel}" == "3" ]; then
			sc="guided, 1"
		else
			sc="static, (int)(q_limit+d_limit)/(2*threads)"
		fi
	fi
fi

if [ "${imp}" == "2" ]; then
	gcc -D_GNU_SOURCE -Dsched="${sc}" -fopenmp -Wall -ggdb3 \
 generic.c OMP_functions.c Smith-Waterman_omp.c -o omp
else
	gcc -D_GNU_SOURCE -Wall -ggdb3 \
 generic.c SERIAL_functions.c Smith-Waterman_serial.c -o serial
fi
