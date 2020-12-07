
for(j=1; j<COLS-1; j+=unroll_iterations)
{

    printf("%d\n", j);
    fflush(stdout);

    //Load the image rows into the registers
    //Row 1:
    for(z=j-1; z-j<9; z++)
    {   
        row_tmp[z] = (FLOAT)R[((i-1)*COLS)+z];
    }

    row1_1 = _mm256_loadu_ps(&row_tmp[0]);//j-i:j+6
    row1_2 = _mm256_loadu_ps(&row_tmp[1]);//j:j+7
    row1_3 = _mm256_loadu_ps(&row_tmp[2]);//j+1:j+8

    //Row 2:
    for(z=j-1; z-j<9; z++)
    {
        row_tmp[z] = (FLOAT)R[((i)*COLS)+z];
    }
    row2_1 = _mm256_loadu_ps(&row_tmp[0]);
    row2_2 = _mm256_loadu_ps(&row_tmp[1]);
    row2_3 = _mm256_loadu_ps(&row_tmp[2]);

    //Row 3:
    for(z=j-1; z-j<9; z++)
    {
        row_tmp[z] = (FLOAT)R[((i+1)*COLS)+z];
    }
    row3_1 = _mm256_loadu_ps(&row_tmp[0]);
    row3_2 = _mm256_loadu_ps(&row_tmp[1]);
    row3_3 = _mm256_loadu_ps(&row_tmp[2]);

    accumulators = _mm256_set1_ps(0);

    //Row 1-1:
    kernel_tmp = _mm256_set1_ps(PSF[0]);
    accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(row1_1, kernel_tmp));
    //Row 1-2:
    kernel_tmp = _mm256_set1_ps(PSF[1]);
    accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(row1_2, kernel_tmp));
    //Row 1-3:
    kernel_tmp = _mm256_set1_ps(PSF[2]);
    accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(row1_3, kernel_tmp));

    //Row 2-1:
    kernel_tmp = _mm256_set1_ps(PSF[3]);
    accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(row2_1, kernel_tmp));
    //Row 2-2:
    kernel_tmp = _mm256_set1_ps(PSF[4]);
    accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(row2_2, kernel_tmp));
    //Row 2-3:
    kernel_tmp = _mm256_set1_ps(PSF[5]);
    accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(row2_3, kernel_tmp));

    //Row 3-1:
    kernel_tmp = _mm256_set1_ps(PSF[6]);
    accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(row3_1, kernel_tmp));
    //Row 3-2:
    kernel_tmp = _mm256_set1_ps(PSF[7]);
    accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(row3_2, kernel_tmp));
    //Row 3-3:
    kernel_tmp = _mm256_set1_ps(PSF[8]);
    accumulators = _mm256_add_ps(accumulators, _mm256_mul_ps(row3_3, kernel_tmp));
    
    for(pixel=j; pixel-j<unroll_iterations && pixel<COLS-1; pixel++)
    {
        tmp = accumulators[pixel-j];

        //printf("%f\n", tmp);

        if(tmp<0.0) tmp=0.0;
        if(tmp>255.0) tmp=255.0;

        convR[(i*COLS)+pixel]=(UINT8)tmp;
    }

}

//LOADS I'VE TRIED

//THIS DOESN'T WORK, WE HAVE TO READ UINT8
    /*
    //loadu_ps loads 8 32 bit floats from memory
    //Row 1:
    //j-i:j+6
    row1_1 = _mm256_loadu_ps(R[((i-1)*COLS)+j-1]);
    //j:j+7
    row1_2 = _mm256_loadu_ps(R[((i-1)*COLS)+j]);
    //j+1:j+8
    row1_3 = _mm256_loadu_ps(R[((i-1)*COLS)+j+1]);
    //Row 2:
    row2_1 = _mm256_loadu_ps(R[((i)*COLS)+j-1]);
    row2_2 = _mm256_loadu_ps(R[((i)*COLS)+j]);
    row2_3 = _mm256_loadu_ps(R[((i)*COLS)+j+1]);
    //Row 3:
    row3_1 = _mm256_loadu_ps(R[((i+1)*COLS)+j-1]);
    row3_2 = _mm256_loadu_ps(R[((i+1)*COLS)+j]);
    row3_3 = _mm256_loadu_ps(R[((i+1)*COLS)+j+1]);
    //_mm256_castpd_ps
    */

