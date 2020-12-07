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
            data[3]=_mm256_load_ps(&R_float[(i-1)*COLS + j]);
            data[4]=_mm256_load_ps(&R_float[(i-1)*COLS + j+1]);
            data[5]=_mm256_load_ps(&R_float[(i-1)*COLS + j+2]);

            data[6]=_mm256_load_ps(&R_float[(i)*COLS + j]);
            data[7]=_mm256_load_ps(&R_float[(i)*COLS + j+1]);
            data[8]=_mm256_load_ps(&R_float[(i)*COLS + j+2]);

            for(i=1; i<ROWS-1; i++)
            {   
                //Initialise accumulators
                accumulators = _mm256_set1_ps(0);

                //Move data registers to remove first row and make second and third 
                //represent first and second
                data[0]=data[3];
                data[1]=data[4];
                data[2]=data[5];

                data[3]=data[6];
                data[4]=data[7];
                data[5]=data[8];

                //Load data for the last row
                data[6]=_mm256_load_ps(&R_float[(i+1)*COLS + j]);
                data[7]=_mm256_load_ps(&R_float[(i+1)*COLS + j+1]);
                data[8]=_mm256_load_ps(&R_float[(i+1)*COLS + j+2]);
                
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
            data[3]=_mm256_load_ps(&G_float[(i-1)*COLS + j]);
            data[4]=_mm256_load_ps(&G_float[(i-1)*COLS + j+1]);
            data[5]=_mm256_load_ps(&G_float[(i-1)*COLS + j+2]);

            data[6]=_mm256_load_ps(&G_float[(i)*COLS + j]);
            data[7]=_mm256_load_ps(&G_float[(i)*COLS + j+1]);
            data[8]=_mm256_load_ps(&G_float[(i)*COLS + j+2]);

            for(i=1; i<ROWS-1; i++)
            {   
                //Initialise accumulators
                accumulators = _mm256_set1_ps(0);

                //Move data registers to remove first row and make second and third 
                //represent first and second
                data[0]=data[3];
                data[1]=data[4];
                data[2]=data[5];

                data[3]=data[6];
                data[4]=data[7];
                data[5]=data[8];

                //Load data for the last row
                data[6]=_mm256_load_ps(&G_float[(i+1)*COLS + j]);
                data[7]=_mm256_load_ps(&G_float[(i+1)*COLS + j+1]);
                data[8]=_mm256_load_ps(&G_float[(i+1)*COLS + j+2]);
                
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
            data[3]=_mm256_load_ps(&B_float[(i-1)*COLS + j]);
            data[4]=_mm256_load_ps(&B_float[(i-1)*COLS + j+1]);
            data[5]=_mm256_load_ps(&B_float[(i-1)*COLS + j+2]);

            data[6]=_mm256_load_ps(&B_float[(i)*COLS + j]);
            data[7]=_mm256_load_ps(&B_float[(i)*COLS + j+1]);
            data[8]=_mm256_load_ps(&B_float[(i)*COLS + j+2]);

            for(i=1; i<ROWS-1; i++)
            {   
                //Initialise accumulators
                accumulators = _mm256_set1_ps(0);

                //Move data registers to remove first row and make second and third 
                //represent first and second
                data[0]=data[3];
                data[1]=data[4];
                data[2]=data[5];

                data[3]=data[6];
                data[4]=data[7];
                data[5]=data[8];

                //Load data for the last row
                data[6]=_mm256_load_ps(&B_float[(i+1)*COLS + j]);
                data[7]=_mm256_load_ps(&B_float[(i+1)*COLS + j+1]);
                data[8]=_mm256_load_ps(&B_float[(i+1)*COLS + j+2]);
                
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