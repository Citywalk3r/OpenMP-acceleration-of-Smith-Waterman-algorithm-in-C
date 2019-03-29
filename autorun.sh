#!/bin/sh

#
# Compile and run script for the Smith Waterman project
# "--cnr" flag: 		compile and run
# "--compile" flag: 	compile only
# "--run" flag: 		run only
# "--perf" flag:		check for cache misses (needs "perf" library installed)
# "--clean" flag: 		removes generated files
#

if [ "$1" = "--cnr" ]; then
	sel=1
elif [ "$1" = "--compile" ]; then
	sel=2
elif [ "$1" = "--run" ]; then
	sel=3
elif [ "$1" = "--perf" ]; then
	sel=4
elif [ "$1" = "--clean" ]; then
	sel=5
else
	echo "Choose an action:"
	echo "1) Compile and run [default]"
	echo "2) Compile only"
	echo "3) Run only"
	echo "4) Cache Analysis (needs \"perf\" library installed)"
	echo "5) Clean project"
	read -p "Selection: " sel
	echo
fi

if [ "${sel}" = "5" ];then
	rm serial_*
	rm omp_*
	rm Report_*
	echo "Cleared all generated files"
elif [ "${sel}" = "4" ];then
	sh perf.sh
elif [ "${sel}" = "3" ]; then
	sh run.sh
elif [ "${sel}" = "2" ]; then
	sh compile.sh
else
	sh compile.sh
	sh run.sh "${flag}"
fi
