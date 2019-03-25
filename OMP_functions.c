#include "generic.h"
#include "OMP_functions.h"

char* concat(const char* s1, const char* s2, const char* s3, const char* s4,\
			const char* s5, const char* s6) {
    char* result = malloc(strlen(s1) + strlen(s2) + strlen(s3) + \
					strlen(s4) + strlen(s5) + strlen(s6) + 1);
    if (result) {
        strcpy(result, s1);
        strcat(result, s2);
        strcat(result, s3);
        strcat(result, s4);
		strcat(result, s5);
        strcat(result, s6);
        return result;
    } else {
        printf("ERROR: Concatenation Error\n");
        free(result);
        return "";
    }
}

int init_parsing(int count, char* *vector, char* *name, char* *input,\
				int *match, int *mismatch, int *gap, int *threads){
    for(int i=1; i<count; i+=2) {
        if (strcmp(name_flag, vector[i]) == 0) {
            *name = vector[i+1];
        }else if(strcmp(input_flag, vector[i]) == 0) {
            *input = vector[i+1];
        }else if(strcmp(match_flag, vector[i]) == 0) {
            if (sscanf(vector[i+1], "%d", match) != 1){
                printf("ERROR: \"match\" NaN\n");
                return 1;
            }
        }else if(strcmp(mismatch_flag, vector[i]) == 0) {
            if (sscanf(vector[i+1], "%d", mismatch) != 1){
                printf("ERROR: \"mismatch\" NaN\n");
                return 1;
            }
        }else if(strcmp(gap_flag, vector[i]) == 0) {
            if (sscanf(vector[i+1], "%d", gap) != 1){
                printf("ERROR: \"gap\" NaN\n");
                return 1;
            }
        }else if(strcmp(threads_flag, vector[i]) == 0) {
            if (sscanf(vector[i+1], "%d", threads) != 1){
                printf("ERROR: \"threads\" NaN\n");
                return 1;
            }
		} else {
            printf("ERROR: Flag \"%s\" is not recognised\n", vector[i]);
            return 1;
        }
    }
    return 0;
}

int calculate_score(int** score_matrix, int match, int mismatch, int gap,\
					int threads, char* q, char* d, MVP* *max_score,\
					double *calc_time){
    
	double start_time = gettime();
    int diagonal, up, left;
	int trash = 0;
	int q_limit = strlen(q);
	int d_limit = strlen(d);
	
	int nthreads=0, i=0, j=0, limSW=0, limNE=0, k=0, antidiag=0;
	nthreads = threads;
	
	push(max_score, 0, 0, 0);
	
	/*
	 * This is the parallel section
	 * Firstprivate values are initialized private values to each thread
	 * Private values are uninitialized private values to each thread
	 * Shared values are public initialized variables that all threads use at 
	 * the same time
	 * Num_threads specifies the maximum number of threads we will use 
	 */
	#pragma omp parallel firstprivate(q_limit,d_limit) \
						 private(antidiag,trash) \
						 shared(q,d,score_matrix,max_score) \
						 num_threads(nthreads)
	{
// 		omp_set_dynamic(0);
		for(antidiag = 1; antidiag < (q_limit+d_limit); antidiag++){
			limSW = antidiag < q_limit ? antidiag : q_limit;
			limNE = antidiag < d_limit ? 0 : antidiag-d_limit;
			#pragma omp for private(k, i, j, diagonal, up, left) schedule(sched)
			for (k = limSW; k > limNE; k--) {
				i = k;
				j = (antidiag-k)+1;
				/* 
				* q = i-1, d = j-1, cause score_matrix zero pads q and d
				* for initialization 
				*/
				if (q[i-1] == d[j-1])
					diagonal = score_matrix[i-1][j-1] + match;
				else 
					diagonal = score_matrix[i-1][j-1] + mismatch;
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
				#pragma omp critical(search_max)
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
	}
    (*calc_time) += (gettime() - start_time);
    return 0;
}

int traceback(int** score_matrix, MVP* *max_score, FILE* out_file, char* q,\
				char* d, unsigned int *steps, double *tb_time){
	double start_time = gettime();
	unsigned int startq, startd, stopd;
	int i, j, m, pos, max_val;
	char q_temp, d_temp;
	int count = 1;
	int q_jump = 0;
	int d_jump = 0;
	
	char* q_print = (char *)malloc(sizeof(char));
	char* d_print = (char *)malloc(sizeof(char));
	
	if(*max_score != NULL){
		max_val = (*max_score)->value;
	}
	while(*max_score != NULL){
		startq = ((*max_score)->q);
		startd = (*max_score)->d;
		stopd = startd;
		
		q_print = (char *)realloc(q_print, sizeof(char));
		d_print = (char *)realloc(d_print, sizeof(char));
		strcpy(q_print, "\0");
		strcpy(d_print, "\0");
		do{
			q_jump = strlen(q_print)+1;
			q_print = (char *)realloc(q_print, sizeof(char)*(q_jump+1));
			if (q_print == NULL){
				printf("ERROR: Reallocating q failed\n");
				return 1;
			}
			
			d_jump = strlen(d_print)+1;
			d_print = (char *)realloc(d_print, sizeof(char)*(d_jump+1));
			if (d_print == NULL){
				printf("ERROR: Reallocating d failed\n");
				return 1;
			}
			
			/* 
			 * i = q+1, j = d+1 cause score_matrix zero pads q and d for 
			 * initialization. Call-by-reference to auto update within
			 * the loop.
			 */
			i = (*(&startq)) + 1;
			j = (*(&startd)) + 1;
			/*
			 * max -> diagonal, up, left
			 * pos ->    0    ,  1,   2
			 */
			m = max(score_matrix[i-1][j-1],\
					score_matrix[i-1][j],\
					score_matrix[i][j-1], &pos);
			
			/* priority: diagonal > up > left */
			if (pos == 0){
				q_temp = q[startq];
				d_temp = d[startd];
				startq -= 1;
				startd -= 1;
			} else if (pos == 1){
				q_temp = q[startq];
				d_temp = '-';
				startq -= 1;
			} else {
				q_temp = '-';
				d_temp = d[startd];
				startd -= 1;
			}
			if (memmove(q_print+1, q_print, q_jump) == NULL){
				free(q_print);
				free(d_print);
				printf("ERROR: memmove on q failed\n");
				return 1;
			}
			q_print[0] = q_temp;
			
			if (memmove(d_print+1, d_print, d_jump) == NULL){
				free(q_print);
				free(d_print);
				printf("ERROR: memmove on d failed\n");
				return 1;
			}
			d_print[0] = d_temp;
			
			(*steps)++;
			if (m == 0) break;
		} while (1);
		
		pop(max_score);
		fprintf(out_file, "Match %d [Score: %d, Start: %d, Stop: %d]\n",\
				count, max_val, startd, stopd);
		pretty_print(out_file, q_print, d_print, 100);
		
		count++;
	}
	free(q_print);
	free(d_print);
	(*tb_time) += (gettime() - start_time);
	return 0;
}
