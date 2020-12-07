//RED
// Skip first and last row, no neighbors to convolve with
for(i=1; i<ROWS-1; i++)
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