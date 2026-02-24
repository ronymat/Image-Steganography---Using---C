#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;// variable name of the EncodeInfo datatype
    DecodeInfo decInfo;// variable name of decode info datatype
    //Checking weather the CLA's are correct or not
    if(argc<3)
    {
        printf("Error !\n");
        printf("For Encoding use :\n %s -e  source_image_file_name(.bmp) secret_text_file_name [encodec_file_name.bmp]",argv[0]);
        printf("\nFor Decoding use :\n %s -d  encoded_file_name.bmp [decode_file.txt] \n",argv[0]);
        return 1;
    }
    // Determine whether encoding or decoding operation is selected
    int ret=check_operation_type(argv);
    {
        if(ret==e_encode)//do encoding things if it is true
        {
            printf("Selected Encoding\n");
            if(read_and_validate_encode_args(argv, &encInfo) == e_success)
            {
                printf("Arguments validations success\n");
                //calling do encoding function
                if(do_encoding(&encInfo) == e_success)
                {
                    printf("Encoding complete successfully...!");
                }
                else
                    printf("Encoding failed...!");
            }
            else
            {
                printf("Arguments validations failed...!"); 
            }           
        }
        else if(ret==e_decode)//Do decoding things if it is true
        {
            printf("Selected Decoding\n");
             if(read_and_validate_decode_args(argv, &decInfo) == e_success)
            {
                printf("Arguments validations success\n");
                //calling do decoding function
                if(do_decoding(&decInfo) == e_success)
                {
                    printf("Decoding complete successfully...!");
                }
                else
                    printf("Decoding failed...!");
            }
            else
            {
                printf("Arguments validations failed...!"); 
            }           
        }
        else
        {
            printf("Invalie input\n");
        }
    }
    return 0;
}
  //check weather we are doing encoding or decoding
   OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e")==0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1],"-d")==0)
    {
        return e_decode;
    }
    else
    {
        return  e_unsupported;
    }
       
}


