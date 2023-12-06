/******************************************************************************
 * Programacao Concorrente
 * MEEC 21/22
 *
 * Projecto - Parte1
 *                           serial-complexo.c
 * 
 * Compilacao: gcc serial-complexo -o serial-complex -lgd
 *           
 *****************************************************************************/

#include <gd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <time.h> 
#include <pthread.h>
#include "image-lib.h"

/******************************************************************************
 * main()
 *
 * Arguments: (none)
 * Returns: 0 in case of sucess, positive number in case of failure
 * Side-Effects: creates thumbnail, resized copy and watermarked copies
 *               of images
 *
 * Description: implementation of the complex serial version 
 *              This application only works for a fixed pre-defined set of files
 *
 *****************************************************************************/
int main(int argc, char *argv[]){
    struct timespec start_time_total, end_time_total;
    struct timespec start_time_seq, end_time_seq;
    struct timespec start_time_par, end_time_par;

	if(argc != 3) return 0;
	int thread_num = atoi(argv[2]);

	clock_gettime(CLOCK_MONOTONIC, &start_time_total);
	clock_gettime(CLOCK_MONOTONIC, &start_time_seq);
	/* array containg the names of files to be processed	 */
	char **files =  get_images(argv[1]);

	/* input images */
	gdImagePtr in_img;

	/* creation of output directories */
	if (create_directory(OLD_IMAGE_DIR, argv[1]) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", OLD_IMAGE_DIR);
		exit(-1);
	}

	gdImagePtr in_texture_img =  read_png_file("./paper-texture.png");

	clock_gettime(CLOCK_MONOTONIC, &end_time_seq);
	clock_gettime(CLOCK_MONOTONIC, &start_time_par);

	/* thread initialization */
	pthread_t thread_id[thread_num];

	/* open timing_n.txt file */
	char timing[256];
	sprintf(timing, "%s%s%d", argv[1], "/timing_", thread_num);
	FILE *timing_n = fopen(timing, "w");

	/* Iteration over all the files to resize images */
	int aux = 0;
	
	/* we can pass multiple elements through a single argument by using a struct */
	struct timespec* result[thread_num]; /* execution time */

	for (int i = 0; i < thread_num; i++) {
    	result[i] = malloc(sizeof(struct timespec));
	}

	/* mark the files array index which has a NULL object */
	int j = 0;
	while(files[j] != NULL){
		j++;
		if(files[j] == NULL) aux = j;
	}

	j = 0;
	while(j < aux){

		Thread_params* params[thread_num];
		for (int i = 0; i < thread_num; i++) {
			params[i] = malloc(sizeof(Thread_params));
			params[i]->arg = argv[1];
			params[i]->png_img = in_texture_img;
			if (j + i > aux) { 
				params[i]->file = NULL;
			} else {
				params[i]->file = files[i + j]; 
			}
			pthread_create(&thread_id[i], NULL, thread_func, (void*)params[i]);
		}

		/* wait for threads to finish */
		for (int k = 0; k < thread_num; k++) {
			pthread_join(thread_id[k], (void **)result[k]);

			printf("Thread %d result: %10jd.%09ld seconds\n", k, result[k]->tv_sec, result[k]->tv_nsec); // %.9Lf prints up to 9 decimal places

			free(params[k]);
		}
		j += thread_num;
	
	}
	
	for (int j = 0; j < thread_num; j++) {
    	free(result[j]);
	}

	free_array(files);

	clock_gettime(CLOCK_MONOTONIC, &end_time_par);
	clock_gettime(CLOCK_MONOTONIC, &end_time_total);

	struct timespec par_time = diff_timespec(&end_time_par, &start_time_par);
	struct timespec seq_time = diff_timespec(&end_time_seq, &start_time_seq);
	struct timespec total_time = diff_timespec(&end_time_total, &start_time_total);
    printf("\tseq \t %10jd.%09ld\n", seq_time.tv_sec, seq_time.tv_nsec);
    printf("\tpar \t %10jd.%09ld\n", par_time.tv_sec, par_time.tv_nsec);
    printf("total \t %10jd.%09ld\n", total_time.tv_sec, total_time.tv_nsec);
	fprintf(timing_n, "total %d %10jd.%09ld\n", thread_num, total_time.tv_sec, total_time.tv_nsec);
	fclose(timing_n);

	exit(0);
}

