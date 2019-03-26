#include "generic.h"
#include "SERIAL_functions.h"

char* concat(const char* s1, const char* s2, const char* s3, const char* s4) {
    char* result = malloc(strlen(s1) + strlen(s2) + strlen(s3) + \
					strlen(s4) + 1);
    if (result) {
        strcpy(result, s1);
        strcat(result, s2);
        strcat(result, s3);
        strcat(result, s4);
        return result;
    } else {
        printf("ERROR: Concatenation Error\n");
        free(result);
        return "";
    }
}

int init_parsing(int count, char* *vector, char* *name, char* *input,\
				int *match, int *mismatch, int *gap){
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
        } else {
            printf("ERROR: Flag \"%s\" is not recognised\n", vector[i]);
            return 1;
        }
    }
    return 0;
}

int calculate_score(int** score_matrix, int match, int mismatch, int gap,\
					char* q, char* d, MVP* *max_score, double *calc_time){
    
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
