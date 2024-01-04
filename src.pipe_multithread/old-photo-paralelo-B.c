/******************************************************************************
 * Programacao Concorrente
 * MEEC 21/22
 *
 * Projecto - Parte B
 *                           paralelo.c
 * 
 * Compilacao: gcc old-photo-paralelo-B.c image-lib.c -g -o
 * 			   old-photo-paralelo-B -lgd
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

/* declare pipe */
int pipefd[2];

/* declare timing file */
FILE *timing_n;

/******************************************************************************
 * main()
 *
 * Arguments: argc and argv
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

	/* creation of output directories */
	if (create_directory(OLD_IMAGE_DIR, argv[1]) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", OLD_IMAGE_DIR);
		exit(-1);
	}

	/* input images */
	gdImagePtr in_img;

	/* read png image */
	gdImagePtr in_texture_img =  read_png_file("./paper-texture.png");

	/* thread initialization */
	pthread_t thread_id[thread_num];

	/* declare pipe */
	pipe(pipefd);

	/* open timing_n.txt file */
	char timing[256];
	sprintf(timing, "%s%s%d", argv[1], "/timing_", thread_num);
	timing_n = fopen(timing, "w");

	/* mark the files array index which has a NULL object */
	int cut_off = 0;
	while(files[cut_off] != NULL){
		cut_off++;
	}

	/* start reading threads */
	for(int i = 0; i < thread_num; i++){
		pthread_create(&thread_id[i], NULL, thread_func, (void*)(intptr_t)i);
	}

	/* start writing */
	Thread_params* params[cut_off];

	/* initialize vector to store binary (0 and -1) between a file being accessible or not */
	int file_ok[cut_off];

	/* Iteration over all the files and write them to the pipe */
	int i = 0;
	while(i < cut_off){
		params[i] = malloc(sizeof(Thread_params));
		params[i]->arg = argv[1];
		params[i]->png_img = in_texture_img;
		params[i]->file = files[i]; 

		/* check to see if the file has already been parsed */
		char path[256]; 
		sprintf(path, "%s/%s/%s", argv[1], OLD_IMAGE_DIR, params[i]->file);
		file_ok[i] = access(path, F_OK);

		/* write to the pipe if file is not accessible */
		if(file_ok[i] == -1){
			write(pipefd[1], params[i], sizeof(Thread_params));
		}
		i++;
	}
	close(pipefd[1]);
	
	int total_images = 0; 
	void* images;

	for (int i = 0; i < thread_num; i++) {
		pthread_join(thread_id[i], (void*)&images);
		total_images += (intptr_t)images;
	}

	for(i = 0; i < cut_off; i++) free(params[i]);
	
	free_array(files);

	clock_gettime(CLOCK_MONOTONIC, &end_time_total);
	struct timespec total_time = diff_timespec(&end_time_total, &start_time_total);
	fprintf(timing_n, "total %d %10jd.%09ld\n", total_images, total_time.tv_sec, total_time.tv_nsec);

	fclose(timing_n);

	exit(0);
}