//FOR SOME REASON AFTER EXTENSION EVERYTHING IS 0
/*
    //loadu_si64 loads 64 bits from memory into the 0-63 indexes of a 128-bit register
    //Row 1: 
    row1_1i = _mm_loadu_si64(&R[((i-1)*COLS)+j-1]);//j-i:j+6
    row1_2i = _mm_loadu_si64(&R[((i-1)*COLS)+j]);//j:j+7
    row1_3i = _mm_loadu_si64(&R[((i-1)*COLS)+j+1]);//j+1:j+8
    //Row 2:
    row2_1i = _mm_loadu_si64(&R[((i)*COLS)+j-1]);
    row2_2i = _mm_loadu_si64(&R[((i)*COLS)+j]);
    row2_3i = _mm_loadu_si64(&R[((i)*COLS)+j+1]);
    //Row 3:
    row3_1i = _mm_loadu_si64(&R[((i+1)*COLS)+j-1]);
    row3_2i = _mm_loadu_si64(&R[((i+1)*COLS)+j]);
    row3_3i = _mm_loadu_si64(&R[((i+1)*COLS)+j+1]);

    //cvtepu8_epi32 zero extends 8 8-bit ints in the 0-63 indexes of a 128 bit register to 32 bits 
    row1_1 = _mm256_castsi256_ps (_mm256_cvtepu8_epi32 (row1_1i));
    row1_2 = _mm256_castsi256_ps (_mm256_cvtepu8_epi32 (row1_2i));
    row1_3 = _mm256_castsi256_ps (_mm256_cvtepu8_epi32 (row1_3i));
    row2_1 = _mm256_castsi256_ps (_mm256_cvtepu8_epi32 (row2_1i));
    row2_2 = _mm256_castsi256_ps (_mm256_cvtepu8_epi32 (row2_2i));
    row2_3 = _mm256_castsi256_ps (_mm256_cvtepu8_epi32 (row2_3i));
    row3_1 = _mm256_castsi256_ps (_mm256_cvtepu8_epi32 (row3_1i));
    row3_2 = _mm256_castsi256_ps (_mm256_cvtepu8_epi32 (row3_2i));
    row3_3 = _mm256_castsi256_ps (_mm256_cvtepu8_epi32 (row3_3i));
*/

//Carga todo a un array de floats, casteando a float, y de ahí al registro para no tener que usar 
//las instrucciones de extensión con ceros
/*
    for(z=j-1; z-j<9; z++)
    {
        row_tmp[z] = (FLOAT)R[((i-1)*COLS)+j];
    }
    row1_1 = _mm256_loadu_ps(row_tmp[0]);//j-i:j+6
    row1_2 = _mm256_loadu_ps(row_tmp[1]);//j:j+7
    row1_3 = _mm256_loadu_ps(row_tmp[2]);//j+1:j+8
    //Row 2:
    for(z=j-1; z-j<9; z++)
    {
        row_tmp[z] = (FLOAT)R[((i)*COLS)+j];
    }
    row2_1 = _mm256_loadu_ps(row_tmp[0]);
    row2_2 = _mm256_loadu_ps(row_tmp[1]);
    row2_3 = _mm256_loadu_ps(row_tmp[2]);
    //Row 3:
    for(z=j-1; z-j<9; z++)
    {
        row_tmp[z] = (FLOAT)R[((i+1)*COLS)+j];
    }
    row3_1 = _mm256_loadu_ps(row_tmp[0]);
    row3_2 = _mm256_loadu_ps(row_tmp[1]);
    row3_3 = _mm256_loadu_ps(row_tmp[2]);
*/

