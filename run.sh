#!/bin/sh
file1="$PWD/serial"
file2="$PWD/omp"

if [ -f "$file1" ] && [ -f "$file2" ]; then
	echo "Which program do you want to run?"
	echo "1) Serial [default]"
	echo "2) OpenMP"
	
	read -p "Selection: " sel
elif [ -f "$file1" ]; then
	sel=1
elif [ -f "$file2" ]; then
	sel=2
else
	echo "Compile a program first"
	exit
fi

read -p "Dataset you want to run: " d
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
	./omp -name output -input $PWD/Datasets/D"${d}".txt -match "${m}" \
-mismatch "${mm}" -gap "${g}" -threads "${t}"
else
# 	valgrind --leak-check=full \
# 			--show-leak-kinds=all \
# 			--track-origins=yes \
# 			--verbose \
# 			--log-file=valgrind-out.txt \
	./serial -name output -input $PWD/Datasets/D"${d}".txt -match "${m}" \
-mismatch "${mm}" -gap "${g}"
fi
