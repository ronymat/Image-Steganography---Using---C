#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"

typedef struct _DecodeInfo
{
    //input stego image file
    char *stego_image_fname;  //encoded filename ie in (bmp)format  
    FILE *fptr_stego_image;   //image address is stored  
    // Secret file output
    char output_fname[50];  //store name of output file
    char extension[10];     //store extension
    FILE *fptr_output;      

    int  extn_size;       //extension size is stored
    int secret_file_size;     // Size of secret file
} DecodeInfo;

//Decode function prototypes

// Decode argument validation 
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
//open the file to be decoded
Status open_files_decode(DecodeInfo *decInfo);
//decode the each byte from lsb of image
char decode_byte_from_lsb(char *image_buffer);
//decode the secret data
Status decode_data_from_image(char *buffer, int size, FILE *fptr_stego_image);
//do decoding
Status do_decoding(DecodeInfo *decInfo);

#endif
