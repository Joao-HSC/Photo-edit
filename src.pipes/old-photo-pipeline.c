/******************************************************************************
 * Programacao Concorrente
 * MEEC 21/22
 *
 * Projecto - Parte B
 *                           paralelo.c
 * 
 * Compilacao: gcc old-photo-pipeline.c image-lib.c -g -o
 * 			   old-photo-pipeline -lgd
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
#include <string.h>
#include "image-lib.h"

/* the directories where the output files will be placed */
#define OLD_IMAGE_DIR "/Old-photo-PIPELINE"

/* Num of threads */
#define THREAD_NUM 4

/* timing output file */
FILE *timing_n;

/* declare pipes*/
int pipefd1[2];
int pipefd2[2];
int pipefd3[2];
int pipefd4[2];

/* 1st pipe function */
void* transform1(void* arg) {
    
    Threadin* aux = (Threadin*)arg;

    Pipe_params params;

    while (read(pipefd1[0], &params, sizeof(Pipe_params)) > 0) {

        clock_gettime(CLOCK_MONOTONIC, &(params.time));                                        
        struct timespec start = diff_timespec(&params.time, &aux->total);                       
        char *out = strrchr(params.file_name, '/');
        fprintf(timing_n, "%s start %jd.%03ld\n", out + 1, start.tv_sec, start.tv_nsec); 

        gdImagePtr out_contrast_img = contrast_image(params.file);
        gdImageDestroy(params.file);
        params.file = out_contrast_img;
        write(pipefd2[1], &params, sizeof(Pipe_params));                                         
    }
    close(pipefd2[1]);                                                                        
}

/* 2nd pipe function */
void* transform2() {

	Pipe_params params;

	while (read(pipefd2[0], &params, sizeof(Pipe_params)) > 0){
		gdImagePtr out_smoothed_img = smooth_image(params.file);
		gdImageDestroy(params.file);	
        params.file = out_smoothed_img;

        /* send struct to next pipe */
        write(pipefd3[1],&params, sizeof(Pipe_params));
	}
    close(pipefd3[1]);                                                                        
}

/* 3rd pipe function */
void* transform3() {

	Pipe_params params;

	gdImagePtr in_texture_img = read_png_file("./paper-texture.png");

	while (read(pipefd3[0], &params, sizeof(Pipe_params)) > 0){
		gdImagePtr out_textured_img = texture_image(params.file, in_texture_img);
        gdImageDestroy(params.file);
        params.file = out_textured_img;
        write(pipefd4[1], &params, sizeof(Pipe_params));
	}
    close(pipefd4[1]);                                                                         
}

/* 4th pipe function */
void* transform4(void* arg) {

	Pipe_params params;
       
    Threadout *aux = (Threadout* )arg;

	while (read(pipefd4[0], &params, sizeof(Pipe_params)) > 0){ 

		gdImagePtr out_sepia_img = sepia_image(params.file);
        char *name_out = strrchr(params.file_name, '/');

        char *output = (char *)malloc(strlen(name_out) + strlen(aux->directory) + 1);
        strcpy(output, aux->directory);
        strcat(output, name_out);
        printf("%s\n", output);

        if (write_jpeg_file(out_sepia_img, output) == 0) {
            fprintf(stderr, "Impossible to write %s image\n", output);
        }

        /* timing file */
        clock_gettime(CLOCK_MONOTONIC, &(params.time));
        struct timespec end_img = diff_timespec(&params.time, &aux->total);
        fprintf(timing_n, "%s end %jd.%03ld\n", name_out + 1, end_img.tv_sec, end_img.tv_nsec);
    }
}


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

	if(argc != 2) return 0;
	
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

	/* declare pipes */
	if (pipe(pipefd1)!=0){
        exit(-1);
    }
    if (pipe(pipefd2)!=0){
        exit(-1);
    }
    if (pipe(pipefd3)!=0){
        exit(-1);
    }
    if (pipe(pipefd4)!=0){
        exit(-1);
    }

	/* open timing_n.txt file */
	char timing[256];
	sprintf(timing, "%s%s", argv[1], "/timing_pipeline");
	timing_n = fopen(timing, "w");

	/* mark the files array index which has a NULL object */
	int cut_off = 0;
	while(files[cut_off] != NULL){
		cut_off++;
	}

	/* initialize vector to store binary (0 and -1) between a file being accessible or not */
	int file_ok[cut_off];

	/* Iteration over all the files and write them to the pipe */
	Pipe_params* params[cut_off];
	int i = 0;
	while(i < cut_off){
		params[i] = malloc(sizeof(Pipe_params));
        char* filepath = malloc(256 * sizeof(char));
	    sprintf(filepath, "%s/%s", argv[1], files[i]);
		params[i]->file_name = files[i];
		params[i]->file = read_jpeg_file(filepath); 
		/* check to see if the file has already been parsed */
		char path[256]; 
		sprintf(path, "%s%s/%s", argv[1], OLD_IMAGE_DIR, params[i]->file_name);
		file_ok[i] = access(path, F_OK);

		/* write to the pipe if file is not accessible */
		if(file_ok[i] == -1){
			write(pipefd1[1], params[i], sizeof(Pipe_params));
		}
		i++;
	}
	close(pipefd1[1]);

    /* initialize thread info and start the threads */
	Threadout thread_res;
	Threadin thread_in;
	pthread_t thread_id[THREAD_NUM];
	thread_in.total = start_time_total;
	thread_res.directory = (char *)malloc(256);
    sprintf(thread_res.directory, "%s%s", argv[1], OLD_IMAGE_DIR);  
    thread_in.dir_path = thread_res.directory;
    thread_res.total = start_time_total;

	pthread_create(&thread_id[0], NULL, transform1, (void*)&thread_in);
    pthread_create(&thread_id[1], NULL, transform2, NULL);    
    pthread_create(&thread_id[2], NULL, transform3, NULL);
    pthread_create(&thread_id[3], NULL, transform4, (void *)&thread_res);

	for (i = 0; i < THREAD_NUM; i++) {
        pthread_join(thread_id[i], NULL);
    }

	for(i = 0; i < cut_off; i++) free(params[i]);
	
	free_array(files);
	free(thread_res.directory);
	clock_gettime(CLOCK_MONOTONIC, &end_time_total);
	struct timespec total_time = diff_timespec(&end_time_total, &start_time_total);
	fprintf(timing_n, "total %d %10jd.%09ld\n", cut_off, total_time.tv_sec, total_time.tv_nsec);

	fclose(timing_n);

	exit(0);
}

