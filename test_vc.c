#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>

//flags
const char* name_flag = "-name";
const char* input_flag = "-input";
const char* match_flag = "-match";
const char* mismatch_flag = "-mismatch";
const char* gap_flag = "-gap";
const int line_size = 256;

typedef struct MVP{
    unsigned int q;
	unsigned int d;
    struct MVP *next;
}MVP;

int push(MVP* *max_score, unsigned int q, unsigned int d){
    MVP* temp = (struct MVP*)malloc(sizeof(struct MVP));
    if (!temp){
        printf("ERROR: Overflowing Heap\n");
        return 1;
    }
    temp->q = q;
	temp->d = d;
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
//         printf("ERROR: Stack Underflow in EMPTY\n");
        return 0;
    }
	while (*max_score != NULL) {
		pop(max_score);
	}
    return 0;
}

char* concat(const char* s1, const char* s2, const char* s3, const char* s4) {
    char* result = malloc(strlen(s1) + strlen(s2) + strlen(s3) + strlen(s4) + 1);
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

int max(int a, int b, int c, int* pos){
    if (a > b){
        if (a > c){
			*pos = 0;
            return a;
		}
    } else {
        if (b > c){
			*pos = 1;
            return b;
		}
    }
    *pos = 2;
    return c;
}

int init_parsing(int count, char* *vector, char* *name, char* *input, int *match,\
                int *mismatch, int *gap){
    
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
    printf("Program: %s\nName: %s\nInput: %s\nMatch: %d\nMismatch: %d\n\
Gap: %d\n", vector[0], *name, *input, *match, *mismatch, *gap);
    return 0;
}

int file_open(char* input, char* name, FILE* *in_file, FILE* *out_file){
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("ERROR: getcwd() failed\n");
        return 1;
    }
    
    if ((*in_file = fopen(input, "r")) == NULL) { 
        printf("ERROR: Could not open input file \"%s.txt\"\n", input);
        return 1;
    } 
    
    char* out_source = concat(cwd, "/Report_", name, ".txt");
    if (strcmp(out_source, "") == 0) {
        printf("ERROR: Output path invalid\n");
        free(out_source);
        return 1;
    }
    
    if ((*out_file = fopen(out_source, "w")) == NULL) { 
        printf("ERROR: Could not make the file \"%s/%s.txt\"\n", cwd, name);
        free(out_source);
        return 1;
    }
    free(out_source);
    return 0;
}

int header_parse(FILE* in_file, int *pair_size, unsigned int *q_size,\
				unsigned int *d_size){
    char* line = malloc(line_size*sizeof(char));
    
    if (fgets(line, line_size, in_file)) {
        sscanf(line,"Pairs:\t%[^\n]", line);
        if (!isdigit(*line)) {
            printf("ERROR: The dataset does not follow the format: Missing Pair number!\n");
            free(line);
            return 1;
        }
        *pair_size = atoi(line);
        printf("The dataset contains %d Q-D pairs\n", *pair_size);
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
            printf("ERROR: The dataset does not follow the format: Missing Q_Sz_Max number!\n");
            free(line);
            return 1;
        }
        *q_size = atoi(line);
        printf("Q size is %d\n", *q_size);
    } else {
        printf("ERROR: Input file terminated early\n");
        free(line);
        return 1;
    }
    if (fgets(line, line_size, in_file)) {
        sscanf(line,"D_Sz_All:\t%[^\n]", line);
        if (!isdigit(*line)) {
            printf("ERROR: The dataset does not follow the format: Missing D_Sz_All number!\n");
            free(line);
            return 1;
        }
        *d_size = atoi(line);
        printf("D size is %d\n", *d_size);
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
    int flag = 0;
    
    char* line = malloc(line_size*sizeof(char));
    while (cnt < 2) {
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
            cnt++;
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
        } else if (strncmp(line, "\t", 1) == 0 || strncmp(line, "  ", 2) == 0) {
			fprintf(out_file, "\t%s", line);
			sscanf(line, "\t%[^\n]", line);
            if (flag == 1){
                strcat(q, line);
            } else {
                strcat(d, line);
            }
            
        } else {
            //printf("ERROR: Invalid Input\n");
        }
    }
    free(line);
    return 0;
}

int create_score_matrix(unsigned int rows, unsigned int columns,\
						int (*score_matrix)[columns]){
    for (unsigned int i = 0; i < rows; i++){
        for (unsigned int j = 0; j < columns; j++){
			score_matrix[i][j] = 0;
		}
    }
    return 0;
}

