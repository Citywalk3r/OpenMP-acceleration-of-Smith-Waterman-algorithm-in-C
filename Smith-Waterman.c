/*
 * Smith-Waterman serial program
 * -----------------------------
 * Authors: C.Theodoris, M. Pantelidakis
 * Date: 03/2019 
 * 
 */
 
#include "generic.h"
#include "OMP_functions.h"

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
	int threads = 1;
	
    if (init_parsing(argc, argv, &name, &input, &match, &mismatch, &gap,\
					 &threads) == 1)
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
		calculate_score(score_matrix, match, mismatch, gap, threads, q,\
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
