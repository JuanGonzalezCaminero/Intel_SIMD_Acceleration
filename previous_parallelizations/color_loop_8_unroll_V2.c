

for(j=1; j<COLS-1; j+=unroll_iterations)
{
	//UNROLL 8
    //Initialise the accumulators
    for(z=0; z<unroll_iterations; z++)
    {
        temp_accumulators[z]=0;
    }

    *temp_accumulators += (PSF[0] * (FLOAT)R[((i-1)*COLS)+j-1]);
    *temp_accumulators += (PSF[1] * (FLOAT)R[((i-1)*COLS)+j]);
    *temp_accumulators += (PSF[2] * (FLOAT)R[((i-1)*COLS)+j+1]);

    *temp_accumulators += (PSF[3] * (FLOAT)R[((i)*COLS)+j-1]);
    *temp_accumulators += (PSF[4] * (FLOAT)R[((i)*COLS)+j]);
    *temp_accumulators += (PSF[5] * (FLOAT)R[((i)*COLS)+j+1]);

    *temp_accumulators += (PSF[6] * (FLOAT)R[((i+1)*COLS)+j-1]);
    *temp_accumulators += (PSF[7] * (FLOAT)R[((i+1)*COLS)+j]);
    *temp_accumulators += (PSF[8] * (FLOAT)R[((i+1)*COLS)+j+1]);
    temp_accumulators++;
    //
    *temp_accumulators += (PSF[0] * (FLOAT)R[((i-1)*COLS)+j]);
    *temp_accumulators += (PSF[1] * (FLOAT)R[((i-1)*COLS)+j+1]);
    *temp_accumulators += (PSF[2] * (FLOAT)R[((i-1)*COLS)+j+2]);

    *temp_accumulators += (PSF[3] * (FLOAT)R[((i)*COLS)+j]);
    *temp_accumulators += (PSF[4] * (FLOAT)R[((i)*COLS)+j+1]);
    *temp_accumulators += (PSF[5] * (FLOAT)R[((i)*COLS)+j+2]);

    *temp_accumulators += (PSF[6] * (FLOAT)R[((i+1)*COLS)+j]);
    *temp_accumulators += (PSF[7] * (FLOAT)R[((i+1)*COLS)+j+1]);
    *temp_accumulators += (PSF[8] * (FLOAT)R[((i+1)*COLS)+j+2]);
    temp_accumulators++;
    //
    *temp_accumulators += (PSF[0] * (FLOAT)R[((i-1)*COLS)+j+1]);
    *temp_accumulators += (PSF[1] * (FLOAT)R[((i-1)*COLS)+j+2]);
    *temp_accumulators += (PSF[2] * (FLOAT)R[((i-1)*COLS)+j+3]);

    *temp_accumulators += (PSF[3] * (FLOAT)R[((i)*COLS)+j+1]);
    *temp_accumulators += (PSF[4] * (FLOAT)R[((i)*COLS)+j+2]);
    *temp_accumulators += (PSF[5] * (FLOAT)R[((i)*COLS)+j+3]);

    *temp_accumulators += (PSF[6] * (FLOAT)R[((i+1)*COLS)+j+1]);
    *temp_accumulators += (PSF[7] * (FLOAT)R[((i+1)*COLS)+j+2]);
    *temp_accumulators += (PSF[8] * (FLOAT)R[((i+1)*COLS)+j+3]);
    temp_accumulators++;
    //
    *temp_accumulators += (PSF[0] * (FLOAT)R[((i-1)*COLS)+j+2]);
    *temp_accumulators += (PSF[1] * (FLOAT)R[((i-1)*COLS)+j+3]);
    *temp_accumulators += (PSF[2] * (FLOAT)R[((i-1)*COLS)+j+4]);

    *temp_accumulators += (PSF[3] * (FLOAT)R[((i)*COLS)+j+2]);
    *temp_accumulators += (PSF[4] * (FLOAT)R[((i)*COLS)+j+3]);
    *temp_accumulators += (PSF[5] * (FLOAT)R[((i)*COLS)+j+4]);

    *temp_accumulators += (PSF[6] * (FLOAT)R[((i+1)*COLS)+j+2]);
    *temp_accumulators += (PSF[7] * (FLOAT)R[((i+1)*COLS)+j+3]);
    *temp_accumulators += (PSF[8] * (FLOAT)R[((i+1)*COLS)+j+4]);
    temp_accumulators++;
    //
    *temp_accumulators += (PSF[0] * (FLOAT)R[((i-1)*COLS)+j+3]);
    *temp_accumulators += (PSF[1] * (FLOAT)R[((i-1)*COLS)+j+4]);
    *temp_accumulators += (PSF[2] * (FLOAT)R[((i-1)*COLS)+j+5]);

    *temp_accumulators += (PSF[3] * (FLOAT)R[((i)*COLS)+j+3]);
    *temp_accumulators += (PSF[4] * (FLOAT)R[((i)*COLS)+j+4]);
    *temp_accumulators += (PSF[5] * (FLOAT)R[((i)*COLS)+j+5]);

    *temp_accumulators += (PSF[6] * (FLOAT)R[((i+1)*COLS)+j+3]);
    *temp_accumulators += (PSF[7] * (FLOAT)R[((i+1)*COLS)+j+4]);
    *temp_accumulators += (PSF[8] * (FLOAT)R[((i+1)*COLS)+j+5]);
    temp_accumulators++;
    //
    *temp_accumulators += (PSF[0] * (FLOAT)R[((i-1)*COLS)+j+4]);
    *temp_accumulators += (PSF[1] * (FLOAT)R[((i-1)*COLS)+j+5]);
    *temp_accumulators += (PSF[2] * (FLOAT)R[((i-1)*COLS)+j+6]);

    temp_accumulators += (PSF[3] * (FLOAT)R[((i)*COLS)+j+4]);
    *temp_accumulators += (PSF[4] * (FLOAT)R[((i)*COLS)+j+5]);
    *temp_accumulators += (PSF[5] * (FLOAT)R[((i)*COLS)+j+6]);

    *temp_accumulators += (PSF[6] * (FLOAT)R[((i+1)*COLS)+j+4]);
    *temp_accumulators += (PSF[7] * (FLOAT)R[((i+1)*COLS)+j+5]);
    *temp_accumulators += (PSF[8] * (FLOAT)R[((i+1)*COLS)+j+6]);
    temp_accumulators++;
    //
    *temp_accumulators += (PSF[0] * (FLOAT)R[((i-1)*COLS)+j+5]);
    *temp_accumulators += (PSF[1] * (FLOAT)R[((i-1)*COLS)+j+6]);
    *temp_accumulators += (PSF[2] * (FLOAT)R[((i-1)*COLS)+j+7]);

    *temp_accumulators += (PSF[3] * (FLOAT)R[((i)*COLS)+j+5]);
    *temp_accumulators += (PSF[4] * (FLOAT)R[((i)*COLS)+j+6]);
    *temp_accumulators += (PSF[5] * (FLOAT)R[((i)*COLS)+j+7]);

    *temp_accumulators += (PSF[6] * (FLOAT)R[((i+1)*COLS)+j+5]);
    *temp_accumulators += (PSF[7] * (FLOAT)R[((i+1)*COLS)+j+6]);
    *temp_accumulators += (PSF[8] * (FLOAT)R[((i+1)*COLS)+j+7]);
    temp_accumulators++;
    //
    *temp_accumulators += (PSF[0] * (FLOAT)R[((i-1)*COLS)+j+6]);
    *temp_accumulators += (PSF[1] * (FLOAT)R[((i-1)*COLS)+j+7]);
    *temp_accumulators += (PSF[2] * (FLOAT)R[((i-1)*COLS)+j+8]);

    *temp_accumulators += (PSF[3] * (FLOAT)R[((i)*COLS)+j+6]);
    *temp_accumulators += (PSF[4] * (FLOAT)R[((i)*COLS)+j+7]);
    *temp_accumulators += (PSF[5] * (FLOAT)R[((i)*COLS)+j+8]);

    *temp_accumulators += (PSF[6] * (FLOAT)R[((i+1)*COLS)+j+6]);
    *temp_accumulators += (PSF[7] * (FLOAT)R[((i+1)*COLS)+j+7]);
    *temp_accumulators += (PSF[8] * (FLOAT)R[((i+1)*COLS)+j+8]);
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