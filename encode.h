#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types
#include<string.h>
/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;//beautiful.bmp
    FILE *fptr_src_image;//file address is stored
    uint image_capacity;//Size of beautiful.bmp include headder
    uint bits_per_pixel;//no of bits per pixel is stored 3byte for 1 pixel 
    char image_data[MAX_IMAGE_BUF_SIZE];//size of beautiful.bmp excluding headder

    /* Secret File Info */
    char *secret_fname;//secret.txt is stored
    FILE *fptr_secret;//file address is given
    char extn_secret_file[MAX_FILE_SUFFIX];//.txt is stored
    char secret_data[MAX_SECRET_BUF_SIZE];//MAX_SECRET_BUF_SIZE * 8
    long size_secret_file;//size of secret file

    /* Stego Image Info */
    char *stego_image_fname;//user given name or default name
    FILE *fptr_stego_image;//Address of stego image file

} EncodeInfo;


/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif
