 /* FILE: A3_solutions.c is where you will code your answers for Assignment 3.
 * 
 * Each of the functions below can be considered a start for you. They have 
 * the correct specification and are set up correctly with the header file to
 * be run by the tester programs.  
 *
 * You should leave all of the code as is, especially making sure not to change
 * any return types, function name, or argument lists, as this will break
 * the automated testing. 
 *
 * Your code should only go within the sections surrounded by
 * comments like "REPLACE EVERTHING FROM HERE... TO HERE.
 *
 * The assignment document and the header A3_solutions.h should help
 * to find out how to complete and test the functions. Good luck!
 *
 */
 /*Name: Robert Jung
  ID: 260769020
  */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "A3_provided_functions.h"

//these are the offsets for BMP header fields.
#define BMP_SIZE         2  // Offset for file size
#define BMP_OFFSET      10  // Offset for the image data offset field
#define BMP_HEADER_SIZE 14  // Offset for the header size field
#define BMP_WIDTH       18  // Offset for the width field
#define BMP_HEIGHT      22  // Offset for the height field
#define BMP_BPP         28  // Offset for the bits per pixel (BPP) field
#define BMP_IMAGE_SIZE  34  // Offset for the image size field


//added this structure to define the necessary fields and their sizes. 
struct BMPFields {
  uint32_t size;
  uint32_t offset;
  uint32_t width;
  uint32_t height;
  uint16_t bits_per_pixel;
};

unsigned char*
bmp_open( char* bmp_filename,        unsigned int *width, 
          unsigned int *height,      unsigned int *bits_per_pixel, 
          unsigned int *padding,     unsigned int *data_size, 
          unsigned int *data_offset                                  )
{

  unsigned char *img_data;
  struct BMPFields bmp_header;
  unsigned int width_bytes;

  // Opening the bmp file. 
  FILE *bmp = fopen(bmp_filename, "rb");

  if (bmp == NULL) {
    return NULL;
  }

  // Reading the BMPFields required to produce the bmp_info output

  fseek(bmp, BMP_SIZE, SEEK_SET);
  fread(&bmp_header.size, sizeof(bmp_header.size), 1, bmp);

  fseek(bmp, BMP_OFFSET, SEEK_SET);
  fread(&bmp_header.offset, sizeof(bmp_header.offset), 1, bmp);

  fseek(bmp, BMP_HEIGHT, SEEK_SET);
  fread(&bmp_header.height, sizeof(bmp_header.height), 1, bmp);  

  fseek(bmp, BMP_WIDTH, SEEK_SET);
  fread(&bmp_header.width, sizeof(bmp_header.width), 1, bmp);

  fseek(bmp, BMP_BPP, SEEK_SET);
  fread(&bmp_header.bits_per_pixel, sizeof(bmp_header.bits_per_pixel), 1, bmp);

  // We can now copmute the width of a row. 
  // We can also compute the padding by assuming that width should be a multiple of 4. 

  width_bytes = bmp_header.width * bmp_header.bits_per_pixel / 8;
  *padding = (4 - (width_bytes % 4)) % 4;

  // Copying the field values into the pointers. 
  *data_size = bmp_header.size;
  *width = bmp_header.width;
  *height = bmp_header.height;
  *bits_per_pixel = bmp_header.bits_per_pixel;
  *data_offset = bmp_header.offset;

  img_data = (unsigned char *)malloc(bmp_header.size);

  // Seek back to the start of the file and read in the entire file
  fseek(bmp, 0, SEEK_SET);
  fread(img_data, bmp_header.size, 1, bmp);

  // Close the file and return
  fclose(bmp);
  return img_data;  
}

void 
bmp_close( unsigned char **img_data )
{
 free(*img_data);
}

