 /*
 * Smith-Waterman serial program
 * -----------------------------
 * Authors: C.Theodoris, M. Pantelidakis
 * Date: 03/2019 
 * 
 */
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
#include <sys/time.h>

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

/*
 * Function: push
 * --------------
 * 	A simple push function for the max-value stack 
 * 
 * 	*max_score: pointer to head of stack
 * 	q: the position on the q string
 * 	d: the position on the d string
 * 
 * 	returns: 0 ok, 1 error
 */
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

/*
 * Function: pop
 * -------------
 * 	A simple pop function for the max-value stack
 * 
 * 	*max_score: pointer to head of stack
 * 
 * 	returns: 0 ok, 1 error
 */
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

/*
 * Function: empty
 * ---------------
 * 	Empties the stack utilizing pop()
 * 
 * 	*max_score: pointer to head of stack
 * 
 * 	returns: 0 ok
 */
int empty(MVP* *max_score){
    if (*max_score == NULL) {
        return 0;
    }
	while (*max_score != NULL) {
		pop(max_score);
	}
    return 0;
}

/*
 * Function: gettime
 * -----------------
 * 	A simple timestamp function, as given in the project description
 * 
 * 	returns: the exact time when it is called
 */
 double gettime(void) {
	struct timeval ttime;
	gettimeofday(&ttime, NULL);
	return ttime.tv_sec+ttime.tv_usec * 0.000001;
}

/*
 * Function: concat
 * ----------------
 * 	Concatenation function, used to create the output file path
 * 
 * 	s1-4: strings to concat
 * 
 * 	returns: concatenated string, or empty string
 */
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

/*
 * Function: max
 * -------------
 * 	Max function
 * 
 * 	a,b,c: values
 * 	pos: position of the max element (0-2)
 * 
 * 	returns: max element
 */
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

/*
 * Function: init_parsing
 * ----------------------
 * 	This function is scanning the args of the program when it's called.
 * 	Implements a simple error handling for wrong/unknown inputs and formats
 * 
 * 	count: argc
 * 	*vector: argv
 *  *name: pointer to fill output name (call-by-reference)
 * 	*input: pointer to fill input file path (call-by-reference)
 * 	*match: pointer to fill match (call-by-reference)
 * 	*mismatch: pointer to fill mismatch (call-by-reference)
 *  *gap: pointer to fill gap (call-by-reference)
 * 
 * 	returns: 0 ok, 1 error
 */
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

/*
 * Function: file_open
 * -------------------
 * 	This function opens the input and output files, using the previous args.
 * 	Saves the output file on the current working directory.
 * 	Implements a simple error handling.
 * 
 * 	input: input file path
 * 	*in_file: pointer to fill input file (call-by-reference)
 * 	*out_file: pointer to fill output file (call-by-reference)
 * 	out_source: output file path
 * 
 * 	returns: 0 ok, 1 error
 */
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

/*
 * Function: header_parse
 * ----------------------
 * 	This function parses the initial data on the head of the input file.
 * 	It produces the data used on the initialization of our data structures.
 * 	It doesn't take into consideration Q_Sz_Min, as we evaluate the size of
 * 	each pair on runtime. Yet again, it implements a simple error handling
 * 
 * 	in_file: input file
 * 	*pair_size: pointer to fill pair size (call-by-reference)
 * 	*q_size: pointer to fill q size (call-by-reference)
 * 	*d_size: pointer to fill d size (call-by-reference)
 * 
 * 	returns: 0 ok, 1 error
 */
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

/*
 * Function: header_parse
 * ----------------------
 * 	This function parses the input file until it finds a whole Q-D pair.
 * 	It uses the dataset's property of having an empty line after each pair.
 * 	To be sure that a whole pair is read, we have a flag that shows if we read
 * 	a Q or a D value. Along with the error handling, it is used to determine
 * 	when we can stop parsing (empty line after D is read)
 * 
 * 	in_file: input file
 * 	out_file: output file
 * 	q: q string
 * 	d: d string
 * 
 * 	returns: 0 ok, 1 error, 2 parsing is finished
 */
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

/*
 * Function: calculate_score
 * -------------------------
 * 	This function gets an initialized, zero-filled score matrix and calculates 
 * 	each cell's value according to the Smith Waterman algorithm. It also keeps
 * 	track of the max value and the coordinates of the cells containing it,
 * 	using the MVP stack 
 * 
 * 	score_matrix: 2d array for q,d alignment
 * 	match, mismatch, gap: parameters of the algorithm
 * 	q,d: q and d strings
 * 	*max_score: pointer to fill the max_score stack (call-by-reference)
 * 	*calc_time: pointer to count runtime (call-by-reference)
 * 
 * 	returns: 0 ok
 */
int calculate_score(int** score_matrix, int match, int mismatch, int gap,\
					char* q, char* d, MVP* *max_score, double *calc_time){
    
	double start_time = gettime();
    int diagonal, up, left;
	int trash = 0;
	int q_limit = strlen(q);
	int d_limit = strlen(d);
	int prev = 0;
    push(max_score, 0, 0);
	
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
			prev = score_matrix[((*max_score)->q)+1][((*max_score)->d)+1]; 
            if (prev < score_matrix[i][j]){
                empty(max_score);
                push(max_score, i-1, j-1);
			/* 
			 * if value is equal to max value, push it to the stack as another
			 * max value array. It doesn't count for max = value = 0.
			 */
            } else if ((prev == score_matrix[i][j]) &&\
						(score_matrix[i][j] != 0)){
                push(max_score, i-1, j-1);
            } else {
            }
        }
    }
    (*calc_time) += (gettime() - start_time);
    return 0;
}

