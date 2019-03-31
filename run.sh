#!/bin/sh

#
# Run script for the Smith Waterman project.
#

echo "Which program do you want to run?"
echo "1) Serial linear [default]"
echo "2) Serial antidiag"
echo "3) OpenMP fine guided"
echo "4) OpenMp coarse"
echo "5) Pthreads fine"
# echo "7) OpenMP fine static"
# echo "8) OpenMP fine dynamic"
read -p "Selection: " sel
echo

read -p "Dataset you want to run: " d
read -p "Output name: " n
read -p "Match: " m
read -p "Mismatch: " mm
read -p "Gap: " g


if [ "${sel}" = "5" ]; then
	ex="pthread_fine"
elif [ "${sel}" = "4" ]; then
	ex="omp_coarse"
elif [ "${sel}" = "3" ]; then
	ex="omp_fine_guided"
elif [ "${sel}" = "2" ]; then
	ex="serial_antidiag"
# elif [ "${sel}" = "8" ]; then
# 	ex="omp_fine_dynamic"
# elif [ "${sel}" = "7" ]; then
# 	ex="omp_fine_static"
else
	ex="serial_linear"
fi

if [ "${sel}" != "1" ] && [ "${sel}" != "2" ] && [ "${sel}" != "5" ]; then 
	read -p "Threads: " t
	export OMP_WAIT_POLICY=active
	export OMP_DYNAMIC=false
	export OMP_PROC_BIND=true
	./"${ex}" -name ""${n}"_OMP_"${t}"" -input $PWD/Datasets/"${d}".txt \
			  -match "${m}" -mismatch "${mm}" -gap "${g}" -threads "${t}"
elif [ "${sel}" = "5" ]; then
	read -p "Threads: " t
	./"${ex}" -name ""${n}"_PTH_"${t}"" -input $PWD/Datasets/"${d}".txt \
			  -match "${m}" -mismatch "${mm}" -gap "${g}" -threads "${t}"		  
else
	./"${ex}" -name "${n}" -input $PWD/Datasets/"${d}".txt -match "${m}" \
			  -mismatch "${mm}" -gap "${g}"
fi
