for(j=1; j<COLS-1; j+=unroll_iterations)
{
	//For each pixel, take all neighbours, multiply with the kernel, and 
    //accumulate the results

    //Initialise the accumulators
    for(z=0; z<unroll_iterations; z++)
    {
        temp_accumulators[z]=0;
    }

    //Upper row
    for(pixel=j; pixel-j<unroll_iterations && pixel<COLS-1; pixel++)
    {
        //printf("%d\n", pixel);
        *temp_accumulators += (PSF[0] * (FLOAT)R[((i-1)*COLS)+pixel-1]);
        *temp_accumulators += (PSF[1] * (FLOAT)R[((i-1)*COLS)+pixel]);
        *temp_accumulators += (PSF[2] * (FLOAT)R[((i-1)*COLS)+pixel+1]);
        temp_accumulators++;
    }
    temp_accumulators = temp_accumulators_base;

    //Same row
    for(pixel=j; pixel-j<unroll_iterations && pixel<COLS-1; pixel++)
    {
        *temp_accumulators += (PSF[3] * (FLOAT)R[((i)*COLS)+pixel-1]);
        *temp_accumulators += (PSF[4] * (FLOAT)R[((i)*COLS)+pixel]);
        *temp_accumulators += (PSF[5] * (FLOAT)R[((i)*COLS)+pixel+1]);
        temp_accumulators++;
    }
    temp_accumulators = temp_accumulators_base;

    //Lower row
    for(pixel=j; pixel-j<unroll_iterations && pixel<COLS-1; pixel++)
    {
        *temp_accumulators += (PSF[6] * (FLOAT)R[((i+1)*COLS)+pixel-1]);
        *temp_accumulators += (PSF[7] * (FLOAT)R[((i+1)*COLS)+pixel]);
        *temp_accumulators += (PSF[8] * (FLOAT)R[((i+1)*COLS)+pixel+1]);
        temp_accumulators++;
    }
    temp_accumulators = temp_accumulators_base;
    

    for(pixel=j; pixel-j<unroll_iterations && pixel<COLS-1; pixel++)
    {
        if(*temp_accumulators<0.0) *temp_accumulators=0.0;
        if(*temp_accumulators>255.0) *temp_accumulators=255.0;

        convR[(i*COLS)+pixel]=(UINT8)*temp_accumulators;
        temp_accumulators++;
    }
    temp_accumulators = temp_accumulators_base;
}