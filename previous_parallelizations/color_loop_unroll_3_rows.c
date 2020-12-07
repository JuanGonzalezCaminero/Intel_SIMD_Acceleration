

for(i=1; i+3<ROWS-1; i+=3)
{
    // Skip first and last column, no neighbors to convolve with
    for(j=1; j<COLS-1; j++)
    {
           
        temp=0;
        temp2=0;
        temp3=0;

        temp += (PSF[0] * (FLOAT)R[((i-1)*320)+j-1]);
        temp += (PSF[1] * (FLOAT)R[((i-1)*320)+j]);
        temp += (PSF[2] * (FLOAT)R[((i-1)*320)+j+1]);

        temp += (PSF[3] * (FLOAT)R[((i)*320)+j-1]);
        temp += (PSF[4] * (FLOAT)R[((i)*320)+j]);
        temp += (PSF[5] * (FLOAT)R[((i)*320)+j+1]);
        temp2 += (PSF[0] * (FLOAT)R[((i)*320)+j-1]);
        temp2 += (PSF[1] * (FLOAT)R[((i)*320)+j]);
        temp2 += (PSF[2] * (FLOAT)R[((i)*320)+j+1]);

        temp += (PSF[6] * (FLOAT)R[((i+1)*320)+j-1]);
        temp += (PSF[7] * (FLOAT)R[((i+1)*320)+j]);
        temp += (PSF[8] * (FLOAT)R[((i+1)*320)+j+1]);
        temp2 += (PSF[3] * (FLOAT)R[((i+1)*320)+j-1]);
        temp2 += (PSF[4] * (FLOAT)R[((i+1)*320)+j]);
        temp2 += (PSF[5] * (FLOAT)R[((i+1)*320)+j+1]);
        temp3 += (PSF[0] * (FLOAT)R[((i+1)*320)+j-1]);
        temp3 += (PSF[1] * (FLOAT)R[((i+1)*320)+j]);
        temp3 += (PSF[2] * (FLOAT)R[((i+1)*320)+j+1]);

        temp2 += (PSF[6] * (FLOAT)R[((i+2)*320)+j-1]);
        temp2 += (PSF[7] * (FLOAT)R[((i+2)*320)+j]);
        temp2 += (PSF[8] * (FLOAT)R[((i+2)*320)+j+1]);
        temp3 += (PSF[3] * (FLOAT)R[((i+2)*320)+j-1]);
        temp3 += (PSF[4] * (FLOAT)R[((i+2)*320)+j]);
        temp3 += (PSF[5] * (FLOAT)R[((i+2)*320)+j+1]);

        temp3 += (PSF[6] * (FLOAT)R[((i+3)*320)+j-1]);
        temp3 += (PSF[7] * (FLOAT)R[((i+3)*320)+j]);
        temp3 += (PSF[8] * (FLOAT)R[((i+3)*320)+j+1]);

        if(temp<0.0) temp=0.0;
        if(temp>255.0) temp=255.0;
        convR[(i*320)+j]=(UINT8)temp;

        if(temp2<0.0) temp2=0.0;
        if(temp2>255.0) temp2=255.0;
        convR[((i+1)*320)+j]=(UINT8)temp2;

        if(temp3<0.0) temp3=0.0;
        if(temp3>255.0) temp3=255.0;
        convR[((i+2)*320)+j]=(UINT8)temp3;
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