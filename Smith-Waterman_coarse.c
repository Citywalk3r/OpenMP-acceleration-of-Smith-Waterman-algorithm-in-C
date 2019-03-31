/*
 * Smith-Waterman serial program
 * -----------------------------
 * Authors: C.Theodoris, M. Pantelidakis
 * Date: 03/2019 
 * 
 */
 
#include "generic.h"
#include "SERIAL_functions.h"
#include <omp.h>

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
    
	static int check = 0;
    int pair;
	static MVP* max_score = NULL;
	unsigned int cell_count = 0;
	unsigned int tb_steps = 0;
	double calc_time = 0.0;
	double tb_time = 0.0;
	
	static unsigned int rows;
	static unsigned int columns;
	static int** score_matrix;
	static char *q, *d;
	
	if (omp_get_dynamic())
		omp_set_dynamic(0);
	
	/* Setting the private values for our threads */
	#pragma omp threadprivate(check,max_score,rows,columns,score_matrix,q,d)
	
	#pragma omp parallel num_threads(threads)
	{
		rows = q_size+1;
		columns = d_size+1;
		score_matrix = calloc (rows, sizeof(*score_matrix));
		for (int i = 0; i < rows; i++)
			score_matrix[i] = calloc (columns, sizeof(*score_matrix[i]));
		q = malloc(sizeof(char[q_size+1]));
		d = malloc(sizeof(char[d_size+1]));
	}

	/*
	 * Using critical over ordered brought 70% speedup and 65% better times
	 * than the serial code
	 * Ordered was 16% slower than serial
	 * Critical outputs pairs in scrambled order, while ordered did not
	 */
	#pragma omp parallel for ordered num_threads(threads) schedule(static,1)
	for (pair = 1; pair <= pair_size; pair++){
		
		#pragma omp critical
		check = parse_file(in_file, out_file, q, d);
		
		if (check == 1){
			printf("Thread %d encountered error in parsing pair %d\n", \
					omp_get_thread_num(), pair);
			pair = pair_size+1;
        }
		
		rows = strlen(q) + 1;
		columns = strlen(d) + 1;
		cell_count += rows*columns;
		calculate_score(score_matrix, match, mismatch, gap, threads, q,\
						d, &max_score, &calc_time);

		#pragma omp critical
		{
			fprintf(out_file, "\n");
			if(traceback(score_matrix, &max_score, out_file, q, d, \
						 &tb_steps, &tb_time) == 1){
				printf("Thread %d encountered error in traceback, pair %d\n", \
						omp_get_thread_num(), pair);
			}
		}
		/* Used to break here, up-ed the pair no to get out of the for instead*/
		if (check == 2){
			pair = pair_size+1;
		}
	}
	
	double end_time = gettime()-start_time;
	printf("Total cells utilized: %u\n", cell_count);
	printf("Total traceback steps: %u\n", tb_steps);
	printf("Elapsed time: %.3f s\n", end_time);
	
	#pragma omp parallel num_threads(threads)
	{
		for (int i = 0; i < q_size+1; i++)
			free(score_matrix[i]);
		free(score_matrix);
		free(q);
		free(d);
		empty(&max_score);
	}
	
	free(out_source);
	fclose(in_file);
	fclose(out_file);
	
	return 0;
}