//SAVES I'VE TRIED

/*
int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
__m256i accumulators_int = (__m256i)accumulators;
tmp0 = (FLOAT)_mm256_extract_epi32(accumulators_int, 0);
tmp1 = (FLOAT)_mm256_extract_epi32(accumulators_int, 1);
tmp2 = (FLOAT)_mm256_extract_epi32(accumulators_int, 2);
tmp3 = (FLOAT)_mm256_extract_epi32(accumulators_int, 3);
tmp4 = (FLOAT)_mm256_extract_epi32(accumulators_int, 4);
tmp5 = (FLOAT)_mm256_extract_epi32(accumulators_int, 5);
tmp6 = (FLOAT)_mm256_extract_epi32(accumulators_int, 6);
tmp7 = (FLOAT)_mm256_extract_epi32(accumulators_int, 7);

if(tmp0<0.0) tmp0=0.0;if(tmp0>255.0) tmp0=255.0;convR[(i*COLS)+j]=(UINT8)tmp0;
if(tmp1<0.0) tmp1=0.0;if(tmp1>255.0) tmp1=255.0;convR[(i*COLS)+j+1]=(UINT8)tmp1;
if(tmp2<0.0) tmp2=0.0;if(tmp2>255.0) tmp2=255.0;convR[(i*COLS)+j+2]=(UINT8)tmp2;
if(tmp3<0.0) tmp3=0.0;if(tmp3>255.0) tmp3=255.0;convR[(i*COLS)+j+3]=(UINT8)tmp3;
if(tmp4<0.0) tmp4=0.0;if(tmp4>255.0) tmp4=255.0;convR[(i*COLS)+j+4]=(UINT8)tmp4;
if(tmp5<0.0) tmp5=0.0;if(tmp5>255.0) tmp5=255.0;convR[(i*COLS)+j+5]=(UINT8)tmp5;
if(tmp6<0.0) tmp6=0.0;if(tmp6>255.0) tmp6=255.0;convR[(i*COLS)+j+6]=(UINT8)tmp6;
if(tmp7<0.0) tmp7=0.0;if(tmp7>255.0) tmp7=255.0;convR[(i*COLS)+j+7]=(UINT8)tmp7;

*/


/*
for(pixel=j; pixel-j<unroll_iterations && pixel<COLS-1; pixel++)
{
    //memcpy(&tmp, accumulators_ptr, 4);
    //tmp = _mm256_cvtss_f32(accumulators);
    //__m256i accumulators_int = (__m256i)accumulators;
    //accumulators_int = _mm256_slli_epi32 (accumulators_int, 32);
    //accumulators = (__m256)accumulators_int;

    //accumulators = accumulators>>32;

    if(tmp<0.0) tmp=0.0;
    if(tmp>255.0) tmp=255.0;

    convR[(i*COLS)+pixel]=(UINT8)tmp;
    
    accumulators_ptr++;
}
*/

//This for checking
/*
tmp0 = (FLOAT)_mm256_extract_epi32((__m256i)row1_1, 0);
tmp1 = (FLOAT)_mm256_extract_epi32((__m256i)row1_1, 1);
tmp2 = (FLOAT)_mm256_extract_epi32((__m256i)row1_1, 2);
tmp3 = (FLOAT)_mm256_extract_epi32((__m256i)row1_1, 3);
tmp4 = (FLOAT)_mm256_extract_epi32((__m256i)row1_1, 4);
tmp5 = (FLOAT)_mm256_extract_epi32((__m256i)row1_1, 5);
tmp6 = (FLOAT)_mm256_extract_epi32((__m256i)row1_1, 6);
tmp7 = (FLOAT)_mm256_extract_epi32((__m256i)row1_1, 7);
printf("%f, %f, %f, %f, %f, %f, %f, %f\n", tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7);
*/