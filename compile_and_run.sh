#!/bin/sh
if [ "$1" == "-d" ]; then
	d="11"
	m="3"
	mm="-1"
	g="-1"
else
	read -p "Dataset you want to run: " d
	read -p "Match: " m
	read -p "Mismatch: " mm
	read -p "Gap: " g
fi
gcc -Wall -o test -ggdb3 test_vc.c
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-out.txt \
            ./test -name output -input $PWD/Datasets/D"${d}".txt -match "${m}" -mismatch "${mm}" -gap "${g}"