/*
 * Function: pretty_print
 * -------------------
 * 	This function takes the results of traceback and word wraps them
 * 	to line_len size lines while printing them to output file
 * 
 * 	out_file: the output file to print the results
 * 	q,d: back traced q and d strings
 */
void pretty_print(FILE* out_file, char* q, char* d, int line_len){
	int print_size;
	
	print_size = fprintf(out_file, "\tQ:\t%.*s\n", line_len, q) - 5;
	q += print_size;        
	while(print_size >= line_len){
		print_size = fprintf(out_file, "\t\t%.*s\n", line_len, q) - 3;
		q += print_size;
	}
	print_size = fprintf(out_file, "\tD:\t%.*s\n", line_len, d) - 5;
	d += print_size;        
	while(print_size >= line_len){
		print_size = fprintf(out_file, "\t\t%.*s\n", line_len, d) - 3;
		d += print_size;
	}
	fprintf(out_file, "\n");
}


/*
 * Function: traceback
 * -------------------
 * 	This function takes the filled score matrix and the MVP stack and traces 
 * 	back all the max values to their root. It then prints out to the out-file
 * 
 * 	score_matrix: 2d array for q,d alignment
 * 	*max_score: pointer to use the max_score stack
 * 	out_file: the output file to print the results
 * 	q,d: q and d strings
 * 	*steps: pointer to count number of traceback steps (call-by-reference)
 * 	*tb_time: pointer to count runtime (call-by-reference)
 * 
 * 	returns: 0 ok, 1 error
 */
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
		max_val = score_matrix[((*max_score)->q)+1][(*max_score)->d+1];
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

/*
 * Function: main
 * --------------
 * 	The main function where everything is initialized and called from
 * 
 * 	argc: count of arguments given
 * 	argv: vector of arguments
 * 
 * 	returns: 0 ok, 1 error
 */
int main(int argc, char* argv[]) {

	double start_time = gettime();
    char* name = "";
    char* input = "";
    int match = -1;
    int mismatch = -1;
    int gap = -1;
	
    if (init_parsing(argc, argv, &name, &input, &match, &mismatch, &gap) == 1)
		return 1;
    
	FILE* in_file;
    FILE* out_file;
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("ERROR: getcwd() failed\n");
        return 1;
    }
	char* out_source = concat(cwd, "/Report_", name, ".txt");
    
	if (file_open(input, &in_file, &out_file, out_source) == 1){
        fclose(in_file);
		free(out_source);	
        fclose(out_file);
        return 1;
    }
    
    int pair_size = 0;
    unsigned int q_size = 0;
    unsigned int d_size = 0;
	
    if (header_parse(in_file, &pair_size, &q_size, &d_size) == 1){
        fclose(in_file);
		free(out_source);
		fclose(out_file);
        return 1;
    }
    
    int check = 0;
    int pairs = 1;
	MVP* max_score = NULL;
	unsigned int cell_count = 0;
	unsigned int tb_steps = 0;
	double calc_time = 0.0;
	double tb_time = 0.0;
	unsigned int rows = q_size+1;
	unsigned int columns = d_size+1;
	int** score_matrix = calloc (rows, sizeof(*score_matrix));
	for (int i = 0; i < rows; i++)
		score_matrix[i] = calloc (columns, sizeof(*score_matrix[i]));
	char* q = malloc(sizeof(char[q_size+1]));
	char* d = malloc(sizeof(char[d_size+1]));
	
	while (pairs <= pair_size){
		
        check = parse_file(in_file, out_file, q, d);
		fprintf(out_file, "\n");
		
		/* 
		 * check to ensure that we have not encountered an error in the dataset
		 */
        if (check == 1){
			for (int i = 0; i < q_size+1; i++)
				free(score_matrix[i]);
			free(score_matrix);
			fclose(in_file);
			fclose(out_file);
            free(q);
            free(d);
			free(out_source);
			empty(&max_score);
            return 1;
        }
        
        /*
		 * specify the space we are using from the allocated memory. At first we 
		 * allocated the maximum space needed, so we need to shape it according
		 * to the specific pair
		 */
        rows = strlen(q) + 1;
		columns = strlen(d) + 1;
		
		cell_count += rows*columns;
		calculate_score(score_matrix, match, mismatch, gap, q,\
						d, &max_score, &calc_time);
        if(traceback(score_matrix, &max_score, out_file, q, d, &tb_steps,\
						&tb_time) == 1){
			for (int i = 0; i < q_size+1; i++)
				free(score_matrix[i]);
			free(score_matrix);
			fclose(in_file);
			fclose(out_file);
			free(q);
			free(d);
			free(out_source);
			empty(&max_score);
			return 1;
		}
		
		/*
		 * emptying the max-values-list, checking if we finished the dataset 
		 * and/or moving to the next pair
		 */
        empty(&max_score);
		if (check == 2) break;
        pairs++;
    }
	
	double end_time = gettime()-start_time;
	printf("Total cells utilized: %u\n", cell_count);
	printf("Total traceback steps: %u\n", tb_steps);
	printf("Elapsed time: %.3f s\n", end_time);
	printf("Total matrix calculation time: %.3f s\n", calc_time);
	printf("Total Traceback time: %.3f s\n", tb_time);
	printf("Cell Updates Per Second of total runtime: %.3f\n",\
			((strlen(q))*(strlen(d)))/end_time);
	printf("Cell Updates Per Second of matrix calculation runtime: %.3f\n",\
			((strlen(q))*(strlen(d)))/calc_time);
	for (int i = 0; i < q_size+1; i++)
		free(score_matrix[i]);
	free(score_matrix);
    fclose(in_file);
    fclose(out_file);
	free(q);
	free(d);
	free(out_source);
    
	return 0;
}
