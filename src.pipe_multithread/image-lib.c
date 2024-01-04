#include <sys/stat.h>
#include <dirent.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include "image-lib.h"
/* the directories where the output files will be placed */
#define OLD_IMAGE_DIR "/Old-image-PAR-B"

/******************************************************************************
 * texture_image()
 *
 * Arguments: in - pointer to image
 *            texture - pointer to texture image
 * Returns: out - pointer to image with watermark, or NULL in case of failure
 * Side-Effects: none
 *
 * Description: creates clone of image and puts a watermark on it
 *
 *****************************************************************************/
gdImagePtr  texture_image(gdImagePtr in_img, gdImagePtr texture_img){
	
	gdImagePtr out_img;

	int width,heigth;

	width = in_img->sx;
	heigth = in_img->sy;


	gdImageSetInterpolationMethod(texture_img, GD_BILINEAR_FIXED);
    gdImagePtr scalled_pattern = gdImageScale(texture_img, width, heigth);


	out_img =  gdImageClone (in_img);

	gdImageCopy(out_img, scalled_pattern, 0, 0, 0, 0, width, heigth);
	gdImageDestroy(scalled_pattern);
	return(out_img);		
} 




/******************************************************************************
 * smooth_image()
 *
 * Arguments: in - pointer to image
 * Returns: out - pointer to smoother image, or NULL in case of failure
 * Side-Effects: none
 *
 * Description: creates clone of image smoother
 *
 *****************************************************************************/
gdImagePtr  smooth_image(gdImagePtr in_img){
	
	gdImagePtr out_img;
	
	out_img =  gdImageClone (in_img);
	if (!out_img) {
		return NULL;
	}

	int ret = gdImageSmooth(out_img, 20);


	if (!out_img) {
		return NULL;
	}

	return(out_img);		
} 




/******************************************************************************
 * contrast_image()
 *
 * Arguments: in - pointer to image
 * Returns: out - pointer to high contrast  image, or NULL in case of failure
 * Side-Effects: none
 *
 * Description: creates clone of image but its contrast reduced
 *
 *****************************************************************************/
gdImagePtr contrast_image(gdImagePtr in_img){
	
	gdImagePtr out_img;
	
	out_img =  gdImageClone(in_img);
	if (!out_img) {
		return NULL;
	}

	int ret = gdImageContrast(out_img, -20);


	return(out_img);		
} 



/******************************************************************************
 * sepia_image()
 *
 * Arguments: in - pointer to image
 * Returns: out - pointer to sepia image, or NULL in case of failure
 * Side-Effects: none
 *
 * Description: creates clone of image but with sepia color
 *
 *****************************************************************************/
gdImagePtr  sepia_image(gdImagePtr in_img){
	
	gdImagePtr out_img;
	
	out_img =  gdImageClone (in_img);
	if (!out_img) {
		return NULL;
	}

	int ret = gdImageColor(out_img, 100, 60, 0, 0);


	return(out_img);		
} 




/******************************************************************************
 * read_png_file()
 *
 * Arguments: file_name - name of file with data for PNG image
 * Returns: img - the image read from file or NULL if failure to read
 * Side-Effects: none
 *
 * Description: reads a PNG image from a file
 *
 *****************************************************************************/
gdImagePtr read_png_file(char * file_name){

	FILE * fp;
	gdImagePtr read_img;

	fp = fopen(file_name, "rb");
   	if (!fp) {
        fprintf(stderr, "Can't read image %s\n", file_name);
        return NULL;
    }
    read_img = gdImageCreateFromPng(fp);
    fclose(fp);
  	if (read_img == NULL) {
    	return NULL;
    }

	return read_img;
}

/******************************************************************************
 * write_png_file()
 *
 * Arguments: img - pointer to image to be written
 *            file_name - name of file where to save PNG image
 * Returns: (bool) 1 in case of success, 0 in case of failure to write
 * Side-Effects: none
 *
 * Description: writes a PNG image to a file
 *
 *****************************************************************************/
int write_png_file(gdImagePtr write_img, char * file_name){
	FILE * fp;

	fp = fopen(file_name, "wb");
	if (fp == NULL) {
		return 0;
	}
	gdImagePng(write_img, fp);
	fclose(fp);

	return 1;
}

