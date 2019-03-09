#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
//Kappa kappa kappa
//flags
const char* name_flag = "-name";
const char* input_flag = "-input";
const char* match_flag = "-match";
const char* mismatch_flag = "-mismatch";
const char* gap_flag = "-gap";
const int line_size = 256;
const int papis = 8;

typedef struct Node{
    int value;
    struct Node *prev; 
}Node;

typedef struct Max_Val_Pos{
    Node *cell;
    struct Max_Val_Pos *next;
}MVP;

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

int max(int a, int b, int c){
    if (a > b){
        if (a > c){
            return a;
    } else {
        if (b > c){
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
    
    *in_file = fopen(input, "r");

    if (*in_file == NULL) {   
        printf("ERROR: Could not open input file \"%s.txt\"\n", input);
        return 1;
    } 
    
    char* out_source = concat(cwd, "/Report_", name, ".txt");
    if (strcmp(out_source, "") == 0) {
        printf("ERROR: Output path invalid\n");
        return 1;
    }
    
    *out_file = fopen(out_source, "w");
    free (out_source);
    
    if (*out_file == NULL) {   
        printf("ERROR: Could not make the file \"%s/%s.txt\"\n", cwd, name);
        return 1;
    }
    
    return 0;
}

int header_parse(FILE* in_file, long int *pair_size, long int *q_size, long int *d_size){
    char* line = malloc(line_size*sizeof(char));
    
    if (fgets(line, line_size, in_file) != NULL) {
        sscanf(line,"Pairs:\t%[^\n]", line);
        if (!isdigit(*line)) {
            printf("ERROR: The dataset does not follow the format: Missing Pair number!\n");
            return 1;
        }
        *pair_size = atoi(line);
        printf("The dataset contains %ld Q-D pairs\n", *pair_size);
    } else {
        printf("ERROR: Input file was empty\n");
        return 1;
    }
    if (fgets(line, line_size, in_file) == NULL) {
        printf("ERROR: Input file terminated early\n");
        return 1;
    }
    if (fgets(line, line_size, in_file) != NULL) {
        sscanf(line,"Q_Sz_Max:\t%[^\n]", line);
        if (!isdigit(*line)) {
            printf("ERROR: The dataset does not follow the format: Missing Q_Sz_Max number!\n");
            return 1;
        }
        *q_size = atoi(line);
        printf("Q size is %ld\n", *q_size);
    } else {
        printf("ERROR: Input file terminated early\n");
        return 1;
    }
    if (fgets(line, line_size, in_file) != NULL) {
        sscanf(line,"D_Sz_All:\t%[^\n]", line);
        if (!isdigit(*line)) {
            printf("ERROR: The dataset does not follow the format: Missing D_Sz_All number!\n");
            return 1;
        }
        *d_size = atoi(line);
        printf("D size is %ld\n", *d_size);
    } else {
        printf("ERROR: Input file terminated early\n");
        return 1;
    }
    
    free(line);
    return 0;
}

int parse_file(FILE* in_file, long int pair_size, char** *q, char** *d){
    long int cnt_q = 0;
    long int cnt_d = 0;
    int flag = 0;
    
    char* line = malloc(line_size*sizeof(char));
    while (fgets(line, line_size, in_file) != NULL) {
        if (cnt_q > pair_size){
        printf("Warning: Input file contains more data than initially stated!\n\
The excess data will be ignored\n");
        break;
        }
        if (strncmp(line, "Q:", 2) == 0) {
            if (flag == 1){
                printf("ERROR: \"D\" entry no.%ld is missing\n", cnt_d+1);
                return 1;
            }
            sscanf(line, "Q:\t%[^\n]", line);
            strcpy(*q[cnt_q], line);
//             printf("[%d]Q: \"%s\"\n", cnt_q, q[cnt_q]);
            flag = 1;
            cnt_q++;
        } else if (strncmp(line, "D:", 2) == 0) {
            if (flag == 0){
                printf("ERROR: \"Q\" entry no.%ld is missing\n", cnt_q+1);
                return 1;
            }
            sscanf(line, "D:\t%[^\n]", line);
            strcpy(*d[cnt_d], line);
//             printf("[%d]D: \"%s\"\n", cnt_d, d[cnt_d]);         
            flag = 0;
            cnt_d++;
        } else if (strncmp(line, "\t", 1) == 0 || strncmp(line, "  ", 2) == 0) {
//             int tmp;
//             printf("Entered %d\n", ++tmp);
            if (flag == 1){
                sscanf(line, "\t%[^\n]", line);
                strcat(*q[cnt_q-1], line);
            } else {
                sscanf(line, "\t%[^\n]", line);
                strcat(*d[cnt_d-1], line);
            }
            
        } else {
            //printf("ERROR: Invalid Input\n");
        }
    }
    if (cnt_q!=cnt_d){
        printf("ERROR: \"D\" entry no.%ld is missing\n", cnt_d+1);
        return 1;
    }
    if (cnt_q < pair_size){
        printf("Warning: Input file contains less data than initially stated!\n\
%ld entries were stored\n", cnt_q);
        pair_size = cnt_q;
    }
    
    free(line);
    return 0;
}

// void calc_score(int match, int mismatch, int gap, MVP* *mvp, Node *sm,\
//                 long int i, long int j, char* q, char* d){
// /*    int diag = 0;
//     int up = 0;
//     int left = 0;
//     
//     if (q[i-1] == d[j-1]){
//          diag = sm[i-1][j-1]->value + match;
//     }
//     up = sm[i-1][j]->value + gap;
//     left = sm[i][j-1]->value + gap;
//     
//     int max_v = max(diag, up, left);
//         
//     sm[i][j]->value = max_v;
//     
//     if (max_v == diag){
//         sm[i][j]->prev = sm[i-1][j-1];
//     } else if (max_v == up){
//         sm[i][j]->prev = sm[i-1][j];
//     } else {
//         sm[i][j]->prev = sm[1][j-1];
//     }
//     
//     if (mvp->cell->value < max_v){
//         mvp->cell = sm[i][j];
//         mvp->next = NULL;
//     } else if (mvp->cell->value == max_v){
//         mvp->next->cell = sm[i][j];
//     } else
//   */  
// 
//     printf("Matroska %d\n", *sm[0][0].value);
// }


int create_score_matrix(int match, int mismatch, int gap, MVP *mvp,\
                        long int pair_size, char** *q, char** *d){
    long int rows;
    long int cols;
    
    for (long int p = 0; p<pair_size; p++){
        rows = strlen(*q[p]) + 1;
        cols = strlen(*d[p]) + 1;
        printf("%s\n", *q[p]);
        if (Node** score_matrix = (Node **)malloc(rows*sizeof(Node*)) == NULL){
            printf("ERROR: Not enough memory\n");
            return 1;
        }
        
        for (int i = 0; i < rows; i++){
            if (score_matrix[i] = (Node *)malloc(cols* sizeof(Node)) == NULL){
                printf("ERROR: Not enough memory\n");
                return 1;
            }
        }
        
        for (long int i = 0; i < rows; i++){
            score_matrix[i][0].value = 0;
            score_matrix[i][0].prev = NULL;
        }
        for (long int i = 0; i < cols; i++){
            score_matrix[0][i].value = 0;
            score_matrix[0][i].prev = NULL;
        }
    
//         for (long int i = 1; i < rows; i++){
//             for (long int j = 1; j<cols; j++){
//                 calc_score(match, mismatch, gap, &mvp, &score_matrix, i, j, **q[p], **d[p]);
//             }
//         }
    }
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

    if (file_open(input, name, &in_file, &out_file) == 1) return 1;
    
    long int pair_size = 0;
    long int q_size = 0;
    long int d_size = 0;
    
    if (header_parse(in_file, &pair_size, &q_size, &d_size) == 1) return 1;
    
    char** q;
    char** d;
    q = malloc(pair_size*sizeof(char*));
    d = malloc(pair_size*sizeof(char*));
    for (long int i=0; i<pair_size; i++){
        q[i] = malloc(q_size*sizeof(char));
        d[i] = malloc(d_size*sizeof(char));
    }
    
    if (parse_file(in_file, pair_size, &q, &d) == 1) return 1;

    MVP* mvp;
//     mvp->cell = NULL;
//     mvp->next = NULL;
    
    if (create_score_matrix(match, mismatch, gap, mvp, pair_size, &q, &d) == 1) return 1;
    
//     for (int i=0; i<pair_size; i++){
//         printf("Validate Integrity of Pair %d:\nQLen: %lu\nQ: %s\nDLen: %lu\n
// D: %s\n", i+1, strlen(q[i]), q[i], strlen(d[i]), d[i]);
//     }
// 
//     char* ptr;
//     char* ptr2;
//     int sz = 0;
//     int line_len = 100;
//     for (int i=0; i<pair_size; i++){
//         ptr = q[i];
//         ptr2 = d[i];
//         
//         sz = fprintf(out_file, "Q:\t%.*s\n", line_len, ptr) - 4;
//         ptr += sz;        
//         while(sz >= line_len){
//             sz = fprintf(out_file, "\t%.*s\n", line_len, ptr) - 2;
//             ptr += sz;
//         }
//         sz = fprintf(out_file, "D:\t%.*s\n", line_len, ptr2) - 4;
//         ptr2 += sz;        
//         while(sz >= line_len){
//             sz = fprintf(out_file, "\t%.*s\n", line_len, ptr2) - 2;
//             ptr2 += sz;
//         }
//     }
    
    for (long int i=0; i<pair_size; i++){
        free(q[i]);
        free(d[i]);
    }
    
    free(q);
    free(d);
    fclose(in_file);
    fclose(out_file);
    
    return 0;  
}
