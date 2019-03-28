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
	int mv = 0;
	
	int i=0, j=0, limSW=0, limNE=0, k=0, antidiag=0;
	for(antidiag = 1; antidiag < (q_limit+d_limit); antidiag++){
		limSW = antidiag < q_limit ? antidiag : q_limit;
		limNE = antidiag < d_limit ? 0 : antidiag-d_limit;
		
		for (k = limSW; k > limNE; k--) {
			i = k;
			j = (antidiag-k)+1;
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
			mv = mv >= score_matrix[i][j] ? mv : score_matrix[i][j];
		}
	}
	
	for (unsigned int i = 1; i <= q_limit; i++){
		for (unsigned int j = 1; j <= d_limit; j++){
			if (mv == score_matrix[i][j]){
				push(max_score, i-1, j-1, score_matrix[i][j]);
			}
		}
	}
    (*calc_time) += (gettime() - start_time);
    return 0;
}
