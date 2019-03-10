#!/bin/sh
gcc -Wall -o test -ggdb3 test_vc.c
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-out.txt \
            ./test -name output -input $PWD/Datasets/D5.txt -match 1 -mismatch -4 -gap 2
