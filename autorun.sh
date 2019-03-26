#!/bin/sh

#
# Compile and run script for the Smith Waterman project
# "--cnr" flag: 		compile and run
# "--compile" flag: 	compile only
# "--run" flag: 		run only
# "--clean" flag: 		removes generated files
#

if [ "$1" = "--cnr" ]; then
	sel=1
elif [ "$1" = "--compile" ]; then
	sel=2
elif [ "$1" = "--run" ]; then
	sel=3
elif [ "$1" = "--clean" ]; then
	sel=4
else
	echo "Choose an action:"
	echo "1) Compile and run [default]"
	echo "2) Compile only"
	echo "3) Run only"
	echo "4) Clean project"
	read -p "Selection: " sel
	echo 
fi

if [ "${sel}" = "4" ]; then
	if [ -f serial ]; then 
		rm serial
	fi
	if [ -f omp ]; then 
		rm omp
	fi
	if [ -f Report_* ]; then 
		rm Report_*
	fi
elif [ "${sel}" = "3" ]; then 
	sh run.sh
elif [ "${sel}" = "2" ]; then
	sh compile.sh
else
	sh compile.sh
	if [ "$?" = 1 ]; then
		flag="--serial"
	else
		flag="--omp"
	fi
	sh run.sh "${flag}"
fi
