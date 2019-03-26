#!/bin/sh

#
# Run script for the Smith Waterman project.
# 
# "--serial" flag: 	runs serial algorithm
# "--omp" flag: 	runs parallel algorithm
# 
# if you have compiled only one of the two algorithms, it runs without prompt
#

file1="$PWD/serial"
file2="$PWD/omp"

if [ "$1" = "--serial" ]; then
	sel=1
elif [ "$1" = "--omp" ]; then
	sel=2
else
	if [ -f "$file1" ] && [ -f "$file2" ]; then
		echo "Which program do you want to run?"
		echo "1) Serial [default]"
		echo "2) OpenMP"
		read -p "Selection: " sel
		echo
	elif [ -f "$file1" ]; then
		sel=1
	elif [ -f "$file2" ]; then
		sel=2
	else
		echo "Compile a program first"
		exit
	fi
fi

read -p "Dataset you want to run: " d
read -p "Output name: " n
read -p "Match: " m
read -p "Mismatch: " mm
read -p "Gap: " g

if [ "${sel}" = "2" ]; then
read -p "Threads: " t
# 	valgrind --leak-check=full \
# 			--show-leak-kinds=all \
# 			--track-origins=yes \
# 			--verbose \
# 			--log-file=valgrind-out.txt \
	./omp -name "${n}" -input $PWD/Datasets/"${d}".txt -match "${m}" \
-mismatch "${mm}" -gap "${g}" -threads "${t}"
else
# 	valgrind --leak-check=full \
# 			--show-leak-kinds=all \
# 			--track-origins=yes \
# 			--verbose \
# 			--log-file=valgrind-out.txt \
	./serial -name "${n}" -input $PWD/Datasets/"${d}".txt -match "${m}" \
-mismatch "${mm}" -gap "${g}"
fi
