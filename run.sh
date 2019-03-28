#!/bin/sh

#
# Run script for the Smith Waterman project.
#

echo "Which program do you want to run?"
echo "1) Serial linear [default]"
echo "2) Serial antidiag"
echo "3) OpenMP fine static"
echo "4) OpenMP fine dynamic"
echo "5) OpenMP fine guided"
read -p "Selection: " sel
echo

read -p "Dataset you want to run: " d
read -p "Output name: " n
read -p "Match: " m
read -p "Mismatch: " mm
read -p "Gap: " g

if [ "${sel}" = "5" ]; then
	ex="omp_fine_guided"
elif [ "${sel}" = "4" ]; then
	ex="omp_fine_dynamic"
elif [ "${sel}" = "3" ]; then
	ex="omp_fine_static"
elif [ "${sel}" = "2" ]; then
	ex="serial_antidiag"
else
	ex="serial_linear"
fi

if [ "${sel}" != "1" ] && [ "${sel}" != "2" ]; then 
	read -p "Threads: " t
	./"${ex}" -name ""${n}"_OMP_"${t}"" -input $PWD/Datasets/"${d}".txt \
			  -match "${m}" -mismatch "${mm}" -gap "${g}" -threads "${t}"
else
	./"${ex}" -name "${n}" -input $PWD/Datasets/"${d}".txt -match "${m}" \
			  -mismatch "${mm}" -gap "${g}"
fi
