#include "generic.h"
#include "pthread_pool.h"
#include "PTHREAD_functions.h"
#include "unistd.h"

int trash;

thread_data_t* make_args(int antidiag, int limSW, int limNE, char* q, char* d,\
						 int** score_matrix){
	thread_data_t* tdata = malloc(sizeof(thread_data_t));
	tdata->antidiag = antidiag;
	tdata->limSW = limSW;
	tdata->limNE = limNE;
	tdata->q = q;
	tdata->d = d;
	tdata->score_matrix = score_matrix;
	
	return tdata;
}

int calculate_score(int** score_matrix, int match, int mismatch, int gap,\
					int threads, char* q, char* d, MVP* *max_score,\
					double *calc_time){
    
	double start_time = gettime();
	int q_limit = strlen(q);
	int d_limit = strlen(d);
	int antidiag=1, mv=0, k=0;

	
	/* Initialize args to pass to queue*/
	thread_data_t* args;
	
	/* Initialize thread pool */
	struct pool *p = pool_start(inner, threads);
	
	/* 
	 * The loop where we fill the queue with tasks to be executed by our
	 * threads in the pool
	 * The pool is filled with every cell in the antidiagonal and awaits 
	 * before it proceeds to the next antidiagonal 
	 */
	for(antidiag = 1; antidiag < (q_limit+d_limit); antidiag++){
		int limSW = antidiag < q_limit ? antidiag : q_limit;
		int limNE = antidiag < d_limit ? 0 : antidiag-d_limit;
		
		args = make_args(antidiag, limSW, limNE, q, d, score_matrix);
		/* Enqueue args in the pool and free them after it's over */
		pool_enqueue(p, (void*)args, 1);
		//COND VAR HERE
	}
	pool_end(p);
	
	for (unsigned int i = 1; i <= q_limit; i++){
		for (unsigned int j = 1; j <= d_limit; j++){
			if (mv < score_matrix[i][j]){
				empty(max_score);
				mv = score_matrix[i][j];
				push(max_score, i-1, j-1, score_matrix[i][j]);
			} else if (mv == score_matrix[i][j]){
				push(max_score, i-1, j-1, score_matrix[i][j]);
			} else {
			}
		}
	}
    (*calc_time) += (gettime() - start_time);
    return 0;
}

void* inner(void* args){
	thread_data_t *t_data = args;
	
	for (int k = t_data->limSW; k > t_data->limNE; k--) {
		int i = k;
		int j = (t_data->antidiag - k) + 1;
		
		if (k > ceil(limSW-limNE/2)){
			//COND VAR HERE
		}
		
		/* 
		 * q = i-1, d = j-1, cause score_matrix zero pads q and d
		 * for initialization 
		 */
		int diagonal = t_data->q[i-1] == t_data->d[j-1] \
					? (t_data->score_matrix[i-1][j-1] + match) \
					: (t_data->score_matrix[i-1][j-1] + mismatch);
		int up = t_data->score_matrix[i-1][j] + gap;
		int left = t_data->score_matrix[i][j-1] + gap;

		/* priority in max: diagonal > up > left */
		t_data->score_matrix[i][j] = max(diagonal, up, left, &trash);

		/* if value is less than zero, replace with zero */
		if (t_data->score_matrix[i][j] < 0){
			t_data->score_matrix[i][j] = 0;
		}
	}
	
	return ((void*) 0);
}
