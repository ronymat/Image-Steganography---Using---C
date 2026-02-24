#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

#define BMP_HEADER_SIZE 54  

// Read and validate decode arguments 
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Check whether stego image name is provided
    if (argv[2] == NULL)
    {
        printf("ERROR: Stego image not provided\n");
        return e_failure;
    }
    // Store stego image filename
    decInfo->stego_image_fname = argv[2];

    // Store output file base name if provided
    if (argv[3] != NULL)
    {
        // Copy output filename safely
        strncpy(decInfo->output_fname, argv[3],sizeof(decInfo->output_fname) - 1);
        decInfo->output_fname[sizeof(decInfo->output_fname) - 1] = '\0';
    }
    else
    {
        strcpy(decInfo->output_fname, "output");
    }

    // Check .bmp extension in stego image
    char *dot = strrchr(decInfo->stego_image_fname, '.');
    if (dot == NULL || strcmp(dot, ".bmp") != 0)
    {
        printf("ERROR: Stego image must be a .bmp file\n");
        return e_failure;
    }

    return e_success;
}

// Open stego image file 
Status open_files_decode(DecodeInfo *decInfo)
{
    //Opens the stego image file in read-binary mode.
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
    if (decInfo->fptr_stego_image == NULL)
    {
        printf("ERROR: Unable to open %s\n", decInfo->stego_image_fname);
        return e_failure;
    }
    return e_success;
}

// Decode one byte from 8 LSBs of image
char decode_byte_from_lsb(char *image_buffer)
{
    char data = 0;
    // Extract 1 LSB from each byte and rebuild character
    for (int i = 0; i < 8; i++)
    {
        data |= (image_buffer[i] & 1) << i;
    }
    return data;
}
// Decode data from image 'size' bytes of hidden data
Status decode_data_from_image(char *buffer, int size, FILE *fptr_stego)
{
    // Buffer to read 8 bytes per character
    char image_buffer[8];

    for (int i = 0; i < size; i++)
    {
        // Read 8 bytes from image
        if (fread(image_buffer, 1, 8, fptr_stego) != 8)
        {
            return e_failure;
        }
        // Decode one byte from LSBs
        buffer[i] = decode_byte_from_lsb(image_buffer);
    }
    return e_success;
}

// Main decoding function 
Status do_decoding(DecodeInfo *decInfo)
{
    // Step 1: Open stego image 
    if (open_files_decode(decInfo) == e_failure)
    {
        printf("Error: Unable to open the file\n");
        return e_failure;
    }

    // Step 2: Skip BMP header size 54
   if (fseek(decInfo->fptr_stego_image, BMP_HEADER_SIZE, SEEK_SET) != 0)
{
    printf("Error: Failed to skip BMP header to position\n");
    return e_failure;
}

    // Step 3: Decode magic string 
    int magic_len = strlen(MAGIC_STRING);
    char magic[magic_len + 1];

    if (decode_data_from_image(magic, magic_len,decInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    magic[magic_len] = '\0';// Null-terminate magic string

    if (strcmp(magic, MAGIC_STRING) != 0)
    {
        printf("Error: Magic string mismatch occoured\n");
        return e_failure;
    }

    // Step 4: Decode extension length 
    if (decode_data_from_image((char *)&decInfo->extn_size,sizeof(int),decInfo->fptr_stego_image) == e_failure)
    {
        printf("Error: Failed to decode extension length\n");
        return e_failure;
    }
    // Validate extension size
    if (decInfo->extn_size <= 0 || decInfo->extn_size >= sizeof(decInfo->extension))
    {
        printf("Error: Invalid extension size\n");
        return e_failure;
    }

    // Step 5: Decode extension string
    if (decode_data_from_image(decInfo->extension, decInfo->extn_size,decInfo->fptr_stego_image) == e_failure)
    {
        printf("Error: Faileed to edcode extension string\n");
        return e_failure;
    }
     // Null-terminate extension
    decInfo->extension[decInfo->extn_size] = '\0';

        // Step 6: Create output file name safely 
        char final_output[100];
        snprintf(final_output, sizeof(final_output),"%s.%s", decInfo->output_fname,decInfo->extension);//snprintf prevent overflow
        // Open output file
        decInfo->fptr_output = fopen(final_output, "wb");
        if (decInfo->fptr_output == NULL)
        {
            printf("Error: Unable to create output file\n");
            return e_failure;
        }

    // Step 7: Decode secret file size 
    if (decode_data_from_image((char *)&decInfo->secret_file_size,sizeof(int),decInfo->fptr_stego_image) == e_failure)
    {
        printf("Error: Unable to decode file size\n");
        return e_failure;
    }
    // Validate secret file size
    if (decInfo->secret_file_size <= 0)
    {
        printf("Error: Invalid secret file size\n");
        return e_failure;
    }
    printf("Decoded secret file size = %d\n", decInfo->secret_file_size);

    // Step 8: Decode secret data 
    char ch;
    for (long i = 0; i < decInfo->secret_file_size; i++)
    {
        if (decode_data_from_image(&ch, 1, decInfo->fptr_stego_image) == e_failure)
        {
            printf("Error:Failed to decode the data\n");
            return e_failure;
        }
        // Write decoded byte to output file
        fwrite(&ch, 1, 1, decInfo->fptr_output);
    }
  //close all files
    fclose(decInfo->fptr_output);
    fclose(decInfo->fptr_stego_image);

    return e_success;
}
