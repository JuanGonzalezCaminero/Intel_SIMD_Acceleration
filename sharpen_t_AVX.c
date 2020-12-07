#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "ctimer.h"

#include <immintrin.h>

//typedef double FLOAT;
typedef float FLOAT;

// Cycle Counter Code
//
// Can be replaced with ippGetCpuFreqMhz and ippGetCpuClocks
// when IPP core functions are available.
//
typedef unsigned int UINT32;
typedef unsigned long long int UINT64;
typedef unsigned char UINT8;

// PPM Edge Enhancement Code
//
/*
//FOR GCC
UINT8 header[22];
UINT8 R[76800];
UINT8 G[76800];
UINT8 B[76800];
UINT8 convR[76800];
UINT8 convG[76800];
UINT8 convB[76800];
__attribute__((aligned(32))) FLOAT R_float[76800+2*240];
__attribute__((aligned(32))) FLOAT G_float[76800+2*240];
__attribute__((aligned(32))) FLOAT B_float[76800+2*240];
__attribute__((aligned(32))) FLOAT out_float[76800];
*/

UINT8 header[22];
UINT8 R[76800];
UINT8 G[76800];
UINT8 B[76800];
UINT8 convR[76800];
UINT8 convG[76800];
UINT8 convB[76800];
__declspec(align(32)) FLOAT R_float[76800+2*240];
__declspec(align(32)) FLOAT G_float[76800+2*240];
__declspec(align(32)) FLOAT B_float[76800+2*240];
__declspec(align(32)) FLOAT out_float[76800];


#define K 4.0

#define ROWS 240
#define COLS 320

//KERNEl
FLOAT PSF[9] = {-K/8.0, -K/8.0, -K/8.0, -K/8.0, K+1.0, -K/8.0, -K/8.0, -K/8.0, -K/8.0};

