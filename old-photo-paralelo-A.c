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
	if (create_directory(OLD_IMAGE_DIR) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", OLD_IMAGE_DIR);
		exit(-1);
	}

	gdImagePtr in_texture_img =  read_png_file("./paper-texture.png");

	clock_gettime(CLOCK_MONOTONIC, &end_time_seq);
	clock_gettime(CLOCK_MONOTONIC, &start_time_par);

	/* thread initialization */
	pthread_t thread_id[thread_num];

	/* Iteration over all the files to resize images */
	int i = 0;
	Thread_params* params[thread_num];
	while(files[i] != NULL){

		for(int j = 0; j < thread_num; j++){

			params[j] = malloc(sizeof(Thread_params));
			params[j]->arg = argv[1];
			params[j]->png_img = in_texture_img;
			params[j]->file = files[i];

			pthread_create(&thread_id[j], NULL, thread_func, (void*)params[j]);

			i++;
		}

		for (int j = 0; j < thread_num; j++) {
			pthread_join(thread_id[j], NULL);
			free(params[j]);
    }
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


	exit(0);
}

