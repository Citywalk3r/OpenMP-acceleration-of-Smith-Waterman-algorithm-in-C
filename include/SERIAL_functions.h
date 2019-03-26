#ifndef SERIAL_FUNC_H
#define SERIAL_FUNC_H

/*
 * Function: concat
 * ----------------
 * 	Concatenation function, used to create the output file path
 * 
 * 	s1-4: strings to concat
 * 
 * 	returns: concatenated string, or empty string
 */
extern char* concat(const char* s1, const char* s2, const char* s3, const char* s4);

 /*
 * Function: init_parsing
 * ----------------------
 * 	This function is scanning the args of the program when it's called.
 * 	Implements a simple error handling for wrong/unknown inputs and formats
 * 
 * 	count: argc
 * 	*vector: argv
 *  *name: pointer to fill output name (call-by-reference)
 * 	*input: pointer to fill input file path (call-by-reference)
 * 	*match: pointer to fill match (call-by-reference)
 * 	*mismatch: pointer to fill mismatch (call-by-reference)
 *  *gap: pointer to fill gap (call-by-reference)
 * 
 * 	returns: 0 ok, 1 error
 */
extern int init_parsing(int count, char* *vector, char* *name, char* *input,\
				int *match, int *mismatch, int *gap);

/*
 * Function: calculate_score
 * -------------------------
 * 	This function gets an initialized, zero-filled score matrix and calculates 
 * 	each cell's value according to the Smith Waterman algorithm. It also keeps
 * 	track of the max value and the coordinates of the cells containing it,
 * 	using the MVP stack 
 * 
 * 	score_matrix: 2d array for q,d alignment
 * 	match, mismatch, gap: parameters of the algorithm
 * 	q,d: q and d strings
 * 	*max_score: pointer to fill the max_score stack (call-by-reference)
 * 	*calc_time: pointer to count runtime (call-by-reference)
 * 
 * 	returns: 0 ok
 */
extern int calculate_score(int** score_matrix, int match, int mismatch, int gap,\
					char* q, char* d, MVP* *max_score, double *calc_time);


/*
 * Function: calculate_score_omp
 * -----------------------------
 * 	Parallel implementation of the above function using OpenMP
 * 
 * 	score_matrix: 2d array for q,d alignment
 * 	match, mismatch, gap: parameters of the algorithm
 *  threads: number of threads for openmp
 * 	q,d: q and d strings
 * 	*max_score: pointer to fill the max_score stack (call-by-reference)
 * 	*calc_time: pointer to count runtime (call-by-reference)
 * 
 * 	returns: 0 ok
 */
extern int calculate_score_omp(int** score_matrix, int match, int mismatch, int gap,\
					int threads, char* q, char* d, MVP* *max_score,\
					double *calc_time);

#endif
