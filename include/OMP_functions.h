/*
 *  Functions used specifically by OMP implementations
 */

/* 
 * At the moment this header is exactly like its SERIAL counterpart, but for 
 * scalability reasons we keep both files 
 */

#ifndef OMP_FUNC_H
#define OMP_FUNC_H

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
 *  threads: number of threads for openmp
 * 	q,d: q and d strings
 * 	*max_score: pointer to fill the max_score stack (call-by-reference)
 * 	*calc_time: pointer to count runtime (call-by-reference)
 * 
 * 	returns: 0 ok
 */
extern int calculate_score(int** score_matrix, int match, int mismatch, \
						   int gap, int threads, char* q, char* d, \
						   MVP* *max_score,	double *calc_time);

#endif
