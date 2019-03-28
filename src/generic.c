#include "generic.h"
 
char* name_flag = "-name";
char* input_flag = "-input";
char* match_flag = "-match";
char* mismatch_flag = "-mismatch";
char* gap_flag = "-gap";
char* threads_flag = "-threads";
int line_size = 256;

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

int push(MVP* *max_score, unsigned int q, unsigned int d, int value){
    MVP* temp = (struct MVP*)malloc(sizeof(struct MVP));
    if (!temp){
        printf("ERROR: Overflowing Heap\n");
        return 1;
    }
    temp->q = q;
	temp->d = d;
	temp->value = value;
    temp->next = *max_score;
    *max_score = temp;
    return 0;
}

int pop(MVP* *max_score){
    if (*max_score == NULL) {
        printf("ERROR: Stack Underflow in POP\n");
        return 1;
    }
	MVP* temp = *max_score;
	(*max_score) = (*max_score)->next;
	free(temp);
	
    return 0;
}

int empty(MVP* *max_score){
    if (*max_score == NULL) {
        return 0;
    }
	while (*max_score != NULL) {
		pop(max_score);
	}
    return 0;
}

 double gettime(void) {
	struct timeval ttime;
	gettimeofday(&ttime, NULL);
	return ttime.tv_sec+ttime.tv_usec * 0.000001;
}

int max(int a, int b, int c, int* pos){
    if (a >= b){
        if (a >= c){
			*pos = 0;
            return a;
		}
    } else {
        if (b >= c){
			*pos = 1;
            return b;
		}
    }
    *pos = 2;
    return c;
}

void pretty_print(FILE* out_file, char* q, char* d, int line_len){
	int print_size;
	
	print_size = fprintf(out_file, "\t\tD:\t%.*s\n", line_len, d) - 6;
	d += print_size;        
	while(print_size >= line_len){
		print_size = fprintf(out_file, "\t\t\t%.*s\n", line_len, d) - 4;
		d += print_size;
	}
	print_size = fprintf(out_file, "\t\tQ:\t%.*s\n", line_len, q) - 6;
	q += print_size;        
	while(print_size >= line_len){
		print_size = fprintf(out_file, "\t\t\t%.*s\n", line_len, q) - 4;
		q += print_size;
	}
	fprintf(out_file, "\n");
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

int file_open(char* input, FILE* *in_file, FILE* *out_file,\
				char* out_source){
    if ((*in_file = fopen(input, "r")) == NULL) { 
        printf("ERROR: Could not open input file \"%s.txt\"\n", input);
        return 1;
    } 
    
    if (strcmp(out_source, "") == 0) {
        printf("ERROR: Output path invalid\n");
        free(out_source);
        return 1;
    }
    
    if ((*out_file = fopen(out_source, "w")) == NULL) { 
        printf("ERROR: Could not make the output file");
        free(out_source);
        return 1;
    }
    return 0;
}

int header_parse(FILE* in_file, int *pair_size, unsigned int *q_size,\
				unsigned int *d_size){
	char* line = malloc(line_size*sizeof(char));
    
    if (fgets(line, line_size, in_file)) {
        sscanf(line,"Pairs:\t%[^\n]", line);
        if (!isdigit(*line)) {
            printf("ERROR: The dataset does not follow the format: \
Missing Pair number!\n");
            free(line);
            return 1;
        }
        *pair_size = atoi(line);
        printf("\nThe dataset contains %d Q-D pairs\n", *pair_size);
    } else {
        printf("ERROR: Input file was empty\n");
        free(line);
        return 1;
    }
    if (fgets(line, line_size, in_file) == NULL) {
        printf("ERROR: Input file terminated early\n");
        free(line);
        return 1;
    }
    if (fgets(line, line_size, in_file)) {
        sscanf(line,"Q_Sz_Max:\t%[^\n]", line);
        if (!isdigit(*line)) {
            printf("ERROR: The dataset does not follow the format: Missing \
Q_Sz_Max number!\n");
            free(line);
            return 1;
        }
        *q_size = atoi(line);
    } else {
        printf("ERROR: Input file terminated early\n");
        free(line);
        return 1;
    }
    if (fgets(line, line_size, in_file)) {
        sscanf(line,"D_Sz_All:\t%[^\n]", line);
        if (!isdigit(*line)) {
            printf("ERROR: The dataset does not follow the format: Missing \
D_Sz_All number!\n");
            free(line);
            return 1;
        }
        *d_size = atoi(line);
    } else {
        printf("ERROR: Input file terminated early\n");
        free(line);
        return 1;
    }
    
    free(line);
    return 0;
}

int parse_file(FILE* in_file, FILE* out_file, char* q, char* d){
	
	int cnt = 0;
	/* 2 for init, to dodge the first empty line in the dataset */
    int flag = 2;
	
    char* line = malloc(line_size*sizeof(char));
    while (cnt < 1) {
        if (fgets(line, line_size, in_file) == NULL){
            free(line);
            return 2;
        }
        if (strncmp(line, "Q:", 2) == 0) {
            if (flag == 1){
                printf("ERROR: A \"D\" entry is missing\n");
                free(line);
                return 1;
            }
            sscanf(line, "Q:\t%[^\n]", line);
            fprintf(out_file, "Q:\t\t%s\n", line);
			strcpy(q, line);
            flag = 1;
        } else if (strncmp(line, "D:", 2) == 0) {
            if (flag == 0){
                printf("ERROR: A \"Q\" entry is missing\n");
                free(line);
                return 1;
            }
            sscanf(line, "D:\t%[^\n]", line);
			fprintf(out_file, "D:\t\t%s\n", line);
            strcpy(d, line);
            flag = 0;
        } else if (strncmp(line, "\t", 1) == 0) {
			fprintf(out_file, "\t%s", line);
			sscanf(line, "\t%[^\n]", line);
            if (flag == 1){
                strcat(q, line);
            } else {
                strcat(d, line);
            }
        } else {
            if (flag == 0){
				cnt++;
			}
        }
    }
    free(line);
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
