#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    
    rewind(fptr_image);
    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file open in binary read
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file open in binary read
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file open in binary write
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
//get file size of bmp
uint get_file_size(FILE *fptr)
{
    long current = ftell(fptr);        // get current position
    fseek(fptr, 0, SEEK_END);          // go to end of file
    long size = ftell(fptr);           // Get position = size
    fseek(fptr, current, SEEK_SET);    // Restore original pointer
    return size;
}
//read and validate
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //argv[2]->secret image
    //argv[3]->secret file
    //argv[4]->stego image or saved image that is given by user

    if(argv[2]==NULL || argv[3]==NULL)
    {
        printf("Error in given files,Enter valid file\n");
        return e_failure;
    }
     
    //save CLA to encInfo to use it in program

    encInfo->src_image_fname=argv[2];
    encInfo->secret_fname=argv[3];
    if(argv[4]!=NULL)
    {
        encInfo->stego_image_fname=argv[4];
    }
    else
    {
    encInfo->stego_image_fname="stego_img.bmp";
    }

    //check weather the extensions are correct
    
    char *dot=strrchr(encInfo->src_image_fname,'.');
    if(dot==NULL || strcmp(dot,".bmp")!=0)
    {
        printf("Error: Wrong file given \nSource image must have <.bmp> extension\n");
        return e_failure;
    }
    //check secret file extensions
    dot=strrchr(encInfo->secret_fname,'.');
    if(dot==NULL)
    {
        printf("Error: Wrong file given \nSecret file must have <.txt> or other extension");
        return e_failure;
    } 
    strcpy(encInfo->extn_secret_file, dot + 1);
   
    //check stego file extension
     dot=strrchr(encInfo->stego_image_fname,'.');
    if(dot==NULL || strcmp(dot,".bmp")!=0)
    {
        printf("Error: Wrong file given \nOutput image must have <.bmp> extension");
        return e_failure;
    }
    if(strcmp(encInfo->stego_image_fname,encInfo->src_image_fname)==0)
    {
        printf("Error:  Stego image file cannot be the same as the source image file");
        return e_failure;
    }
  return e_success;
}
//Do check capacity
Status check_capacity(EncodeInfo *encInfo)
{
    //get img size
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    //get secret file size
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    //take length of the secret file extension given
    char *dot=strrchr(encInfo->secret_fname,'.');

    int len_ext;
    if(dot!=NULL)
    {
        len_ext=strlen(dot+1);
    }
    else
    {
        len_ext=0;
    }
    //Check total bytes needed to hide the secret data
    //total bytes = size of magicstring(#*) + store extension length + length of extension characters(txt length(3)) + size of secretfile  + actual secret file data
    uint total_bytes = strlen(MAGIC_STRING) + sizeof(int) + len_ext  + sizeof(int) + encInfo->size_secret_file;


    //Convert total bytes to required image bytes needed for encoding ie: 1 byte need 8 bytes to store the data
    uint required_image_bytes = total_bytes * 8;

    //Check ewather the capacity of image is grater than required bytes

    if(required_image_bytes > encInfo->image_capacity)
    {
        printf("Error: Image capacity is not inough to do encoding !\n");
        return e_failure;
    }
    
    return e_success;
}

//Copy the headder of bmp file
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
  unsigned char header[54];//used to store the header values from file
// take the pointer to point towards the begin ning of file
  rewind(fptr_src_image);
//read header from source image
  if(fread(header,1,54,fptr_src_image)!=54)
  {
    printf("Error: Couldnot able to read the bmp file\n");
    return e_failure;
  }
