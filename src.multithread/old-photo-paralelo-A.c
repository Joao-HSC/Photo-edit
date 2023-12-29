/******************************************************************************
 * Programacao Concorrente
 * MEEC 21/22
 *
 * Projecto - Parte A
 *                           paralelo.c
 * 
 * Compilacao: gcc old-photo-paralelo-A.c image-lib.c -g -o
 * 			   old-photo-paralelo-A -lgd
 *           
 *****************************************************************************/

#include <gd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <time.h> 
#include <pthread.h>
#include <unistd.h>
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

	if(argc != 3) return 0;
	int thread_num = atoi(argv[2]);

	clock_gettime(CLOCK_MONOTONIC, &start_time_total);

	/* array containg the names of files to be processed */
	char **files =  get_images(argv[1]);

	/* input images */
	gdImagePtr in_img;

	/* creation of output directories */
	if (create_directory(OLD_IMAGE_DIR, argv[1]) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", OLD_IMAGE_DIR);
		exit(-1);
	}

	gdImagePtr in_texture_img =  read_png_file("./paper-texture.png");

	/* thread initialization */
	pthread_t thread_id[thread_num];

	/* open timing_n.txt file */
	char timing[256];
	sprintf(timing, "%s%s%d", argv[1], "/timing_", thread_num);
	FILE *timing_n = fopen(timing, "w");
	
	/* alloc vector so we can store the execution time values of each thread */
	struct timespec *result; 
        result = malloc(thread_num * sizeof(struct timespec));
	/* mark the files array index which has a NULL object */
	int j = 0;
	int aux = 0;
	while(files[j] != NULL){
		j++;
		if(files[j] == NULL) aux = j;
	}
	j = 0;

	Thread_times* times[thread_num];
	for(int i = 0; i < thread_num; i++){
		times[i] = malloc(sizeof(Thread_times));
		times[i]->n_images = 0;
	}
	/* Iteration over all the files to resize images */
	while(j < aux){

		/* initialize vector to store binary (0 and -1) between a file being accessible or not */
		int file_ok[thread_num];
		/* we can pass multiple elements through a single argument by using a struct */
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

			/* check to see if the file has already been parsed */
			char path[256]; 
			sprintf(path, "%s/%s/%s", argv[1], OLD_IMAGE_DIR, params[i]->file);
			file_ok[i] = access(path, F_OK);
			/* create thread if file is not accessible */
			if(file_ok[i] == -1){
				pthread_create(&thread_id[i], NULL, thread_func, (void*)params[i]);
			}
			
		}

		void* timer = malloc(sizeof(void*));
		/* wait for threads to finish */
		for (int k = 0; k < thread_num; k++) {
			if(file_ok[k] == -1){
				pthread_join(thread_id[k], &timer);
				times[k]->time = *(struct timespec*) timer;
				if(params[k]->file != NULL){
					times[k]->n_images += 1;
				}
				free(params[k]);
			}
		}
		free(timer);
		j += thread_num;
	
	}

        free(result);
	free_array(files);

	clock_gettime(CLOCK_MONOTONIC, &end_time_total);

	struct timespec total_time = diff_timespec(&end_time_total, &start_time_total);
	fprintf(timing_n, "total %d %10jd.%09ld\n", thread_num, total_time.tv_sec, total_time.tv_nsec);

	for(int i = 0; i < thread_num; i++){
		fprintf(timing_n, "Thread_%d %d %10jd.%09ld\n", i, times[i]->n_images, times[i]->time.tv_sec, times[i]->time.tv_nsec);
		free(times[i]);
	}
	fclose(timing_n);

	exit(0);
}

