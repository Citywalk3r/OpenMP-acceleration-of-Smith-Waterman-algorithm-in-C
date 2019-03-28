#include "generic.h"
#include "SERIAL_functions.h"

int calculate_score(int** score_matrix, int match, int mismatch, int gap,\
					int threads, char* q, char* d, MVP* *max_score,\
					double *calc_time){
    
	double start_time = gettime();
    int diagonal, up, left;
	int trash = 0;
	int q_limit = strlen(q);
	int d_limit = strlen(d);
    push(max_score, 0, 0, 0);
	
    for (unsigned int i = 1; i <= q_limit; i++){
        for (unsigned int j = 1; j <= d_limit; j++){
			/* 
			 * q = i-1, d = j-1, cause score_matrix zero pads q and d
			 * for initialization 
			 */
            diagonal = q[i-1] == d[j-1] ? (score_matrix[i-1][j-1] + match) \
											: score_matrix[i-1][j-1] + mismatch;
            up = score_matrix[i-1][j] + gap;
            left = score_matrix[i][j-1] + gap;
			
			/* priority in max: diagonal > up > left */
			score_matrix[i][j] = max(diagonal, up, left, &trash);
			
			/* if value is less than zero, replace with zero */
			if (score_matrix[i][j] < 0){
				score_matrix[i][j] = 0;
			}
			/* 
			 * if new value is greater than the old max, empty the max stack
			 * and replace the max value with new value
			 */
            if ((*max_score)->value < score_matrix[i][j]){
                empty(max_score);
                push(max_score, i-1, j-1, score_matrix[i][j]);
			/* 
			 * if value is equal to max value, push it to the stack as another
			 * max value array. It doesn't count for max = value = 0.
			 */
            } else if (((*max_score)->value == score_matrix[i][j]) &&\
						(score_matrix[i][j] != 0)){
                push(max_score, i-1, j-1, score_matrix[i][j]);
            } else {
            }
        }
    }
    (*calc_time) += (gettime() - start_time);
    return 0;
}
