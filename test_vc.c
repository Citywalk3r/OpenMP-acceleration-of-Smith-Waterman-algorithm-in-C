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

typedef struct Node{
    long int value;
    struct Node *prev; 
}Node;

typedef struct MVP{
    Node *cell;
    struct MVP *next;
}MVP;

MVP* max_score;

int push(Node* cell){
    MVP* temp;
    temp = (struct MVP*)malloc(sizeof(struct MVP));
    if (!temp){
        printf("ERROR: Overflowing Heap\n");
        return 1;
    }
    temp->cell = cell;
    temp->next = max_score;
    max_score = temp;
    return 0;
}

int pop(){
    MVP* temp;
    if (max_score == NULL) {
        printf("ERROR: Stack Underflow\n");
        return 1;
    } else {
        temp = max_score;
        max_score = max_score->next;
        temp->next = NULL;
        free(temp);
    }
    return 0;
}

int empty(){
    if (max_score == NULL) {
        printf("ERROR: Stack Underflow\n");
        return 1;
    } else {
        while (max_score != NULL) {
            pop();
        }
        
    }
    return 0;
}

char* concat(const char* s1, const char* s2,  const char* s3,  const char* s4) {
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

int max(long int a, long int b, long int c){
    if (a > b){
        if (a > c)
            return a;
    } else {
        if (b > c)
            return b;
    }
    return c;
}

int init_parsing(int count, char* *vector, char* *name, char* *input, long int *match,\
                long int *mismatch, long int *gap){
    
    for(int i=1; i<count; i+=2) {
        if (strcmp(name_flag, vector[i]) == 0) {
            *name = vector[i+1];
        }else if(strcmp(input_flag, vector[i]) == 0) {
            *input = vector[i+1];
        }else if(strcmp(match_flag, vector[i]) == 0) {
            if (sscanf(vector[i+1], "%ld", match) != 1){
                printf("ERROR: \"match\" NaN\n");
                return 1;
            }
        }else if(strcmp(mismatch_flag, vector[i]) == 0) {
            if (sscanf(vector[i+1], "%ld", mismatch) != 1){
                printf("ERROR: \"mismatch\" NaN\n");
                return 1;
            }
        }else if(strcmp(gap_flag, vector[i]) == 0) {
            if (sscanf(vector[i+1], "%ld", gap) != 1){
                printf("ERROR: \"gap\" NaN\n");
                return 1;
            }
        } else {
            printf("ERROR: Flag \"%s\" is not recognised\n", vector[i]);
            return 1;
        }
    }
    printf("Program: %s\nName: %s\nInput: %s\nMatch: %ld\nMismatch: %ld\n\
Gap: %ld\n", vector[0], *name, *input, *match, *mismatch, *gap);
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

int header_parse(FILE* in_file, long int *pair_size, long int *q_size, long int *d_size){
    char* line = malloc(line_size*sizeof(char));
    
    if (fgets(line, line_size, in_file) != NULL) {
        sscanf(line,"Pairs:\t%[^\n]", line);
        if (!isdigit(*line)) {
            printf("ERROR: The dataset does not follow the format: Missing Pair number!\n");
            free(line);
            return 1;
        }
        *pair_size = atoi(line);
        printf("The dataset contains %ld Q-D pairs\n", *pair_size);
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
    if (fgets(line, line_size, in_file) != NULL) {
        sscanf(line,"Q_Sz_Max:\t%[^\n]", line);
        if (!isdigit(*line)) {
            printf("ERROR: The dataset does not follow the format: Missing Q_Sz_Max number!\n");
            free(line);
            return 1;
        }
        *q_size = atoi(line);
        printf("Q size is %ld\n", *q_size);
    } else {
        printf("ERROR: Input file terminated early\n");
        free(line);
        return 1;
    }
    if (fgets(line, line_size, in_file) != NULL) {
        sscanf(line,"D_Sz_All:\t%[^\n]", line);
        if (!isdigit(*line)) {
            printf("ERROR: The dataset does not follow the format: Missing D_Sz_All number!\n");
            free(line);
            return 1;
        }
        *d_size = atoi(line);
        printf("D size is %ld\n", *d_size);
    } else {
        printf("ERROR: Input file terminated early\n");
        free(line);
        return 1;
    }
    
    free(line);
    return 0;
}

int parse_file(FILE* in_file, char* q, char* d){
    long int cnt = 0;
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
            strcpy(d, line);       
            flag = 0;
        } else if (strncmp(line, "\t", 1) == 0 || strncmp(line, "  ", 2) == 0) {
            if (flag == 1){
                sscanf(line, "\t%[^\n]", line);
                strcat(q, line);
            } else {
                sscanf(line, "\t%[^\n]", line);
                strcat(d, line);
            }
            
        } else {
            //printf("ERROR: Invalid Input\n");
        }
    }
    free(line);
    return 0;
}

int calculate_score(long int c, Node (*score_matrix)[c], long int match, long int mismatch,\
                    long int gap, char* q, char* d){
    
    long int diagonal, up, left;
    push(&score_matrix[0][0]);
    
    for (int i = 1; i < strlen(q); i++){
        for (int j = 1; j < strlen(d); j++){
            if (q[i-1] == d[j-1])
                diagonal = score_matrix[i-1][j-1].value + match;
            else 
                diagonal = score_matrix[i-1][j-1].value + mismatch;
            up = score_matrix[i-1][j].value + gap;
            left = score_matrix[i][j-1].value + gap;
            
            score_matrix[i][j].value = max(diagonal, up, left);
            if (max_score->cell->value < score_matrix[i][j].value){
                empty();
                push(&score_matrix[i][j]);
            } else if (max_score->cell->value == score_matrix[i][j].value){
                push(&score_matrix[i][j]);
            } else {
                
            }
        }
    }
    return 0;
}

int create_score_matrix(long int match, long int mismatch, long int gap, char* q, char* d){
    long unsigned int rows = strlen(q) + 1;
    long unsigned int columns = strlen(d) + 1;
    Node (*score_matrix)[columns] = malloc(sizeof(Node[rows][columns]));
    
    for (int i = 0; i < rows; i++){
        score_matrix[i][0].value = 0;
        score_matrix[i][0].prev = NULL;
    }
    for (int i = 0; i < columns; i++){
        score_matrix[0][i].value = 0;
        score_matrix[0][i].prev = NULL;
    }
    
    if (calculate_score(columns, score_matrix, match, mismatch, gap, q, d) == 1){
        free(score_matrix);
        return 1;
    }
    printf("Max score: %ld\n", max_score->cell->value);
    free(score_matrix);
    return 0;
}

int main(int argc, char* argv[]) {
    
    //variables
    char* name = "";
    char* input = "";
    long int match = -1;
    long int mismatch = -1;
    long int gap = -1;
    
    if (init_parsing(argc, argv, &name, &input, &match, &mismatch, &gap) == 1) return 1;
    
    FILE* in_file;
    FILE* out_file;

    if (file_open(input, name, &in_file, &out_file) == 1){
        fclose(in_file);
        fclose(out_file);
        return 1;
    } 
    
    long int pair_size = 0;
    long int q_size = 0;
    long int d_size = 0;
    
    if (header_parse(in_file, &pair_size, &q_size, &d_size) == 1){
        fclose(in_file);
        return 1;
    }
    int check = 0;
    int pairs = 0;
    while (pairs < pair_size){
        char *q = malloc(sizeof(char[q_size+1]));
        char *d = malloc(sizeof(char[d_size+1]));
        
        check = parse_file(in_file, q, d);
        
        if (check == 1){
            free(q);
            free(d);
            free(max_score);
            return 1;
        } 
        
        if (create_score_matrix(match, mismatch, gap, q, d) == 1){
            free(q);
            free(d);
            free(max_score);
            return 1;
        }
        
        if (check == 2){
            free(q);
            free(d);
            free(max_score);
            break;
        }
        empty();
        free(q);
        free(d);
        free(max_score);
        pairs++;
    } 
    
    fclose(in_file);
    fclose(out_file);
    
    return 0;  
}
