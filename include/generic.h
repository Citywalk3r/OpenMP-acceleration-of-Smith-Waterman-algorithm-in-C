#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <sys/time.h>

extern char* name_flag;
extern char* input_flag;
extern char* match_flag;
extern char* mismatch_flag;
extern char* gap_flag;
extern char* threads_flag;
extern int line_size;

typedef struct MVP{
    unsigned int q;
	unsigned int d;
	int value;
    struct MVP *next;
}MVP;

 /*
 * Function: push
 * --------------
 * 	A simple push function for the max-value stack 
 * 
 * 	*max_score: pointer to head of stack
 * 	q: the position on the q string
 * 	d: the position on the d string
 *  value: value of the cell [q][d]
 * 
 * 	returns: 0 ok, 1 error
 */
extern int push(MVP* *max_score, unsigned int q, unsigned int d, int value);

/*
 * Function: pop
 * -------------
 * 	A simple pop function for the max-value stack
 * 
 * 	*max_score: pointer to head of stack
 * 
 * 	returns: 0 ok, 1 error
 */
extern int pop(MVP* *max_score);

/*
 * Function: empty
 * ---------------
 * 	Empties the stack utilizing pop()
 * 
 * 	*max_score: pointer to head of stack
 * 
 * 	returns: 0 ok
 */
extern int empty(MVP* *max_score);

/*
 * Function: gettime
 * -----------------
 * 	A simple timestamp function, as given in the project description
 * 
 * 	returns: the exact time when it is called
 */
extern double gettime(void);
 
 /*
 * Function: max
 * -------------
 * 	Max function
 * 
 * 	a,b,c: values
 * 	pos: position of the max element (0-2)
 * 
 * 	returns: max element
 */
extern int max(int a, int b, int c, int* pos);
 
 /*
 * Function: pretty_print
 * ----------------------
 * 	This function takes the results of traceback and word wraps them
 * 	to line_len size lines while printing them to output file
 * 
 * 	out_file: the output file to print the results
 * 	q,d: back traced q and d strings
 */
extern void pretty_print(FILE* out_file, char* q, char* d, int line_len);
 
/*
 * Function: file_open
 * -------------------
 * 	This function opens the input and output files, using the previous args.
 * 	Saves the output file on the current working directory.
 * 	Implements a simple error handling.
 * 
 * 	input: input file path
 * 	*in_file: pointer to fill input file (call-by-reference)
 * 	*out_file: pointer to fill output file (call-by-reference)
 * 	out_source: output file path
 * 
 * 	returns: 0 ok, 1 error
 */
extern int file_open(char* input, FILE* *in_file, FILE* *out_file,\
				char* out_source);

/*
 * Function: header_parse
 * ----------------------
 * 	This function parses the initial data on the head of the input file.
 * 	It produces the data used on the initialization of our data structures.
 * 	It doesn't take into consideration Q_Sz_Min, as we evaluate the size of
 * 	each pair on runtime. Yet again, it implements a simple error handling
 * 
 * 	in_file: input file
 * 	*pair_size: pointer to fill pair size (call-by-reference)
 * 	*q_size: pointer to fill q size (call-by-reference)
 * 	*d_size: pointer to fill d size (call-by-reference)
 * 
 * 	returns: 0 ok, 1 error
 */
extern int header_parse(FILE* in_file, int *pair_size, unsigned int *q_size,\
				unsigned int *d_size);

/*
 * Function: parse_file
 * --------------------
 * 	This function parses the input file until it finds a whole Q-D pair.
 * 	It uses the dataset's property of having an empty line after each pair.
 * 	To be sure that a whole pair is read, we have a flag that shows if we read
 * 	a Q or a D value. Along with the error handling, it is used to determine
 * 	when we can stop parsing (empty line after D is read)
 * 
 * 	in_file: input file
 * 	out_file: output file
 * 	q: q string
 * 	d: d string
 * 
 * 	returns: 0 ok, 1 error, 2 parsing is finished
 */
extern int parse_file(FILE* in_file, FILE* out_file, char* q, char* d);

/*
 * Function: traceback
 * -------------------
 * 	This function takes the filled score matrix and the MVP stack and traces 
 * 	back all the max values to their root. It then prints out to the out-file
 * 
 * 	score_matrix: 2d array for q,d alignment
 * 	*max_score: pointer to use the max_score stack
 * 	out_file: the output file to print the results
 * 	q,d: q and d strings
 * 	*steps: pointer to count number of traceback steps (call-by-reference)
 * 	*tb_time: pointer to count runtime (call-by-reference)
 * 
 * 	returns: 0 ok, 1 error
 */
int traceback(int** score_matrix, MVP* *max_score, FILE* out_file, char* q,\
				char* d, unsigned int *steps, double *tb_time);

#endif