/******************************************************************************
 * read_jpeg_file()
 *
 * Arguments: file_name - name of file with data for JPEG image
 * Returns: img - the image read from file or NULL if failure to read
 * Side-Effects: none
 *
 * Description: reads a JPEG image from a file
 *
 *****************************************************************************/
gdImagePtr read_jpeg_file(char * file_name){

	FILE * fp;
	gdImagePtr read_img;

	fp = fopen(file_name, "rb");
   	if (!fp) {
        fprintf(stderr, "Can't read image %s\n", file_name);
        return NULL;
    }
    read_img = gdImageCreateFromJpeg(fp);
    fclose(fp);
  	if (read_img == NULL) {
    	return NULL;
    }

	return read_img;
}

/******************************************************************************
 * write_jpeg_file()
 *
 * Arguments: img - pointer to image to be written
 *            file_name - name of file where to save PNG image
 * Returns: (bool) 1 in case of success, 0 in case of failure to write
 * Side-Effects: none
 *
 * Description: writes a JPEG image to a file
 *
 *****************************************************************************/
int write_jpeg_file(gdImagePtr write_img, char * file_name){
	FILE * fp;

	fp = fopen(file_name, "wb");
	if (fp == NULL) {
		return 0;
	}
	gdImageJpeg(write_img, fp, 70);
	fclose(fp);

	return 1;
}


/******************************************************************************
 * create_directory()
 *
 * Arguments: file_name - name of directory to be created
 * Returns: (bool) 1 if the directory already exists or succesfully created
 *                 0 in case of failure to create
 * Side-Effects: none
 *
 * Description: Create a directory. 
 *
 *****************************************************************************/
int create_directory(char * dir_name, char* folder){

	char* pre_dir = malloc(sizeof(char) * (strlen(folder) + strlen(dir_name) + 1));
	strcpy(pre_dir, folder);

	DIR * d = opendir(strcat(pre_dir, dir_name));
	if (d == NULL){
		if (mkdir(pre_dir, 0777)!=0){
			free(pre_dir);
			return 0;
		}
	}else{
		closedir(d);
	}
	free(pre_dir);
	
	return 1;
}



/******************************************************************************
 * diff_timespec()
 *
 * Arguments: time 1 - structure containg the time seasure with  clock_gettime
 *            time 2 - structure containg the time seasure with  clock_gettime
 * Returns: (struct timespec) 
 * Side-Effects: none
 *
 * Description: This functino subtracts the two received times and returns the result
 *
 *****************************************************************************/
struct timespec diff_timespec(const struct timespec *time1, const struct timespec *time0) {
  assert(time1);
  assert(time0);
  struct timespec diff = {.tv_sec = time1->tv_sec - time0->tv_sec,
      .tv_nsec = time1->tv_nsec - time0->tv_nsec};
  if (diff.tv_nsec < 0) {
    diff.tv_nsec += 1000000000; /* nsec/sec */
    diff.tv_sec--;
  }
  return diff;
}

/******************************************************************************
 * get_images()
 *
 * Arguments: folder in which image-list.txt is in
 * Returns: array with the images' names
 * Side-Effects: none
 *
 * Description: returns and array with every image's file name
 *
 *****************************************************************************/
char** get_images(char *folder){

	char file[256];
	strcpy(file, folder);
	strcat(file, "/image-list.txt");
	FILE *image_txt = fopen(file, "r");
	
	int array_cap = 10;
	char **images = malloc(array_cap * sizeof(char *));;
	int n_images = 0;
	char line[256];

    /* Read lines from the file */
    while (fgets(line, sizeof(line), image_txt) != NULL) {
        /* Remove newline character, if present */
        char *newline = strchr(line, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }

        /* Allocate memory for the new string */
        char* newImage = strdup(line);

        /* Resize the array of strings */
        if (n_images == array_cap) {
            /* Double the capacity */
            array_cap *= 2;
            images = realloc(images, array_cap * sizeof(char *));
   	    }
	 	images[n_images] = newImage;
        n_images++;
	}

	images = realloc(images, (n_images + 1) * sizeof(char *));
  
    images[n_images] = NULL;
	fclose(image_txt);

	return images;
}

