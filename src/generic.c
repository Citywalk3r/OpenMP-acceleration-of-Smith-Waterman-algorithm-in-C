#include "generic.h"
 
char* name_flag = "-name";
char* input_flag = "-input";
char* match_flag = "-match";
char* mismatch_flag = "-mismatch";
char* gap_flag = "-gap";
char* threads_flag = "-threads";
int line_size = 256;
 
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