int calculate_score(unsigned int rows, unsigned int columns, int (*score_matrix)[columns],\
					int match, int mismatch,\
                    int gap, char* q, char* d, MVP* *max_score){
    
    int diagonal, up, left;
	int trash = 0;
    push(max_score, 0, 0);
    
    for (unsigned int i = 1; i <= strlen(q); i++){
        for (unsigned int j = 1; j <= strlen(d); j++){
			//q = i-1, d = j-1 cause score_matrix zero pads q and d for initialization
            if (q[i-1] == d[j-1])
                diagonal = score_matrix[i-1][j-1] + match;
            else 
                diagonal = score_matrix[i-1][j-1] + mismatch;
            up = score_matrix[i-1][j] + gap;
            left = score_matrix[i][j-1] + gap;
			
			score_matrix[i][j] = max(diagonal, up, left, &trash);
			if (score_matrix[i][j] < 0){
				score_matrix[i][j] = 0;
			}
			//q = i-1, d = j-1 cause score_matrix zero pads q and d for initialization
            if (score_matrix[((*max_score)->q)+1][((*max_score)->d)+1] < score_matrix[i][j]){
                empty(max_score);
                push(max_score, i-1, j-1);
            } else if ((score_matrix[((*max_score)->q)+1][((*max_score)->d)+1] == \
						score_matrix[i][j]) && (score_matrix[i][j] != 0)){
                push(max_score, i-1, j-1);
            } else {
            }
        }
    }
    return 0;
}

int traceback(unsigned int rows, unsigned int columns, int (*score_matrix)[columns],\
				MVP* *max_score, FILE* out_file, char* q, char* d){
	int count = 1;
	unsigned int startq, startd, stopq, stopd;
	int max_val;
	int pos;
	if(*max_score != NULL){
		max_val = score_matrix[((*max_score)->q)+1][(*max_score)->d+1];
	}
	while(*max_score != NULL){
		startq = ((*max_score)->q);
		startd = (*max_score)->d;
		stopq = startq;
		stopd = startd;
		//i = q+1, j = d+1 cause score_matrix zero pads q and d for initialization
		while (score_matrix[startq+1][startd+1] > 0){
			//max -> {diagonal,0}, {up,1}, {left,2}
			//check q,d: [-1][-1], [-1][0], [0][-1] for traceback
			//so we search score_matrix: [0][0], [0][+1], [+1][0]
			max(score_matrix[startq][startd], score_matrix[startq][startd+1],\
				score_matrix[startq+1][startd], &pos);
			if (pos == 0){
				if (score_matrix[startq][startd] == 0) break;
				startq -= 1;
				startd -= 1;
			} else if (pos == 1){
				if (score_matrix[startq][startd+1] == 0) break;
				startq -= 1;
			} else {
				if (score_matrix[startq+1][startd] == 0) break;
				startd -= 1;
			}
		}
		pop(max_score);
		
		fprintf(out_file, "Match %d [Score: %d, Start: %d, Stop: %d]\n",\
				count, max_val, startd, stopd);
		fprintf(out_file, "\t\tD:%.*s\n", (int)(stopd-startd+1), &d[startd]);
		fprintf(out_file, "\t\tQ:%.*s\n", (int)(stopq-startq+1), &q[startq]);
		fprintf(out_file, "\n");
		count++;
	}
	return 0;
}

int main(int argc, char* argv[]) {
    
    //variables
    char* name = "";
    char* input = "";
    int match = -1;
    int mismatch = -1;
    int gap = -1;
    
    if (init_parsing(argc, argv, &name, &input, &match, &mismatch, &gap) == 1) return 1;
    
    FILE* in_file;
    FILE* out_file;

    if (file_open(input, name, &in_file, &out_file) == 1){
        fclose(in_file);
        fclose(out_file);
        return 1;
    }
    
    int pair_size = 0;
    unsigned int q_size = 0;
    unsigned int d_size = 0;
    
    if (header_parse(in_file, &pair_size, &q_size, &d_size) == 1){
        fclose(in_file);
		fclose(out_file);
        return 1;
    }
    
    int check = 0;
    int pairs = 1;
	unsigned int rows, columns;
	MVP* max_score = NULL;
	int (*score_matrix)[] = NULL;
	
    while (pairs <= pair_size){
		printf("Pair %d\n", pairs);
        char* q = malloc(sizeof(char[q_size+1]));
		char* d = malloc(sizeof(char[d_size+1]));
		
        check = parse_file(in_file, out_file, q, d);
        
		fprintf(out_file, "\n");
		
        if (check == 1){
            free(q);
            free(d);
			if (max_score){
				empty(&max_score);
			}
			if (score_matrix){
				free(score_matrix);
			}
			fclose(in_file);
			fclose(out_file);
            return 1;
        }
        
        rows = strlen(q) + 1;
		columns = strlen(d) + 1;
		
		int (*score_matrix)[columns] = malloc(sizeof(int[rows][columns]));
        create_score_matrix(rows, columns, score_matrix);
		calculate_score(rows, columns, score_matrix, match, mismatch, gap, q, d, &max_score);
        traceback(rows, columns, score_matrix, &max_score, out_file, q, d);
		
		free(q);
		free(d);
		free(score_matrix);
        empty(&max_score);
		if (check == 2) break;
        pairs++;
    } 
    fclose(in_file);
    fclose(out_file);
    
    return 0;
}
