__m256 data[9], kernel_tmp, accumulators, k_sides, k_center, register_255, register_0;
//floats for reading from the accumulators register and for loading and casting to float each row
float accumulators_ram[8];

k_sides = _mm256_set1_ps(PSF[0]);
k_center = _mm256_set1_ps(PSF[4]);

register_255 = _mm256_set1_ps(255);
register_0 = _mm256_set1_ps(0);



// Start of convolution time stamp
///////////////////////////////////////////////////////////////////////////////////////////

ctimer_(&elapsed0, &ucpu0, &scpu0);
for (vuelta=1;vuelta<500;vuelta++)
{
    for(i=1; i+8<ROWS-1; i+=8)
    {
        j=1;
        //Register preload:
        data[3]=_mm256_set_ps((FLOAT)R[((i+6)*COLS)+j-1], (FLOAT)R[((i+5)*COLS)+j-1],
                              (FLOAT)R[((i+4)*COLS)+j-1], (FLOAT)R[((i+3)*COLS)+j-1],
                              (FLOAT)R[((i+2)*COLS)+j-1], (FLOAT)R[((i+1)*COLS)+j-1],
                              (FLOAT)R[((i)*COLS)+j-1], (FLOAT)R[((i-1)*COLS)+j-1]);
        data[4]=_mm256_set_ps((FLOAT)R[((i+7)*COLS)+j-1], (FLOAT)R[((i+6)*COLS)+j-1],
                              (FLOAT)R[((i+5)*COLS)+j-1], (FLOAT)R[((i+4)*COLS)+j-1],
                              (FLOAT)R[((i+3)*COLS)+j-1], (FLOAT)R[((i+2)*COLS)+j-1],
                              (FLOAT)R[((i+1)*COLS)+j-1], (FLOAT)R[((i)*COLS)+j-1]);
        data[5]=_mm256_set_ps((FLOAT)R[((i+8)*COLS)+j-1], (FLOAT)R[((i+7)*COLS)+j-1],
                              (FLOAT)R[((i+6)*COLS)+j-1], (FLOAT)R[((i+5)*COLS)+j-1],
                              (FLOAT)R[((i+4)*COLS)+j-1], (FLOAT)R[((i+3)*COLS)+j-1],
                              (FLOAT)R[((i+2)*COLS)+j-1], (FLOAT)R[((i+1)*COLS)+j-1]);


        data[6]=_mm256_set_ps((FLOAT)R[((i+6)*COLS)+j], (FLOAT)R[((i+5)*COLS)+j],
                              (FLOAT)R[((i+4)*COLS)+j], (FLOAT)R[((i+3)*COLS)+j],
                              (FLOAT)R[((i+2)*COLS)+j], (FLOAT)R[((i+1)*COLS)+j],
                              (FLOAT)R[((i)*COLS)+j], (FLOAT)R[((i-1)*COLS)+j]);
        data[7]=_mm256_set_ps((FLOAT)R[((i+7)*COLS)+j], (FLOAT)R[((i+6)*COLS)+j],
                              (FLOAT)R[((i+5)*COLS)+j], (FLOAT)R[((i+4)*COLS)+j],
                              (FLOAT)R[((i+3)*COLS)+j], (FLOAT)R[((i+2)*COLS)+j],
                              (FLOAT)R[((i+1)*COLS)+j], (FLOAT)R[((i)*COLS)+j]);
        data[8]=_mm256_set_ps((FLOAT)R[((i+8)*COLS)+j], (FLOAT)R[((i+7)*COLS)+j],
                              (FLOAT)R[((i+6)*COLS)+j], (FLOAT)R[((i+5)*COLS)+j],
                              (FLOAT)R[((i+4)*COLS)+j], (FLOAT)R[((i+3)*COLS)+j],
                              (FLOAT)R[((i+2)*COLS)+j], (FLOAT)R[((i+1)*COLS)+j]);

        // Skip first and last column, no neighbors to convolve with
        for(j=1; j<COLS-1; j++)
        {   

            //Initialise accumulators
            accumulators = _mm256_set1_ps(0);

            //Move data registers to remove first column and make second and third 
            //represent first and second
            data[0]=data[3];
            data[1]=data[4];
            data[2]=data[5];

            data[3]=data[6];
            data[4]=data[7];
            data[5]=data[8];

            //Load data for the last column
            data[6]=_mm256_set_ps((FLOAT)R[((i+6)*COLS)+j+1], (FLOAT)R[((i+5)*COLS)+j+1],
                                  (FLOAT)R[((i+4)*COLS)+j+1], (FLOAT)R[((i+3)*COLS)+j+1],
                                  (FLOAT)R[((i+2)*COLS)+j+1], (FLOAT)R[((i+1)*COLS)+j+1],
                                  (FLOAT)R[((i)*COLS)+j+1], (FLOAT)R[((i-1)*COLS)+j+1]);
            data[7]=_mm256_set_ps((FLOAT)R[((i+7)*COLS)+j+1], (FLOAT)R[((i+6)*COLS)+j+1],
                                  (FLOAT)R[((i+5)*COLS)+j+1], (FLOAT)R[((i+4)*COLS)+j+1],
                                  (FLOAT)R[((i+3)*COLS)+j+1], (FLOAT)R[((i+2)*COLS)+j+1],
                                  (FLOAT)R[((i+1)*COLS)+j+1], (FLOAT)R[((i)*COLS)+j+1]);
            data[8]=_mm256_set_ps((FLOAT)R[((i+8)*COLS)+j+1], (FLOAT)R[((i+7)*COLS)+j+1],
                                  (FLOAT)R[((i+6)*COLS)+j+1], (FLOAT)R[((i+5)*COLS)+j+1],
                                  (FLOAT)R[((i+4)*COLS)+j+1], (FLOAT)R[((i+3)*COLS)+j+1],
                                  (FLOAT)R[((i+2)*COLS)+j+1], (FLOAT)R[((i+1)*COLS)+j+1]);
            
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

            _mm256_store_ps(accumulators_ram, accumulators);

            convR[(i*COLS)+j]=(UINT8)accumulators_ram[0];
            convR[((i+1)*COLS)+j]=(UINT8)accumulators_ram[1];
            convR[((i+2)*COLS)+j]=(UINT8)accumulators_ram[2];
            convR[((i+3)*COLS)+j]=(UINT8)accumulators_ram[3];
            convR[((i+4)*COLS)+j]=(UINT8)accumulators_ram[4];
            convR[((i+5)*COLS)+j]=(UINT8)accumulators_ram[5];
            convR[((i+6)*COLS)+j]=(UINT8)accumulators_ram[6];
            convR[((i+7)*COLS)+j]=(UINT8)accumulators_ram[7];

            /*
            convR[(i*COLS)+j]=(UINT8)accumulators[0];
            convR[((i+1)*COLS)+j]=(UINT8)accumulators[1];
            convR[((i+2)*COLS)+j]=(UINT8)accumulators[2];
            convR[((i+3)*COLS)+j]=(UINT8)accumulators[3];
            */
        }
    }
    
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
    for(i=1; i+8<ROWS-1; i+=8)
    {
        j=1;
        //Register preload:
        data[3]=_mm256_set_ps((FLOAT)G[((i+6)*COLS)+j-1], (FLOAT)G[((i+5)*COLS)+j-1],
                              (FLOAT)G[((i+4)*COLS)+j-1], (FLOAT)G[((i+3)*COLS)+j-1],
                              (FLOAT)G[((i+2)*COLS)+j-1], (FLOAT)G[((i+1)*COLS)+j-1],
                              (FLOAT)G[((i)*COLS)+j-1], (FLOAT)G[((i-1)*COLS)+j-1]);
        data[4]=_mm256_set_ps((FLOAT)G[((i+7)*COLS)+j-1], (FLOAT)G[((i+6)*COLS)+j-1],
                              (FLOAT)G[((i+5)*COLS)+j-1], (FLOAT)G[((i+4)*COLS)+j-1],
                              (FLOAT)G[((i+3)*COLS)+j-1], (FLOAT)G[((i+2)*COLS)+j-1],
                              (FLOAT)G[((i+1)*COLS)+j-1], (FLOAT)G[((i)*COLS)+j-1]);
        data[5]=_mm256_set_ps((FLOAT)G[((i+8)*COLS)+j-1], (FLOAT)G[((i+7)*COLS)+j-1],
                              (FLOAT)G[((i+6)*COLS)+j-1], (FLOAT)G[((i+5)*COLS)+j-1],
                              (FLOAT)G[((i+4)*COLS)+j-1], (FLOAT)G[((i+3)*COLS)+j-1],
                              (FLOAT)G[((i+2)*COLS)+j-1], (FLOAT)G[((i+1)*COLS)+j-1]);


        data[6]=_mm256_set_ps((FLOAT)G[((i+6)*COLS)+j], (FLOAT)G[((i+5)*COLS)+j],
                              (FLOAT)G[((i+4)*COLS)+j], (FLOAT)G[((i+3)*COLS)+j],
                              (FLOAT)G[((i+2)*COLS)+j], (FLOAT)G[((i+1)*COLS)+j],
                              (FLOAT)G[((i)*COLS)+j], (FLOAT)G[((i-1)*COLS)+j]);
        data[7]=_mm256_set_ps((FLOAT)G[((i+7)*COLS)+j], (FLOAT)G[((i+6)*COLS)+j],
                              (FLOAT)G[((i+5)*COLS)+j], (FLOAT)G[((i+4)*COLS)+j],
                              (FLOAT)G[((i+3)*COLS)+j], (FLOAT)G[((i+2)*COLS)+j],
                              (FLOAT)G[((i+1)*COLS)+j], (FLOAT)G[((i)*COLS)+j]);
        data[8]=_mm256_set_ps((FLOAT)G[((i+8)*COLS)+j], (FLOAT)G[((i+7)*COLS)+j],
                              (FLOAT)G[((i+6)*COLS)+j], (FLOAT)G[((i+5)*COLS)+j],
                              (FLOAT)G[((i+4)*COLS)+j], (FLOAT)G[((i+3)*COLS)+j],
                              (FLOAT)G[((i+2)*COLS)+j], (FLOAT)G[((i+1)*COLS)+j]);

        // Skip first and last column, no neighbors to convolve with
        for(j=1; j<COLS-1; j++)
        {   

            //Initialise accumulators
            accumulators = _mm256_set1_ps(0);

            //Move data registers to remove first column and make second and third 
            //represent first and second
            data[0]=data[3];
            data[1]=data[4];
            data[2]=data[5];

            data[3]=data[6];
            data[4]=data[7];
            data[5]=data[8];

            //Load data for the last column
            data[6]=_mm256_set_ps((FLOAT)G[((i+6)*COLS)+j+1], (FLOAT)G[((i+5)*COLS)+j+1],
                                  (FLOAT)G[((i+4)*COLS)+j+1], (FLOAT)G[((i+3)*COLS)+j+1],
                                  (FLOAT)G[((i+2)*COLS)+j+1], (FLOAT)G[((i+1)*COLS)+j+1],
                                  (FLOAT)G[((i)*COLS)+j+1], (FLOAT)G[((i-1)*COLS)+j+1]);
            data[7]=_mm256_set_ps((FLOAT)G[((i+7)*COLS)+j+1], (FLOAT)G[((i+6)*COLS)+j+1],
                                  (FLOAT)G[((i+5)*COLS)+j+1], (FLOAT)G[((i+4)*COLS)+j+1],
                                  (FLOAT)G[((i+3)*COLS)+j+1], (FLOAT)G[((i+2)*COLS)+j+1],
                                  (FLOAT)G[((i+1)*COLS)+j+1], (FLOAT)G[((i)*COLS)+j+1]);
            data[8]=_mm256_set_ps((FLOAT)G[((i+8)*COLS)+j+1], (FLOAT)G[((i+7)*COLS)+j+1],
                                  (FLOAT)G[((i+6)*COLS)+j+1], (FLOAT)G[((i+5)*COLS)+j+1],
                                  (FLOAT)G[((i+4)*COLS)+j+1], (FLOAT)G[((i+3)*COLS)+j+1],
                                  (FLOAT)G[((i+2)*COLS)+j+1], (FLOAT)G[((i+1)*COLS)+j+1]);
            
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

            _mm256_store_ps(accumulators_ram, accumulators);

            convG[(i*COLS)+j]=(UINT8)accumulators_ram[0];
            convG[((i+1)*COLS)+j]=(UINT8)accumulators_ram[1];
            convG[((i+2)*COLS)+j]=(UINT8)accumulators_ram[2];
            convG[((i+3)*COLS)+j]=(UINT8)accumulators_ram[3];
            convG[((i+4)*COLS)+j]=(UINT8)accumulators_ram[4];
            convG[((i+5)*COLS)+j]=(UINT8)accumulators_ram[5];
            convG[((i+6)*COLS)+j]=(UINT8)accumulators_ram[6];
            convG[((i+7)*COLS)+j]=(UINT8)accumulators_ram[7];

            /*
            convG[(i*COLS)+j]=(UINT8)accumulators[0];
            convG[((i+1)*COLS)+j]=(UINT8)accumulators[1];
            convG[((i+2)*COLS)+j]=(UINT8)accumulators[2];
            convG[((i+3)*COLS)+j]=(UINT8)accumulators[3];
            */
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
    for(i=1; i+8<ROWS-1; i+=8)
    {
        j=1;
        //Register preload:
        data[3]=_mm256_set_ps((FLOAT)B[((i+6)*COLS)+j-1], (FLOAT)B[((i+5)*COLS)+j-1],
                              (FLOAT)B[((i+4)*COLS)+j-1], (FLOAT)B[((i+3)*COLS)+j-1],
                              (FLOAT)B[((i+2)*COLS)+j-1], (FLOAT)B[((i+1)*COLS)+j-1],
                              (FLOAT)B[((i)*COLS)+j-1], (FLOAT)B[((i-1)*COLS)+j-1]);
        data[4]=_mm256_set_ps((FLOAT)B[((i+7)*COLS)+j-1], (FLOAT)B[((i+6)*COLS)+j-1],
                              (FLOAT)B[((i+5)*COLS)+j-1], (FLOAT)B[((i+4)*COLS)+j-1],
                              (FLOAT)B[((i+3)*COLS)+j-1], (FLOAT)B[((i+2)*COLS)+j-1],
                              (FLOAT)B[((i+1)*COLS)+j-1], (FLOAT)B[((i)*COLS)+j-1]);
        data[5]=_mm256_set_ps((FLOAT)B[((i+8)*COLS)+j-1], (FLOAT)B[((i+7)*COLS)+j-1],
                              (FLOAT)B[((i+6)*COLS)+j-1], (FLOAT)B[((i+5)*COLS)+j-1],
                              (FLOAT)B[((i+4)*COLS)+j-1], (FLOAT)B[((i+3)*COLS)+j-1],
                              (FLOAT)B[((i+2)*COLS)+j-1], (FLOAT)B[((i+1)*COLS)+j-1]);


        data[6]=_mm256_set_ps((FLOAT)B[((i+6)*COLS)+j], (FLOAT)B[((i+5)*COLS)+j],
                              (FLOAT)B[((i+4)*COLS)+j], (FLOAT)B[((i+3)*COLS)+j],
                              (FLOAT)B[((i+2)*COLS)+j], (FLOAT)B[((i+1)*COLS)+j],
                              (FLOAT)B[((i)*COLS)+j], (FLOAT)B[((i-1)*COLS)+j]);
        data[7]=_mm256_set_ps((FLOAT)B[((i+7)*COLS)+j], (FLOAT)B[((i+6)*COLS)+j],
                              (FLOAT)B[((i+5)*COLS)+j], (FLOAT)B[((i+4)*COLS)+j],
                              (FLOAT)B[((i+3)*COLS)+j], (FLOAT)B[((i+2)*COLS)+j],
                              (FLOAT)B[((i+1)*COLS)+j], (FLOAT)B[((i)*COLS)+j]);
        data[8]=_mm256_set_ps((FLOAT)B[((i+8)*COLS)+j], (FLOAT)B[((i+7)*COLS)+j],
                              (FLOAT)B[((i+6)*COLS)+j], (FLOAT)B[((i+5)*COLS)+j],
                              (FLOAT)B[((i+4)*COLS)+j], (FLOAT)B[((i+3)*COLS)+j],
                              (FLOAT)B[((i+2)*COLS)+j], (FLOAT)B[((i+1)*COLS)+j]);

        // Skip first and last column, no neighbors to convolve with
        for(j=1; j<COLS-1; j++)
        {   

            //Initialise accumulators
            accumulators = _mm256_set1_ps(0);

            //Move data registers to remove first column and make second and third 
            //represent first and second
            data[0]=data[3];
            data[1]=data[4];
            data[2]=data[5];

            data[3]=data[6];
            data[4]=data[7];
            data[5]=data[8];

            //Load data for the last column
            data[6]=_mm256_set_ps((FLOAT)B[((i+6)*COLS)+j+1], (FLOAT)B[((i+5)*COLS)+j+1],
                                  (FLOAT)B[((i+4)*COLS)+j+1], (FLOAT)B[((i+3)*COLS)+j+1],
                                  (FLOAT)B[((i+2)*COLS)+j+1], (FLOAT)B[((i+1)*COLS)+j+1],
                                  (FLOAT)B[((i)*COLS)+j+1], (FLOAT)B[((i-1)*COLS)+j+1]);
            data[7]=_mm256_set_ps((FLOAT)B[((i+7)*COLS)+j+1], (FLOAT)B[((i+6)*COLS)+j+1],
                                  (FLOAT)B[((i+5)*COLS)+j+1], (FLOAT)B[((i+4)*COLS)+j+1],
                                  (FLOAT)B[((i+3)*COLS)+j+1], (FLOAT)B[((i+2)*COLS)+j+1],
                                  (FLOAT)B[((i+1)*COLS)+j+1], (FLOAT)B[((i)*COLS)+j+1]);
            data[8]=_mm256_set_ps((FLOAT)B[((i+8)*COLS)+j+1], (FLOAT)B[((i+7)*COLS)+j+1],
                                  (FLOAT)B[((i+6)*COLS)+j+1], (FLOAT)B[((i+5)*COLS)+j+1],
                                  (FLOAT)B[((i+4)*COLS)+j+1], (FLOAT)B[((i+3)*COLS)+j+1],
                                  (FLOAT)B[((i+2)*COLS)+j+1], (FLOAT)B[((i+1)*COLS)+j+1]);
            
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

            _mm256_store_ps(accumulators_ram, accumulators);

            convB[(i*COLS)+j]=(UINT8)accumulators_ram[0];
            convB[((i+1)*COLS)+j]=(UINT8)accumulators_ram[1];
            convB[((i+2)*COLS)+j]=(UINT8)accumulators_ram[2];
            convB[((i+3)*COLS)+j]=(UINT8)accumulators_ram[3];
            convB[((i+4)*COLS)+j]=(UINT8)accumulators_ram[4];
            convB[((i+5)*COLS)+j]=(UINT8)accumulators_ram[5];
            convB[((i+6)*COLS)+j]=(UINT8)accumulators_ram[6];
            convB[((i+7)*COLS)+j]=(UINT8)accumulators_ram[7];

            /*
            convR[(i*COLS)+j]=(UINT8)accumulators[0];
            convR[((i+1)*COLS)+j]=(UINT8)accumulators[1];
            convR[((i+2)*COLS)+j]=(UINT8)accumulators[2];
            convR[((i+3)*COLS)+j]=(UINT8)accumulators[3];
            */
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
