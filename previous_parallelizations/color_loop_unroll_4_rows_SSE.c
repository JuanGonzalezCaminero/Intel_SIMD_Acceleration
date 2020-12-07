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

            /*
            printf("%f, %f, %f \n %f, %f, %f \n %f, %f, %f \n %f, %f, %f \n\n", 
                data[0][0], data[3][0], data[6][0],
                data[0][1], data[3][1], data[6][1],
                data[0][2], data[3][2], data[6][2],
                data[0][3], data[3][3], data[6][3]);

            printf("%f, %f, %f \n %f, %f, %f \n %f, %f, %f \n %f, %f, %f \n\n", 
                data[1][0], data[4][0], data[7][0],
                data[1][1], data[4][1], data[7][1],
                data[1][2], data[4][2], data[7][2],
                data[1][3], data[4][3], data[7][3]);

            printf("%f, %f, %f \n %f, %f, %f \n %f, %f, %f \n %f, %f, %f \n\n", 
                data[2][0], data[5][0], data[8][0],
                data[2][1], data[5][1], data[8][1],
                data[2][2], data[5][2], data[8][2],
                data[2][3], data[5][3], data[8][3]);

            printf("-------------------------------------------\n\n");
            */
            
            accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[0], _mm_set_ps1(PSF[0])));

            accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[1], _mm_set_ps1(PSF[3])));

            accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[2], _mm_set_ps1(PSF[6])));

            accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[3], _mm_set_ps1(PSF[1])));

            accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[4], _mm_set_ps1(PSF[4])));

            accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[5], _mm_set_ps1(PSF[7])));

            accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[6], _mm_set_ps1(PSF[2])));

            accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[7], _mm_set_ps1(PSF[5])));

            accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[8], _mm_set_ps1(PSF[8])));
            

            accumulators = _mm_min_ps(accumulators, _mm_set_ps1(255));
            accumulators = _mm_max_ps(accumulators, _mm_set_ps1(0));

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

            /*
            temp0=accumulators[0];
            temp1=accumulators[1];
            temp2=accumulators[2];
            temp3=accumulators[3];
            
            if(temp0<0.0) temp0=0.0;
            if(temp0>255.0) temp0=255.0;
            convR[(i*COLS)+j]=(UINT8)temp0;

            if(temp1<0.0) temp1=0.0;
            if(temp1>255.0) temp1=255.0;
            convR[((i+1)*COLS)+j]=(UINT8)temp1;

            if(temp2<0.0) temp2=0.0;
            if(temp2>255.0) temp2=255.0;
            convR[((i+2)*COLS)+j]=(UINT8)temp2;

            if(temp3<0.0) temp3=0.0;
            if(temp3>255.0) temp3=255.0;
            convR[((i+3)*COLS)+j]=(UINT8)temp3;
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

    //Print red channel for checking
    /*
    for(i=1; i<ROWS-1; i++)
    {
        // Skip first and last column, no neighbors to convolve with
        for(j=1; j<COLS-1; j++)
        {
            printf("%d\n", convR[(i*320)+j]);  
        }
    }
    */

////////////////////////////////////////////////////////////////////////////////////////////
//VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////

__m128 data[9], kernel_tmp, accumulators, k_sides, k_center, register_255, register_0;
//floats for reading from the accumulators register and for loading and casting to float each row
float accumulators_ram[4];

k_sides = _mm_set_ps1(PSF[0]);
k_center = _mm_set_ps1(PSF[4]);

register_255 = _mm_set_ps1(255);
register_0 = _mm_set_ps1(0);


////////////////////////////////////////////////////////////////////////////////////////////
//ALL LOOPS:
////////////////////////////////////////////////////////////////////////////////////////////

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
        
        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[0], _mm_set_ps1(PSF[0])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[1], _mm_set_ps1(PSF[3])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[2], _mm_set_ps1(PSF[6])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[3], _mm_set_ps1(PSF[1])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[4], _mm_set_ps1(PSF[4])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[5], _mm_set_ps1(PSF[7])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[6], _mm_set_ps1(PSF[2])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[7], _mm_set_ps1(PSF[5])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[8], _mm_set_ps1(PSF[8])));
        

        accumulators = _mm_min_ps(accumulators, _mm_set_ps1(255));
        accumulators = _mm_max_ps(accumulators, _mm_set_ps1(0));

        _mm_store_ps(accumulators_ram, accumulators);

        convR[(i*COLS)+j]=(UINT8)accumulators_ram[0];
        convR[((i+1)*COLS)+j]=(UINT8)accumulators_ram[1];
        convR[((i+2)*COLS)+j]=(UINT8)accumulators_ram[2];
        convR[((i+3)*COLS)+j]=(UINT8)accumulators_ram[3];
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
        
        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[0], _mm_set_ps1(PSF[0])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[1], _mm_set_ps1(PSF[3])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[2], _mm_set_ps1(PSF[6])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[3], _mm_set_ps1(PSF[1])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[4], _mm_set_ps1(PSF[4])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[5], _mm_set_ps1(PSF[7])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[6], _mm_set_ps1(PSF[2])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[7], _mm_set_ps1(PSF[5])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[8], _mm_set_ps1(PSF[8])));
        

        accumulators = _mm_min_ps(accumulators, _mm_set_ps1(255));
        accumulators = _mm_max_ps(accumulators, _mm_set_ps1(0));

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
        
        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[0], _mm_set_ps1(PSF[0])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[1], _mm_set_ps1(PSF[3])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[2], _mm_set_ps1(PSF[6])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[3], _mm_set_ps1(PSF[1])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[4], _mm_set_ps1(PSF[4])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[5], _mm_set_ps1(PSF[7])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[6], _mm_set_ps1(PSF[2])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[7], _mm_set_ps1(PSF[5])));

        accumulators = _mm_add_ps(accumulators, _mm_mul_ps(data[8], _mm_set_ps1(PSF[8])));
        

        accumulators = _mm_min_ps(accumulators, _mm_set_ps1(255));
        accumulators = _mm_max_ps(accumulators, _mm_set_ps1(0));

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