/******************************************************************************
 * free_array()
 *
 * Arguments: array
 * Returns: nothing
 * Side-Effects: none
 *
 * Description: frees an array
 *
 *****************************************************************************/

void free_array(char** array){

	int i = 0;

	while(array[i] != NULL){
		free(array[i]);
		i++;
	}
	free(array);

	return;
}
/******************************************************************************
 * image_transform()
 *
 * Arguments: input image, .png transformation
 * Returns: output image
 * Side-Effects: none
 *
 * Description: applies the desired transformations to the input image
 *
 *****************************************************************************/
gdImagePtr image_transform(gdImagePtr input, gdImagePtr png_transform){

	/* output images */
	gdImagePtr out_smoothed_img;
	gdImagePtr out_contrast_img;
	gdImagePtr out_textured_img;
	gdImagePtr out_sepia_img;

	out_contrast_img = contrast_image(input);
	out_smoothed_img = smooth_image(out_contrast_img);
	gdImageDestroy(out_contrast_img); 
	out_textured_img = texture_image(out_smoothed_img , png_transform);
	gdImageDestroy(out_smoothed_img);
	out_sepia_img = sepia_image(out_textured_img); 
	gdImageDestroy(out_textured_img);

	return out_sepia_img;
}

/******************************************************************************
 * thread_func()
 *
 * Arguments: none
 * Returns: NULL
 * Side-Effects: none
 *
 * Description: applies the desired transformations to the input image and writes
 * 				it in the desired folder
 *
 *****************************************************************************/
void* thread_func(void* thread_n){
   
	struct timespec start_time;
	struct timespec end_time;
	struct timespec start_image_time;
	struct timespec end_image_time;
	struct timespec *thr_time = malloc(sizeof(struct timespec));
	struct timespec *image_t = malloc(sizeof(struct timespec));
	
	clock_gettime(CLOCK_MONOTONIC, &start_time);

	/* copy of the struct */
	Thread_params* thread_params = malloc(sizeof(Thread_params));
	
	int n_images = 0;

	while(read(pipefd[0], thread_params, sizeof(Thread_params)) > 0){

		clock_gettime(CLOCK_MONOTONIC, &start_image_time);

		if(thread_params == NULL) continue;
		/* file name of the image created and to be saved on disk */
		char* out_file_name = malloc(256 * sizeof(char));
		gdImagePtr out_img;

		char* filepath = malloc(256 * sizeof(char));
		sprintf(filepath, "%s/%s", thread_params->arg, thread_params->file);
		filepath = realloc(filepath, strlen(filepath) + 1);

		/* load of the input file */
		gdImagePtr in_img = read_jpeg_file(filepath);
		if (in_img == NULL){
			free(filepath);
			free(out_file_name);
			gdImageDestroy(in_img);
			return NULL;
		}
		
		out_img = image_transform(in_img, thread_params->png_img);

		/* save resized */ 
		sprintf(out_file_name, "%s%s/%s", thread_params->arg, OLD_IMAGE_DIR, thread_params->file);
		out_file_name = realloc(out_file_name, strlen(out_file_name) + 1);
		if(write_jpeg_file(out_img, out_file_name) == 0){
			fprintf(stderr, "Impossible to write %s image\n", out_file_name);
		}

		/* write time it took to transform the image */
		clock_gettime(CLOCK_MONOTONIC, &end_image_time);
		*image_t = diff_timespec(&end_image_time, &start_image_time);
		fprintf(timing_n, "%s %10jd.%09ld\n", thread_params->file, image_t->tv_sec, image_t->tv_nsec);
		
		free(filepath);
		free(out_file_name);
		gdImageDestroy(in_img);
		gdImageDestroy(out_img);

		n_images++;
	}
	
	clock_gettime(CLOCK_MONOTONIC, &end_time);
	*thr_time = diff_timespec(&end_time, &start_time);
	fprintf(timing_n, "thread_%d %d %10jd.%09ld\n", (intptr_t)thread_n, n_images, thr_time->tv_sec, thr_time->tv_nsec);

	gdImageDestroy(thread_params->png_img);
	free(thread_params);
	free(thr_time);
	free(image_t);
	return (void *)(intptr_t)n_images; 
	
}
