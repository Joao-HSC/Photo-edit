/******************************************************************************
 * Programacao Concorrente
 * MEEC 22/23
 *
 * Projecto - Parte A
 *                           apply_sepia.c
 * 
 * Compilacao: gcc apply_sepia.c -o apply_sepia -lgd
 *           
 *****************************************************************************/

#include <gd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include "image-lib.h"

/* the directories wher output files will be placed */
#define SEPIA_DIR "./Sepia-dir/"

/******************************************************************************
 * main()
 *
 * Arguments: (none)
 * Returns: 0 in case of sucess, positive number in case of failure
 * Side-Effects: creates a copy of images with sepia applied
 *
 * Description: Example of application of image transformation
 *              This application only works for a fixed pre-defined set of files
 *
 *****************************************************************************/
int main(){

	/* array containg the names of files to be processed	 */
	char * files [] =  {"IST-0.jpeg", "IST-1.jpeg", "IST-2.jpeg", "IST-3.jpeg", "IST-4.jpeg", "IST-5.jpeg", "IST-6.jpeg", "IST-7.jpeg", "IST-8.jpeg", "IST-9.jpeg"};
	/* length of the files array (number of files to be processed	 */
	int nn_files = 10;


	/* file name of the image created and to be saved on disk	 */
	char out_file_name[100];

	/* input images */
	gdImagePtr in_img;
	/* output images */
	gdImagePtr out_sepia_img;

	/* creation of output directories */
	if (create_directory(SEPIA_DIR) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", SEPIA_DIR);
		exit(-1);
	}

	/* Iteration over all the files to apply sepia images
	 */
	for (int i = 0; i < nn_files; i++){	

		printf("sepia %s\n", files[i]);
		/* load of the input file */
	    in_img = read_jpeg_file(files[i]);
		if (in_img == NULL){
			fprintf(stderr, "Impossible to read %s image\n", files[i]);
			continue;
		}
		/* apply sepia of each image */
		out_sepia_img = sepia_image(in_img);
  		if (out_sepia_img == NULL) {
            fprintf(stderr, "Impossible to apply sepia %s image\n", files[i]);
        }else{
			/* save new image */
			sprintf(out_file_name, "%s%s", SEPIA_DIR, files[i]);
			if(write_jpeg_file(out_sepia_img, out_file_name) == 0){
	            fprintf(stderr, "Impossible to write %s image\n", out_file_name);
			}
			gdImageDestroy(out_sepia_img);
		}
		gdImageDestroy(in_img);
	}

	exit(0);
}