unsigned char***  
bmp_scale( unsigned char*** pixel_array, unsigned char* header_data, unsigned int header_size,
           unsigned int* width, unsigned int* height, unsigned int num_colors,
           float scale )
{
  unsigned int i, j, k, i_scale, j_scale;
  unsigned char *** new_pixel_array;

  *height = (unsigned int)(*height * scale);
  *width = (unsigned int)(*width * scale);

  // allocating the heights
  new_pixel_array = (unsigned char ***)malloc(sizeof(unsigned char **) * (*height));

  for (i = 0; i < *height; i++) {
    
    //allocating the widths. 
    new_pixel_array[i] = (unsigned char **)malloc(sizeof(unsigned char *) * (*width));

    for (j = 0; j < *width; j++) {

      new_pixel_array[i][j] = (unsigned char *)malloc(sizeof(unsigned char) * num_colors);
      
      // Computing the original pixel value from the original image. 
      i_scale = (unsigned int)(i / scale);
      j_scale = (unsigned int)(j / scale);

      for (k = 0; k < num_colors; k++) {
        // Copying the color in the old array to the new array. 
        new_pixel_array[i][j][k] = pixel_array[i_scale][j_scale][k];
      }
    }
  }

  // Computing the new padding. 
  uint32_t padding = (4 - (*width * num_colors) % 4) % 4;

  // Computing the size of each row in new image
  uint32_t row_size = *width * num_colors + padding;

  // Computing the total image size, width, height and image_size
  uint32_t bmp_size = row_size * (*height) + header_size;
  uint32_t bmp_width = (uint32_t)(*width);
  uint32_t bmp_height = (uint32_t)(*height);
  uint32_t bmp_image_size = bmp_size - header_size;

  // Replacing the bmp info with new info using memcpy. 
  memcpy(header_data + BMP_SIZE, &bmp_size, sizeof(bmp_size));
  memcpy(header_data + BMP_WIDTH, &bmp_width, sizeof(bmp_width));
  memcpy(header_data + BMP_HEIGHT, &bmp_height, sizeof(bmp_height));
  memcpy(header_data + BMP_IMAGE_SIZE, &bmp_image_size, sizeof(bmp_image_size));

  return new_pixel_array;
}       

//this is a helper method for bmp_collage - which frees a 3D array with a given width and height. 
void free_3d_array(unsigned char ***array, unsigned int width, unsigned int height) {

  unsigned int row, col;

  for (row = 0; row < height; row++) {
    for (col = 0; col < width; col++) {
      free(array[row][col]);
    }

    free(array[row]);
  }

  free(array);
}    



int 
bmp_collage( char* background_image_filename,     char* foreground_image_filename, 
             char* output_collage_image_filename, int row_offset,                  
             int col_offset,                      float scale )
{
  unsigned char *bg_header, *fg_header;
  unsigned int bg_header_size, bg_width, bg_height, bg_num_colors;
  unsigned int fg_header_size, fg_width, fg_height, fg_num_colors;

  // Using the bmp_to_3D_array function to open the file and converting it to a 3D array. 
  unsigned char ***background = bmp_to_3D_array(
    background_image_filename,
    &bg_header,
    &bg_header_size,
    &bg_width,
    &bg_height,
    &bg_num_colors
  );

  if (background == NULL) {
    printf("The background image file couldn't be opened.\n");
    return -1;
  }


  unsigned char ***foreground = bmp_to_3D_array(
    foreground_image_filename,
    &fg_header,
    &fg_header_size,
    &fg_width,
    &fg_height,
    &fg_num_colors
  );

  if (foreground == NULL) {
    printf("The foreground image couldn't be opened. \n");
    return -1;
  }

  if (bg_num_colors != 4) {
    printf("The background image doesn't have a BPP equaling to 32\n");
    return -1;
  } else if (fg_num_colors != 4) {
    printf("The foreground image doesn't have a BPP equaling to 32\n");
    return -1;
  }

  //we need to keep the old width and height of the foreground image before scaling it so we can free them later. 
  unsigned int old_fg_width = fg_width, old_fg_height = fg_height;

  //Scaling the foreground image.
  unsigned char ***scaled_foreground = bmp_scale(
    foreground, 
    fg_header, 
    fg_header_size, 
    &fg_width, 
    &fg_height, 
    fg_num_colors,
    scale
  );

  //freeing the foreground image. 
  free_3d_array(foreground, old_fg_width, old_fg_height);

  // Checking if the size of scaled foreground can fit the background image. 
  if (col_offset + fg_width > bg_width) {
    printf("The foreground image doesn't fit the width of the background. \n");
    return -1;
  }

  if (row_offset + fg_height > bg_height) {
    printf("The foreground image doesn't fit the heigth of the background\n");
    return -1;
  }

  unsigned int i, j, k;
  unsigned char fg_alpha;
  unsigned char *source_pixel, *dest_pixel;

  // Copying all values of the scaled foreground to the background image if its alpha >0. 
  for (i = 0; i < fg_height; i++) {
    for (j = 0; j < fg_width; j++) {

      
      source_pixel = scaled_foreground[i][j];
      fg_alpha = source_pixel[0]; // Color 0 is the alpha channel

      dest_pixel = background[row_offset + i][col_offset + j];

      if (fg_alpha > 0) {
        // Coping the color values. 
        for (k = 0; k < fg_num_colors; k++) {
          dest_pixel[k] = source_pixel[k];
        }
      }
    }
  }

  // Finally saving the 3D array to a bmp file. 
  bmp_from_3D_array(
    output_collage_image_filename,
    bg_header,
    bg_header_size,
    background,
    bg_width,
    bg_height,
    bg_num_colors
  );


  free_3d_array(scaled_foreground, fg_width, fg_height);
  free_3d_array(background, bg_width, bg_height);

  return 0;
}              

