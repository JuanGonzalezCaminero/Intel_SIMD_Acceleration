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
__attribute__((aligned(1))) UINT8 header[22];
__attribute__((aligned(1))) UINT8 R[76800];
__attribute__((aligned(1))) UINT8 G[76800];
__attribute__((aligned(1))) UINT8 B[76800];
__attribute__((aligned(1))) UINT8 convR[76800];
__attribute__((aligned(1))) UINT8 convG[76800];
__attribute__((aligned(1))) UINT8 convB[76800];

#define K 4.0

#define ROWS 240
#define COLS 320

//THIS IS THE KERNEL USED
FLOAT PSF[9] = {-K/8.0, -K/8.0, -K/8.0, -K/8.0, K+1.0, -K/8.0, -K/8.0, -K/8.0, -K/8.0};

int main(int argc, char *argv[])
{
    int fdin, fdout, bytesRead=0, bytesLeft, i, j, z, vuelta;
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

    //printf("header = %s\n", header); 

    // Read RGB data
    for(i=0; i<76800; i++)
    {
        read(fdin, (void *)&R[i], 1); convR[i]=R[i];
        read(fdin, (void *)&G[i], 1); convG[i]=G[i];
        read(fdin, (void *)&B[i], 1); convB[i]=B[i];
    }

    ////////////// New variables ///////////////////
    //Number of iterations to unroll when processing each color.
    int unroll_iterations = 8;
    //Accumulators for each pixel treated in an unrolled iteration
    float* temp_accumulators;
    temp_accumulators = malloc(unroll_iterations * sizeof(float));
    float* temp_accumulators_base = temp_accumulators;
    //Index for the pixel within an unrolled iteration
    int pixel;

    //Float registers
    //__m256 row1_1, row1_2, row1_3, row2_1, row2_2, row2_3, row3_1, row3_2, row3_3, kernel_tmp, accumulators, data;
    //The data register will contain in the first 3 positions the registers corresponding to the first column of 
    //6 items and so on
    __m128 data[9], kernel_tmp, accumulators, k_sides, k_center, register_255, register_0;
    //floats for reading from the accumulators register and for loading and casting to float each row
    float accumulators_ram[4];

    k_sides = _mm_set_ps1(PSF[0]);
    k_center = _mm_set_ps1(PSF[4]);

    register_255 = _mm_set_ps1(255);
    register_0 = _mm_set_ps1(0);



    // Start of convolution time stamp
///////////////////////////////////////////////////////////////////////////////////////////

    ctimer_(&elapsed0, &ucpu0, &scpu0);
    for (vuelta=1;vuelta<500;vuelta++)
    {
        for(i=1; i+4<ROWS-1; i+=4)
        {
            j=1;
            //Register preload:
            data[3]=_mm_set_ps((FLOAT)R[((i+2)*COLS)+j-1], (FLOAT)R[((i+1)*COLS)+j-1],
                                (FLOAT)R[((i)*COLS)+j-1], (FLOAT)R[((i-1)*COLS)+j-1]);
            data[4]=_mm_set_ps((FLOAT)R[((i+3)*COLS)+j-1], (FLOAT)R[((i+2)*COLS)+j-1],
                               (FLOAT)R[((i+1)*COLS)+j-1], (FLOAT)R[((i)*COLS)+j-1]);
            data[5]=_mm_set_ps((FLOAT)R[((i+4)*COLS)+j-1], (FLOAT)R[((i+3)*COLS)+j-1],
                               (FLOAT)R[((i+2)*COLS)+j-1], (FLOAT)R[((i+1)*COLS)+j-1]);

            data[6]=_mm_set_ps((FLOAT)R[((i+2)*COLS)+j], (FLOAT)R[((i+1)*COLS)+j],
                                (FLOAT)R[((i)*COLS)+j], (FLOAT)R[((i-1)*COLS)+j]);
            data[7]=_mm_set_ps((FLOAT)R[((i+3)*COLS)+j], (FLOAT)R[((i+2)*COLS)+j],
                               (FLOAT)R[((i+1)*COLS)+j], (FLOAT)R[((i)*COLS)+j]);
            data[8]=_mm_set_ps((FLOAT)R[((i+4)*COLS)+j], (FLOAT)R[((i+3)*COLS)+j],
                               (FLOAT)R[((i+2)*COLS)+j], (FLOAT)R[((i+1)*COLS)+j]);

            // Skip first and last column, no neighbors to convolve with
            for(j=1; j<COLS-1; j++)
            {   

                //Initialise accumulators
                accumulators = _mm_set_ps1(0);

                //Move data registers to remove first column and make second and third 
                //represent first and second
                data[0]=data[3];
                data[1]=data[4];
                data[2]=data[5];

                data[3]=data[6];
                data[4]=data[7];
                data[5]=data[8];

                //Load data for the last column
                data[6]=_mm_set_ps((FLOAT)R[((i+2)*COLS)+j+1], (FLOAT)R[((i+1)*COLS)+j+1],
                                    (FLOAT)R[((i)*COLS)+j+1], (FLOAT)R[((i-1)*COLS)+j+1]);
                data[7]=_mm_set_ps((FLOAT)R[((i+3)*COLS)+j+1], (FLOAT)R[((i+2)*COLS)+j+1],
                                   (FLOAT)R[((i+1)*COLS)+j+1], (FLOAT)R[((i)*COLS)+j+1]);
                data[8]=_mm_set_ps((FLOAT)R[((i+4)*COLS)+j+1], (FLOAT)R[((i+3)*COLS)+j+1],
                                   (FLOAT)R[((i+2)*COLS)+j+1], (FLOAT)R[((i+1)*COLS)+j+1]);
                
                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[0], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[1], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[2], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[3], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[4], k_center));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[5], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[6], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[7], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[8], k_sides));
                

                accumulators = _mm_min_ps(accumulators, register_255);
                accumulators = _mm_max_ps(accumulators, register_0);

                _mm_store_ps(accumulators_ram, accumulators);

                convR[(i*COLS)+j]=(UINT8)accumulators_ram[0];
                convR[((i+1)*COLS)+j]=(UINT8)accumulators_ram[1];
                convR[((i+2)*COLS)+j]=(UINT8)accumulators_ram[2];
                convR[((i+3)*COLS)+j]=(UINT8)accumulators_ram[3];

                /*
                convR[(i*COLS)+j]=(UINT8)accumulators[0];
                convR[((i+1)*COLS)+j]=(UINT8)accumulators[1];
                convR[((i+2)*COLS)+j]=(UINT8)accumulators[2];
                convR[((i+3)*COLS)+j]=(UINT8)accumulators[3];
                */
            }
        }

        /*
        //Last rows but with SSE
        j=1;
        //Register preload:
        data[3]=_mm_set_ps((FLOAT)R[((i+2)*COLS)+j-1], (FLOAT)R[((i+1)*COLS)+j-1],
                            (FLOAT)R[((i)*COLS)+j-1], (FLOAT)R[((i-1)*COLS)+j-1]);
        data[4]=_mm_set_ps(0, (FLOAT)R[((i+2)*COLS)+j-1],
                           (FLOAT)R[((i+1)*COLS)+j-1], (FLOAT)R[((i)*COLS)+j-1]);
        data[5]=_mm_set_ps(0, 0,
                           (FLOAT)R[((i+2)*COLS)+j-1], (FLOAT)R[((i+1)*COLS)+j-1]);

        data[6]=_mm_set_ps((FLOAT)R[((i+2)*COLS)+j], (FLOAT)R[((i+1)*COLS)+j],
                            (FLOAT)R[((i)*COLS)+j], (FLOAT)R[((i-1)*COLS)+j]);
        data[7]=_mm_set_ps(0, (FLOAT)R[((i+2)*COLS)+j],
                           (FLOAT)R[((i+1)*COLS)+j], (FLOAT)R[((i)*COLS)+j]);
        data[8]=_mm_set_ps(0, 0,
                           (FLOAT)R[((i+2)*COLS)+j], (FLOAT)R[((i+1)*COLS)+j]);

        // Skip first and last column, no neighbors to convolve with
        for(j=1; j<COLS-1; j++)
        {   

            //Initialise accumulators
            accumulators = _mm_set_ps1(0);

            //Move data registers to remove first column and make second and third 
            //represent first and second
            data[0]=data[3];
            data[1]=data[4];
            data[2]=data[5];

            data[3]=data[6];
            data[4]=data[7];
            data[5]=data[8];

            //Load data for the last column
            data[6]=_mm_set_ps((FLOAT)R[((i+2)*COLS)+j+1], (FLOAT)R[((i+1)*COLS)+j+1],
                                (FLOAT)R[((i)*COLS)+j+1], (FLOAT)R[((i-1)*COLS)+j+1]);
            data[7]=_mm_set_ps(0, (FLOAT)R[((i+2)*COLS)+j+1],
                               (FLOAT)R[((i+1)*COLS)+j+1], (FLOAT)R[((i)*COLS)+j+1]);
            data[8]=_mm_set_ps(0, 0,
                               (FLOAT)R[((i+2)*COLS)+j+1], (FLOAT)R[((i+1)*COLS)+j+1]);
            
            accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[0], k_sides));

            accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[1], k_sides));

            accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[2], k_sides));

            accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[3], k_sides));

            accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[4], k_center));

            accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[5], k_sides));

            accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[6], k_sides));

            accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[7], k_sides));

            accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[8], k_sides));
            

            accumulators = _mm_min_ps(accumulators, register_255);
            accumulators = _mm_max_ps(accumulators, register_0);

            _mm_store_ps(accumulators_ram, accumulators);

            convR[(i*COLS)+j]=(UINT8)accumulators_ram[0];
            convR[((i+1)*COLS)+j]=(UINT8)accumulators_ram[1];
        }
        */

        
        //Last rows
        for(i; i<ROWS-1; i++)
        {
            // Skip first and last column, no neighbors to convolve with
            for(j=1; j<COLS-1; j++)
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
                convR[(i*320)+j]=(UINT8)temp;  
            }
        }
        

        //GREEN
        for(i=1; i+4<ROWS-1; i+=4)
        {
            j=1;
            //Register preload:
            data[3]=_mm_set_ps((FLOAT)G[((i+2)*COLS)+j-1], (FLOAT)G[((i+1)*COLS)+j-1],
                                (FLOAT)G[((i)*COLS)+j-1], (FLOAT)G[((i-1)*COLS)+j-1]);
            data[4]=_mm_set_ps((FLOAT)G[((i+3)*COLS)+j-1], (FLOAT)G[((i+2)*COLS)+j-1],
                               (FLOAT)G[((i+1)*COLS)+j-1], (FLOAT)G[((i)*COLS)+j-1]);
            data[5]=_mm_set_ps((FLOAT)G[((i+4)*COLS)+j-1], (FLOAT)G[((i+3)*COLS)+j-1],
                               (FLOAT)G[((i+2)*COLS)+j-1], (FLOAT)G[((i+1)*COLS)+j-1]);

            data[6]=_mm_set_ps((FLOAT)G[((i+2)*COLS)+j], (FLOAT)G[((i+1)*COLS)+j],
                                (FLOAT)G[((i)*COLS)+j], (FLOAT)G[((i-1)*COLS)+j]);
            data[7]=_mm_set_ps((FLOAT)G[((i+3)*COLS)+j], (FLOAT)G[((i+2)*COLS)+j],
                               (FLOAT)G[((i+1)*COLS)+j], (FLOAT)G[((i)*COLS)+j]);
            data[8]=_mm_set_ps((FLOAT)G[((i+4)*COLS)+j], (FLOAT)G[((i+3)*COLS)+j],
                               (FLOAT)G[((i+2)*COLS)+j], (FLOAT)G[((i+1)*COLS)+j]);

            // Skip first and last column, no neighbors to convolve with
            for(j=1; j<COLS-1; j++)
            {   

                //Initialise accumulators
                accumulators = _mm_set_ps1(0);

                //Move data registers to remove first column and make second and third 
                //represent first and second
                data[0]=data[3];
                data[1]=data[4];
                data[2]=data[5];

                data[3]=data[6];
                data[4]=data[7];
                data[5]=data[8];

                //Load data for the last column
                data[6]=_mm_set_ps((FLOAT)G[((i+2)*COLS)+j+1], (FLOAT)G[((i+1)*COLS)+j+1],
                                    (FLOAT)G[((i)*COLS)+j+1], (FLOAT)G[((i-1)*COLS)+j+1]);
                data[7]=_mm_set_ps((FLOAT)G[((i+3)*COLS)+j+1], (FLOAT)G[((i+2)*COLS)+j+1],
                                   (FLOAT)G[((i+1)*COLS)+j+1], (FLOAT)G[((i)*COLS)+j+1]);
                data[8]=_mm_set_ps((FLOAT)G[((i+4)*COLS)+j+1], (FLOAT)G[((i+3)*COLS)+j+1],
                                   (FLOAT)G[((i+2)*COLS)+j+1], (FLOAT)G[((i+1)*COLS)+j+1]);
                
                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[0], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[1], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[2], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[3], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[4], k_center));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[5], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[6], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[7], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[8], k_sides));
                

                accumulators = _mm_min_ps(accumulators, register_255);
                accumulators = _mm_max_ps(accumulators, register_0);

                _mm_store_ps(accumulators_ram, accumulators);

                convG[(i*COLS)+j]=(UINT8)accumulators_ram[0];
                convG[((i+1)*COLS)+j]=(UINT8)accumulators_ram[1];
                convG[((i+2)*COLS)+j]=(UINT8)accumulators_ram[2];
                convG[((i+3)*COLS)+j]=(UINT8)accumulators_ram[3];
            }
        }

        //Last rows
        for(i; i<ROWS-1; i++)
        {
            // Skip first and last column, no neighbors to convolve with
            for(j=1; j<COLS-1; j++)
            {
                temp=0;
                temp += (PSF[0] * (FLOAT)G[((i-1)*320)+j-1]);
                temp += (PSF[1] * (FLOAT)G[((i-1)*320)+j]);
                temp += (PSF[2] * (FLOAT)G[((i-1)*320)+j+1]);
                temp += (PSF[3] * (FLOAT)G[((i)*320)+j-1]);
                temp += (PSF[4] * (FLOAT)G[((i)*320)+j]);
                temp += (PSF[5] * (FLOAT)G[((i)*320)+j+1]);
                temp += (PSF[6] * (FLOAT)G[((i+1)*320)+j-1]);
                temp += (PSF[7] * (FLOAT)G[((i+1)*320)+j]);
                temp += (PSF[8] * (FLOAT)G[((i+1)*320)+j+1]);  

                if(temp<0.0) temp=0.0;
                if(temp>255.0) temp=255.0;
                convG[(i*320)+j]=(UINT8)temp;  
            }
        }

        //BLUE
        for(i=1; i+4<ROWS-1; i+=4)
        {
            j=1;
            //Register preload:
            data[3]=_mm_set_ps((FLOAT)B[((i+2)*COLS)+j-1], (FLOAT)B[((i+1)*COLS)+j-1],
                                (FLOAT)B[((i)*COLS)+j-1], (FLOAT)B[((i-1)*COLS)+j-1]);
            data[4]=_mm_set_ps((FLOAT)B[((i+3)*COLS)+j-1], (FLOAT)B[((i+2)*COLS)+j-1],
                               (FLOAT)B[((i+1)*COLS)+j-1], (FLOAT)B[((i)*COLS)+j-1]);
            data[5]=_mm_set_ps((FLOAT)B[((i+4)*COLS)+j-1], (FLOAT)B[((i+3)*COLS)+j-1],
                               (FLOAT)B[((i+2)*COLS)+j-1], (FLOAT)B[((i+1)*COLS)+j-1]);

            data[6]=_mm_set_ps((FLOAT)B[((i+2)*COLS)+j], (FLOAT)B[((i+1)*COLS)+j],
                                (FLOAT)B[((i)*COLS)+j], (FLOAT)B[((i-1)*COLS)+j]);
            data[7]=_mm_set_ps((FLOAT)B[((i+3)*COLS)+j], (FLOAT)B[((i+2)*COLS)+j],
                               (FLOAT)B[((i+1)*COLS)+j], (FLOAT)B[((i)*COLS)+j]);
            data[8]=_mm_set_ps((FLOAT)B[((i+4)*COLS)+j], (FLOAT)B[((i+3)*COLS)+j],
                               (FLOAT)B[((i+2)*COLS)+j], (FLOAT)B[((i+1)*COLS)+j]);

            // Skip first and last column, no neighbors to convolve with
            for(j=1; j<COLS-1; j++)
            {   

                //Initialise accumulators
                accumulators = _mm_set_ps1(0);

                //Move data registers to remove first column and make second and third 
                //represent first and second
                data[0]=data[3];
                data[1]=data[4];
                data[2]=data[5];

                data[3]=data[6];
                data[4]=data[7];
                data[5]=data[8];

                //Load data for the last column
                data[6]=_mm_set_ps((FLOAT)B[((i+2)*COLS)+j+1], (FLOAT)B[((i+1)*COLS)+j+1],
                                    (FLOAT)B[((i)*COLS)+j+1], (FLOAT)B[((i-1)*COLS)+j+1]);
                data[7]=_mm_set_ps((FLOAT)B[((i+3)*COLS)+j+1], (FLOAT)B[((i+2)*COLS)+j+1],
                                   (FLOAT)B[((i+1)*COLS)+j+1], (FLOAT)B[((i)*COLS)+j+1]);
                data[8]=_mm_set_ps((FLOAT)B[((i+4)*COLS)+j+1], (FLOAT)B[((i+3)*COLS)+j+1],
                                   (FLOAT)B[((i+2)*COLS)+j+1], (FLOAT)B[((i+1)*COLS)+j+1]);
                
                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[0], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[1], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[2], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[3], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[4], k_center));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[5], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[6], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[7], k_sides));

                accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[8], k_sides));
                

                accumulators = _mm_min_ps(accumulators, register_255);
                accumulators = _mm_max_ps(accumulators, register_0);

                _mm_store_ps(accumulators_ram, accumulators);

                convB[(i*COLS)+j]=(UINT8)accumulators_ram[0];
                convB[((i+1)*COLS)+j]=(UINT8)accumulators_ram[1];
                convB[((i+2)*COLS)+j]=(UINT8)accumulators_ram[2];
                convB[((i+3)*COLS)+j]=(UINT8)accumulators_ram[3];
            }
        }

        //Last rows
        for(i; i<ROWS-1; i++)
        {
            // Skip first and last column, no neighbors to convolve with
            for(j=1; j<COLS-1; j++)
            {
                temp=0;
                temp += (PSF[0] * (FLOAT)B[((i-1)*320)+j-1]);
                temp += (PSF[1] * (FLOAT)B[((i-1)*320)+j]);
                temp += (PSF[2] * (FLOAT)B[((i-1)*320)+j+1]);
                temp += (PSF[3] * (FLOAT)B[((i)*320)+j-1]);
                temp += (PSF[4] * (FLOAT)B[((i)*320)+j]);
                temp += (PSF[5] * (FLOAT)B[((i)*320)+j+1]);
                temp += (PSF[6] * (FLOAT)B[((i+1)*320)+j-1]);
                temp += (PSF[7] * (FLOAT)B[((i+1)*320)+j]);
                temp += (PSF[8] * (FLOAT)B[((i+1)*320)+j+1]);  

                if(temp<0.0) temp=0.0;
                if(temp>255.0) temp=255.0;
                convB[(i*320)+j]=(UINT8)temp;  
            }
        }

        /*
        //GREEN
        for(i=1; i<ROWS-1; i++)
        {
            // Skip first and last column, no neighbors to convolve with
            for(j=1; j<COLS-1; j++)
            {

                temp=0;
                temp += (PSF[0] * (FLOAT)G[((i-1)*COLS)+j-1]);
                temp += (PSF[1] * (FLOAT)G[((i-1)*COLS)+j]);
                temp += (PSF[2] * (FLOAT)G[((i-1)*COLS)+j+1]);
                temp += (PSF[3] * (FLOAT)G[((i)*COLS)+j-1]);
                temp += (PSF[4] * (FLOAT)G[((i)*COLS)+j]);
                temp += (PSF[5] * (FLOAT)G[((i)*COLS)+j+1]);
                temp += (PSF[6] * (FLOAT)G[((i+1)*COLS)+j-1]);
                temp += (PSF[7] * (FLOAT)G[((i+1)*COLS)+j]);
                temp += (PSF[8] * (FLOAT)G[((i+1)*COLS)+j+1]);

        	    if(temp<0.0) temp=0.0;
        	    if(temp>255.0) temp=255.0;
        	    convG[(i*COLS)+j]=(UINT8)temp;
            }
        }
        

        //BLUE
        for(i=1; i<ROWS-1; i++)
        {
            // Skip first and last column, no neighbors to convolve with
            for(j=1; j<COLS-1; j++)
            {
                temp=0;
                temp += (PSF[0] * (FLOAT)B[((i-1)*COLS)+j-1]);
                temp += (PSF[1] * (FLOAT)B[((i-1)*COLS)+j]);
                temp += (PSF[2] * (FLOAT)B[((i-1)*COLS)+j+1]);
                temp += (PSF[3] * (FLOAT)B[((i)*COLS)+j-1]);
                temp += (PSF[4] * (FLOAT)B[((i)*COLS)+j]);
                temp += (PSF[5] * (FLOAT)B[((i)*COLS)+j+1]);
                temp += (PSF[6] * (FLOAT)B[((i+1)*COLS)+j-1]);
                temp += (PSF[7] * (FLOAT)B[((i+1)*COLS)+j]);
                temp += (PSF[8] * (FLOAT)B[((i+1)*COLS)+j+1]);

        	    if(temp<0.0) temp=0.0;
        	    if(temp>255.0) temp=255.0;
        	    convB[(i*COLS)+j]=(UINT8)temp;
            }
        }
        */
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