//write header to destination file (stegno.bmp)
  if(fwrite(header,1,54,fptr_dest_image)!=54)
  {
    printf("Error: Couldnot able to write the bmp file\n");
    return e_failure;
  }
  return e_success;
 
}
//Encode byte to lsb for given data
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0;i<8;i++)
    {
        //clear the LSB
        image_buffer[i]=image_buffer[i]&~(1);
        //get values from secret data to lsb
        image_buffer[i]=image_buffer[i] | ((data>>i)&1);
    }
    return e_success;
}
//encoding data to image
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
 char img_buffer[8];
  for(int i=0;i<size;i++)
  {
    //read the data from source image file to do encoding of magic string
    if(fread(img_buffer,1,8,fptr_src_image)!=8)
    {
      printf("Error: Unable to read data from sourcee file\n");
      return e_failure;
    }
     //call encodebyte to lsb to encode 1 byte of data into 8 bytes of image
     encode_byte_to_lsb(data[i],img_buffer);

    //write the data into stego image file using the file pointer
     if(fwrite(img_buffer,1,8,fptr_stego_image)!=8)
     {
      printf("Error: Unable to write into the file");
      return e_failure;
     }
  }
  return e_success;
}
//Encode the magic string
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
 int len =strlen(magic_string);

 if(encode_data_to_image((char*)magic_string,len,encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
{
  printf("Error: failed to encode the magic string \n");
  return e_failure;
}
return e_success;
}

//encode secret file extension and its extension length
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
  int ext_len=strlen(file_extn);
   //first encode the extension length so that the decoder will know howmuch should he need to decode to find the extension
      if(encode_data_to_image((char*)&ext_len,sizeof(int),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
      {
        printf("Error : Unable to encod ethe secret file size \n");
        return e_failure;
      }
  //Second encode the secret file extention string to the stego image file(.txt)
      if(encode_data_to_image((char*)file_extn,ext_len,encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
      {
        printf("Error: unable to encode extension string to stego image \n");
        return e_failure;
      }
      return e_success;
}
//encode secret file size
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
   int size = (int)file_size;   // store size in 4 bytes
 if (encode_data_to_image((char *)&size,sizeof(int), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("ERROR: Failed to encode secret file size\n");
        return e_failure;
    }
    return e_success;
}

//Encode secret file data to stego image
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    rewind(encInfo->fptr_secret);//to move the pointer to starting of the seceret file
     //read byte by bte to character ch ,ie: 1 byte at a time to make it easy
     for(long i=0;i<encInfo->size_secret_file;i++)
     {
      if(fread(&ch,1,1,encInfo->fptr_secret)!=1)
      {
        printf("Error: Readin the data from seceret file failed\n");
        return e_failure;
      }

     //The copied characters to the image ie; 1 bte is taken on each loop and copy it to 8 image bytes
     if(encode_data_to_image(&ch,1,encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
     {
      printf("Error: Unable to encode the data of secret file to stego image \n");
      return e_failure;
     }
    }
     return e_success;
}

//copy remaining image data to the stego image
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
  int n;// we use int n since the return value of fgetc will be an integer
  while((n=fgetc(fptr_src))!=EOF)
  {
    if(fputc(n,fptr_dest)==EOF)
    {
      printf("Error: Unable to copy the remaining daat to stego image\n");
      return e_failure;
    }
  }
  return e_success;
}

//do encoding is done
Status do_encoding(EncodeInfo *encInfo)
{
   printf("Do Encoding of data !\n");
   //step1: open file
   if(open_files(encInfo)==e_failure)
   {
    printf("Error: Open file failed ! \n");
    return e_failure;
   }
   //step2:check capacity of the image file to be encoded
   if(check_capacity(encInfo)==e_failure)
   {
    printf("Error:Image capacity is not meeting requirments !\n");
    return e_failure;
   }
   //step3:Copy bmp headder of the file to stego image file
   if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
   {
    printf("Error:Failed to copy Headder !\n");
    return e_failure;
   }
   //step4:Encode Magic string here we give (#*)
   if(encode_magic_string(MAGIC_STRING, encInfo)==e_failure)
   {
     printf("Error:Magic string encoding failed !\n");
     return e_failure;
   }
   //step5:Encode Secret file extension
   if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_failure)
   {
    printf("Error:Secret file extension failed to encode !\n");
    return e_failure;
   }
   //step6:Encode secret file  size 
   if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_failure)
   {
     printf("Error:Secret file size failed to encode !\n");
    return e_failure;
   }
   //step7:encode secret file data
   if(encode_secret_file_data(encInfo)==e_failure)
   {
    printf("Error:Secret file data failed to encode !\n");
    return e_failure;
   }
   //step8:Copy remaining image data
   if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
   {
    printf("Error:Copy remaining data is failed !\n");
    return e_failure;
   }
   //step9:close all files
   fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);

    //step10: Encoding success
    return e_success;
}



