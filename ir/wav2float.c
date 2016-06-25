#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
/*
  gcc wav2float.c -o wav2float  -lsndfile
*/
int main(int argc, char* argv[])
{
  SNDFILE *sf;
  SF_INFO info;
  int num_channels;
  int num, num_items;
  float *buf;
  int f, sr, channels;
  int i, j;
  char* filename;
  char* varname;
  int samples;
  filename = argc > 1 ? argv[1] : "file.wav";
  varname = argc > 2 ? argv[2] : "ir";
  samples = argc > 3 ? atol(argv[3]) : 6144;
  /* Open the WAV file. */
  info.format = 0;
  sf = sf_open(filename, SFM_READ,&info);
  if (sf == NULL){
    printf("Failed to open file %s.\n", filename);
    return -1;
  }
  /* Print some of the info, and figure out how much data to read. */
  f = info.frames;
  sr = info.samplerate;
  channels = info.channels;
  printf("/* wav2float input file: %s", filename);
  printf("frames=%d\n", f);
  printf("samplerate=%d\n", sr);
  printf("format=0x%06x\n", info.format);
  printf("channels=%d\n", channels);
  num_items = samples; // f*c;
  printf("num_items=%d\n",num_items);
  /* Allocate space for the data to be read, then read it. */
  buf = (float*)malloc(num_items*sizeof(float));
  num = sf_read_float(sf,buf,num_items);
  sf_close(sf);
  printf("Read %d items\n*/\n",num);
  /* Write the data to std out. */
  printf("float %s[%d] = {", varname, num);
  for (i = 0; i < num; i += channels){
    for (j = 0; j < channels; ++j)
      if(j == 0)
	printf("%.9g, ",buf[i+j]);
    /* printf("\n"); */
  }
  printf("};");
  return 0;
}
