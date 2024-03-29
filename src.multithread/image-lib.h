#include "gd.h"
#include <unistd.h>
#define OLD_IMAGE_DIR "/Old-image-PAR-A"

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
gdImagePtr  texture_image(gdImagePtr in_img, gdImagePtr watermark);



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
gdImagePtr  smooth_image(gdImagePtr in_img);

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
gdImagePtr  sepia_image(gdImagePtr in_img);


/******************************************************************************
 * contrast_image()
 *
 * Arguments: in - pointer to image
 * Returns: out - pointer to transformed image, or NULL in case of failure
 * Side-Effects: none
 *
 * Description: creates clone of image but its contrast reduced
 *
 *****************************************************************************/
gdImagePtr  contrast_image(gdImagePtr in_img);


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
gdImagePtr read_png_file(char * file_name);

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
int write_png_file(gdImagePtr write_img, char * file_name);



/******************************************************************************
 * read_jpeg_file()
 *
 * Arguments: file_name - name of file with data for JPEG image
 * Returns: img - the image read from file or NULL if failure to read
 * Side-Effects: none
 *
 * Description: reads a PNG image from a file
 *
 *****************************************************************************/
gdImagePtr read_jpeg_file(char * file_name);

/******************************************************************************
 * write_jpeg_file()
 *
 * Arguments: img - pointer to image to be written
 *            file_name - name of file where to save JPEG image
 * Returns: (bool) 1 in case of success, 0 in case of failure to write
 * Side-Effects: none
 *
 * Description: writes a PNG image to a file
 *
 *****************************************************************************/
int write_jpeg_file(gdImagePtr write_img, char * file_name);

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
int create_directory(char * dir_name, char* pre_dir);

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
char** get_images(char *folder);

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
void free_array(char** array);

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
gdImagePtr image_transform(gdImagePtr input, gdImagePtr png_transform);

/******************************************************************************
 * thread_func()
 *
 * Arguments: struct
 * Returns: NULL
 * Side-Effects: none
 *
 * Description: applies the desired transformations to the input image and writes
 * 				it in the desired folder
 *
 *****************************************************************************/
void* thread_func(void* params);

/* structure for the thread parameters */
typedef struct{
    char* arg;
    char* file;
    gdImagePtr png_img;
} Thread_params;

/* structure for the thread times */
typedef struct{
    struct timespec time;
    int n_images;
} Thread_times;

struct timespec diff_timespec(const struct timespec *time1, const struct timespec *time0);
