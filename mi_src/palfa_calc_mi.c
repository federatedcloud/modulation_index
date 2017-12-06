#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mock_spec.h"

#define BYTES_SAMP 1
#define MAX_BYTES 35000000

void dedisp_smooth(unsigned char *data, unsigned int *delays, int nchan, float t_wid, double *spec, double mean);
void calc_chandelays(unsigned int *delays, float dm, double freq_lo, double bw, int nchan, double tsamp);

int main(int argc, char *argv[])
{
    FILE *fin, *fraw, *fout;
    float dm, snr, downfact;
    double stime, rms, dmean; 
    unsigned int ii, snum, block, loc_nchan=0;
    float dm_prev=-1.0;
    unsigned int start_byte, num_bytes, max_dm_delay=0;
    unsigned int nread,nbytes2read,nscanned=0;
    double Ibar,Ibar2,m_I=0.0;

    //Define the number of channels excluding edges
    loc_nchan=right_chan-left_chan;

    printf("Allocating arrays\n");
    unsigned char *data = (unsigned char *) malloc(sizeof(char) * MAX_BYTES + 1);
    unsigned int *delays = (unsigned int*) malloc(sizeof(unsigned int) * (nchan + 1));
    double *spectrum = (double *) malloc(sizeof(double) * (nchan + 1) );

    //OPEN FILES
    fprintf(stderr, "Opening %s\n", argv[1]);
    fin=fopen(argv[1], "r");
    if(fin==0) fprintf(stderr, "File %s could not be opened", argv[1]);


    fprintf(stderr, "Opening %s\n", argv[2]);
    fraw=fopen(argv[2], "r");
    if(fraw==0) fprintf(stderr, "File %s could not be opened", argv[2]);

    fprintf(stderr, "Opening %s\n", argv[3]);
    fout=fopen(argv[3], "w");
    if(fout==0) fprintf(stderr, "File %s could not be opened", argv[3]);

    while(fscanf(fin, "%f %f %lf %u %f %u %lf %lf\n", &dm, &snr, &stime, &snum, &downfact, &block, &rms, &dmean)==8)
    {
        //RESET VARIABLES
        Ibar=0.0; Ibar2=0.0;
        dmean/=nchan; 

//        fprintf(stderr, "%lf ", dmean);
        if(dm != dm_prev)
        {
            //Get delays
            calc_chandelays(delays, dm, freq_lo, bw, nchan, tsamp);
//            for(ii=0; ii<nchan; ii++)  printf("%u ", delays[ii]);

            //Also while I'm at it, set spectrum to zero:
            for(ii=0; ii<nchan; ii++)  spectrum[ii]=0.0;
            max_dm_delay=delays[0];    //OK?
//            printf("Max DM delay: %u\n", max_dm_delay);
        }

        start_byte=nchan*(snum - ((int) downfact)/2)*BYTES_SAMP;  //Consider edges

        //MOVE TO START BYTE
        fseek(fraw, start_byte, SEEK_SET);

        //READ IN DATA
        nbytes2read=BYTES_SAMP*nchan*(max_dm_delay + (int) (downfact));
        nread=fread(data, sizeof(char), nbytes2read, fraw);
//        printf("nread: %d\n", nread);
        if(nread != nbytes2read) fprintf(stderr, "Number of bytes read incorrect!\n");

        //SMOOTH and DEDISPERE DATA
//        printf("Dedisp and smooth\n");
//        for(ii=0; ii<nread; ii++) data[ii]-dmean;
        dedisp_smooth(data, delays, nchan, downfact, spectrum, dmean);

        //CALC Ibar and I2bar
        for(ii=left_chan; ii<right_chan; ii++)
        {
//            Ibar+=(spectrum[ii]-dmean);
//            Ibar2+=(spectrum[ii]-dmean)*(spectrum[ii]-dmean);
            Ibar+=spectrum[ii];
            Ibar2+=spectrum[ii]*spectrum[ii];
        }
        Ibar/=loc_nchan;
        Ibar2/=loc_nchan;

        m_I=sqrt(Ibar2-Ibar*Ibar)/Ibar;

        //SEND RESULTS TO FILE
        fprintf(fout, "%f   %f   %lf   %u   %f   %lf   %lf   %lf\n", dm, snr, stime, snum, downfact, Ibar, Ibar2, m_I);
        dm_prev=dm;

//        fprintf(stdout, "%lf %u\n", spectrum[300], data[200]);
    }
    //FREE MEMORY
    free(data); free(spectrum); free(delays);

    //CLOSE FILES
    fclose(fin);fclose(fraw);fclose(fout);

    return 0;
}


void dedisp_smooth(unsigned char *data, unsigned int *delays, int nchan, float t_wid, double *spec, double mean)
{
    double tmp=0;
    int ii,jj,n=0;
    
    for(jj=0; jj<nchan; jj++)
    {
        tmp=0.0;
        for(ii=0; ii<t_wid; ii++)
        {
            n=nchan*delays[jj] + jj + nchan*ii;
//            printf("%d %d %u %d \n", ii, jj, delays[jj], n);
//             printf("%u %lf \n", data[n], mean);
            tmp+=( (double) data[n]- mean);
//            if(jj==nchan-1 && ii==t_wid-1) printf("%d %d %u %d \n", ii, jj, delays[jj], n);
        }
        spec[jj]=tmp/t_wid;
    }
} 

void calc_chandelays(unsigned int *delays, float dm, double freq_lo, double bw, int nchan, double tsamp)
{
    /*
    freq_lo = center frequency of the lowest channel
    bw = bandwidth from edge to edge
    tsamp = sample time in msec
    */
    double freq_hi=0;
    int ii=0;
    double tdelay,floc,dnu,bw_corr=0.0;

    dnu=bw/nchan;             //Channel resolution
    freq_hi=freq_lo+bw-dnu;    //Center freq of highest bin
    bw_corr=bw-dnu;           //Bandwidth from center of top and bottom bins

    for(ii=0; ii<nchan; ii++)
    {
        floc=freq_lo + dnu*ii;
        tdelay=4.15e6*dm*(1.0/(floc*floc) - 1.0/(freq_hi*freq_hi));
        delays[ii] = (unsigned int) (tdelay/tsamp + 0.5);
    }
}