int main(int argc, char *argv[])
{
    int fdin, fdout, bytesRead=0, bytesLeft, i, j, j_aux, z, vuelta;
    double elapsed0, ucpu0, scpu0;
    double elapsed1, ucpu1, scpu1;
    FLOAT temp, temp0, temp1, temp2, temp3;

    //INPUT VALIDATION
    if(argc < 2)
    {
       printf("Usage: sharpen file.ppm\n");
       exit(-1);
    }
    else
    {
        if((fdin = open(argv[1], O_RDONLY, 0644)) < 0)
        {
            printf("Error opening %s\n", argv[1]);
        }


        if((fdout = open("sharpen.ppm", (O_RDWR | O_CREAT), 0666)) < 0)
        {
            printf("Error opening %s\n", argv[1]);
        }
    }

    //READ IMAGE HEADER
    bytesLeft=21;
    do
    {
        bytesRead=read(fdin, (void *)header, bytesLeft);
        bytesLeft -= bytesRead;
    } while(bytesLeft > 0);

    header[21]='\0';

    // Read RGB data
    for(i=0; i<76800; i++)
    {
        read(fdin, (void *)&R[i], 1); convR[i]=R[i];
        read(fdin, (void *)&G[i], 1); convG[i]=G[i];
        read(fdin, (void *)&B[i], 1); convB[i]=B[i];
    }

    ////////////// New variables ///////////////////

    //The data register will contain in the first 3 positions the registers corresponding to the first row of 
    //10 items and so on
    __m256 data[12], accumulators, k_sides, k_center, register_255, register_0;

    k_sides = _mm256_set1_ps(PSF[0]);
    k_center = _mm256_set1_ps(PSF[4]);

    register_255 = _mm256_set1_ps(255);
    register_0 = _mm256_set1_ps(0);


    // Start of convolution time stamp
///////////////////////////////////////////////////////////////////////////////////////////

    ctimer_(&elapsed0, &ucpu0, &scpu0);

    //Copy and cast to float each colour, store in columns in the float versions in order 
    //to be able to load with _mm256_load_ps
    for(i=0; i<ROWS; i++)
    {
        for(j=0; j<COLS; j++)
        { 
            //j+1 in order to skip first and last columns
            R_float[i*COLS + j + 1] = (FLOAT)R[i*COLS + j];   
            G_float[i*COLS + j + 1] = (FLOAT)G[i*COLS + j];   
            B_float[i*COLS + j + 1] = (FLOAT)B[i*COLS + j];
        }
    }


    for (vuelta=1;vuelta<500;vuelta++)
    {
        for(j=0; j<COLS; j+=8)
        {
        
            i=1;
            //Register preload:

            data[6]=_mm256_load_ps(&R_float[(i-1)*COLS + j]);
            data[7]=_mm256_load_ps(&R_float[(i-1)*COLS + j+1]);
            data[8]=_mm256_load_ps(&R_float[(i-1)*COLS + j+2]);

            data[9]=_mm256_load_ps(&R_float[(i)*COLS + j]);
            data[10]=_mm256_load_ps(&R_float[(i)*COLS + j+1]);
            data[11]=_mm256_load_ps(&R_float[(i)*COLS + j+2]);

            for(i=1; i<ROWS-1; i+=2)
            {   
                //Initialise accumulators
                accumulators = _mm256_set1_ps(0);

                //Move data registers to remove first row and make second and third 
                //represent first and second
                data[0]=data[6];
                data[1]=data[7];
                data[2]=data[8];

                data[3]=data[9];
                data[4]=data[10];
                data[5]=data[11];

                //Load data for the last rows
                data[6]=_mm256_load_ps(&R_float[(i+1)*COLS + j]);
                data[7]=_mm256_load_ps(&R_float[(i+1)*COLS + j+1]);
                data[8]=_mm256_load_ps(&R_float[(i+1)*COLS + j+2]);

                data[9]=_mm256_load_ps(&R_float[(i+2)*COLS + j]);
                data[10]=_mm256_load_ps(&R_float[(i+2)*COLS + j+1]);
                data[11]=_mm256_load_ps(&R_float[(i+2)*COLS + j+2]);
                
                //FIRST ROW
                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[0], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[1], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[2], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[3], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[4], k_center));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[5], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[6], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[7], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[8], k_sides));
                

                accumulators = _mm256_min_ps(accumulators, register_255);
                accumulators = _mm256_max_ps(accumulators, register_0);

                _mm256_store_ps(&out_float[i*COLS + j], accumulators);

                //SECOND ROW
                accumulators = _mm256_set1_ps(0);
                
                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[3], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[4], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[5], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[6], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[7], k_center));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[8], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[9], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[10], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[11], k_sides));
                

                accumulators = _mm256_min_ps(accumulators, register_255);
                accumulators = _mm256_max_ps(accumulators, register_0);

                _mm256_store_ps(&out_float[(i+1)*COLS + j], accumulators);
                
            }
        }
        
        /*
        j_aux = j;

        //Last rows
        for(i=1; i<ROWS-1; i++)
        {
            // Skip first and last column, no neighbors to convolve with
            for(j=j_aux; j<COLS-1; j++)
            {
                temp=0;
                temp += (PSF[0] * (FLOAT)R[((i-1)*320)+j-1]);
                temp += (PSF[1] * (FLOAT)R[((i-1)*320)+j]);
                temp += (PSF[2] * (FLOAT)R[((i-1)*320)+j+1]);
                temp += (PSF[3] * (FLOAT)R[((i)*320)+j-1]);
                temp += (PSF[4] * (FLOAT)R[((i)*320)+j]);
                temp += (PSF[5] * (FLOAT)R[((i)*320)+j+1]);
                temp += (PSF[6] * (FLOAT)R[((i+1)*320)+j-1]);
                temp += (PSF[7] * (FLOAT)R[((i+1)*320)+j]);
                temp += (PSF[8] * (FLOAT)R[((i+1)*320)+j+1]);  

                if(temp<0.0) temp=0.0;
                if(temp>255.0) temp=255.0;
                out_float[(i*COLS)+j]=temp;
            }
        }
        */
        
        
        for(i=1; i<ROWS-1; i++)
        {
            for(j=1; j<COLS-1; j++)
            {
                convR[(i*COLS)+j] = (UINT8)out_float[(i*COLS)+j];
            }
        }
        


        //GREEN
        for(j=0; j<COLS; j+=8)
        {
        
            i=1;
            //Register preload:

            data[6]=_mm256_load_ps(&G_float[(i-1)*COLS + j]);
            data[7]=_mm256_load_ps(&G_float[(i-1)*COLS + j+1]);
            data[8]=_mm256_load_ps(&G_float[(i-1)*COLS + j+2]);

            data[9]=_mm256_load_ps(&G_float[(i)*COLS + j]);
            data[10]=_mm256_load_ps(&G_float[(i)*COLS + j+1]);
            data[11]=_mm256_load_ps(&G_float[(i)*COLS + j+2]);

            for(i=1; i<ROWS-1; i+=2)
            {   
                //Initialise accumulators
                accumulators = _mm256_set1_ps(0);

                //Move data registers to remove first row and make second and third 
                //represent first and second
                data[0]=data[6];
                data[1]=data[7];
                data[2]=data[8];

                data[3]=data[9];
                data[4]=data[10];
                data[5]=data[11];

                //Load data for the last rows
                data[6]=_mm256_load_ps(&G_float[(i+1)*COLS + j]);
                data[7]=_mm256_load_ps(&G_float[(i+1)*COLS + j+1]);
                data[8]=_mm256_load_ps(&G_float[(i+1)*COLS + j+2]);

                data[9]=_mm256_load_ps(&G_float[(i+2)*COLS + j]);
                data[10]=_mm256_load_ps(&G_float[(i+2)*COLS + j+1]);
                data[11]=_mm256_load_ps(&G_float[(i+2)*COLS + j+2]);
                
                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[0], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[1], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[2], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[3], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[4], k_center));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[5], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[6], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[7], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[8], k_sides));
                

                accumulators = _mm256_min_ps(accumulators, register_255);
                accumulators = _mm256_max_ps(accumulators, register_0);

                _mm256_store_ps(&out_float[i*COLS + j], accumulators);

                                
                accumulators = _mm256_set1_ps(0);
                
                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[3], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[4], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[5], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[6], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[7], k_center));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[8], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[9], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[10], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[11], k_sides));
                

                accumulators = _mm256_min_ps(accumulators, register_255);
                accumulators = _mm256_max_ps(accumulators, register_0);

                _mm256_store_ps(&out_float[(i+1)*COLS + j], accumulators);
                
            }
        }
        
        /*
        j_aux = j;

        //Last rows
        for(i=1; i<ROWS-1; i++)
        {
            // Skip first and last column, no neighbors to convolve with
            for(j=j_aux; j<COLS-1; j++)
            {
                temp=0;
                temp += (PSF[0] * (FLOAT)R[((i-1)*320)+j-1]);
                temp += (PSF[1] * (FLOAT)R[((i-1)*320)+j]);
                temp += (PSF[2] * (FLOAT)R[((i-1)*320)+j+1]);
                temp += (PSF[3] * (FLOAT)R[((i)*320)+j-1]);
                temp += (PSF[4] * (FLOAT)R[((i)*320)+j]);
                temp += (PSF[5] * (FLOAT)R[((i)*320)+j+1]);
                temp += (PSF[6] * (FLOAT)R[((i+1)*320)+j-1]);
                temp += (PSF[7] * (FLOAT)R[((i+1)*320)+j]);
                temp += (PSF[8] * (FLOAT)R[((i+1)*320)+j+1]);  

                if(temp<0.0) temp=0.0;
                if(temp>255.0) temp=255.0;
                out_float[(i*COLS)+j]=temp;
            }
        }
        */
        
        
        for(i=1; i<ROWS-1; i++)
        {
            for(j=1; j<COLS-1; j++)
            {
                convG[(i*COLS)+j] = (UINT8)out_float[(i*COLS)+j];
            }
        }

        //BLUE
        for(j=0; j<COLS; j+=8)
        {
        
            i=1;
            //Register preload:

            data[6]=_mm256_load_ps(&B_float[(i-1)*COLS + j]);
            data[7]=_mm256_load_ps(&B_float[(i-1)*COLS + j+1]);
            data[8]=_mm256_load_ps(&B_float[(i-1)*COLS + j+2]);

            data[9]=_mm256_load_ps(&B_float[(i)*COLS + j]);
            data[10]=_mm256_load_ps(&B_float[(i)*COLS + j+1]);
            data[11]=_mm256_load_ps(&B_float[(i)*COLS + j+2]);

            for(i=1; i<ROWS-1; i+=2)
            {   
                //Initialise accumulators
                accumulators = _mm256_set1_ps(0);

                //Move data registers to remove first row and make second and third 
                //represent first and second
                data[0]=data[6];
                data[1]=data[7];
                data[2]=data[8];

                data[3]=data[9];
                data[4]=data[10];
                data[5]=data[11];

                //Load data for the last rows
                data[6]=_mm256_load_ps(&B_float[(i+1)*COLS + j]);
                data[7]=_mm256_load_ps(&B_float[(i+1)*COLS + j+1]);
                data[8]=_mm256_load_ps(&B_float[(i+1)*COLS + j+2]);

                data[9]=_mm256_load_ps(&B_float[(i+2)*COLS + j]);
                data[10]=_mm256_load_ps(&B_float[(i+2)*COLS + j+1]);
                data[11]=_mm256_load_ps(&B_float[(i+2)*COLS + j+2]);
                
                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[0], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[1], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[2], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[3], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[4], k_center));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[5], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[6], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[7], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[8], k_sides));
                

                accumulators = _mm256_min_ps(accumulators, register_255);
                accumulators = _mm256_max_ps(accumulators, register_0);

                _mm256_store_ps(&out_float[i*COLS + j], accumulators);

                
                accumulators = _mm256_set1_ps(0);
                
                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[3], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[4], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[5], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[6], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[7], k_center));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[8], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[9], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[10], k_sides));

                accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(data[11], k_sides));
                

                accumulators = _mm256_min_ps(accumulators, register_255);
                accumulators = _mm256_max_ps(accumulators, register_0);

                _mm256_store_ps(&out_float[(i+1)*COLS + j], accumulators);
                
            }
        }
        
        /*
        j_aux = j;

        //Last rows
        for(i=1; i<ROWS-1; i++)
        {
            // Skip first and last column, no neighbors to convolve with
            for(j=j_aux; j<COLS-1; j++)
            {
                temp=0;
                temp += (PSF[0] * (FLOAT)R[((i-1)*320)+j-1]);
                temp += (PSF[1] * (FLOAT)R[((i-1)*320)+j]);
                temp += (PSF[2] * (FLOAT)R[((i-1)*320)+j+1]);
                temp += (PSF[3] * (FLOAT)R[((i)*320)+j-1]);
                temp += (PSF[4] * (FLOAT)R[((i)*320)+j]);
                temp += (PSF[5] * (FLOAT)R[((i)*320)+j+1]);
                temp += (PSF[6] * (FLOAT)R[((i+1)*320)+j-1]);
                temp += (PSF[7] * (FLOAT)R[((i+1)*320)+j]);
                temp += (PSF[8] * (FLOAT)R[((i+1)*320)+j+1]);  

                if(temp<0.0) temp=0.0;
                if(temp>255.0) temp=255.0;
                out_float[(i*COLS)+j]=temp;
            }
        }
        */
        
        
        for(i=1; i<ROWS-1; i++)
        {
            for(j=1; j<COLS-1; j++)
            {
                convB[(i*COLS)+j] = (UINT8)out_float[(i*COLS)+j];
            }
        }
    }

///////////////////////////////////////////////////////////////////////////////////////////
    // End of convolution time stamp
    ctimer_(&elapsed1, &ucpu1, &scpu1);

    printf("Tiempo: %fs (real) %fs (cpu) %fs (sys)\n", 
                 elapsed1-elapsed0, ucpu1-ucpu0, scpu1-scpu0);

    write(fdout, (void *)header, 21);

    // Write RGB data
    for(i=0; i<76800; i++)
    {
        write(fdout, (void *)&convR[i], 1);
        write(fdout, (void *)&convG[i], 1);
        write(fdout, (void *)&convB[i], 1);
    }


    close(fdin);
    close(fdout);
 
}
