#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
/*
  compile with:
  gcc wav2float.c -o wav2float  -lsndfile
  run with:
  ./wav2float sample.wav foobar 512 1 > sample.h
  (makes a float array named 'foobar' with the first 512 samples, 1 channel of sample.wav)
*/
int main(int argc, char* argv[])
{
  SNDFILE *sf;
  SF_INFO info;
  int num;
  float *buf;
  int channels;
  int i, j;
  char* filename;
  char* varname;
  int samples;
  filename = argc > 1 ? argv[1] : "file.wav";
  varname = argc > 2 ? argv[2] : "ir";
  samples = argc > 3 ? atol(argv[3]) : 10*1024; // default size 8kB
  channels = argc > 4 ? atol(argv[4]) : 1;
  /* Open the WAV file. */
  info.format = 0;
  sf = sf_open(filename, SFM_READ,&info);
  if (sf == NULL){
    printf("Failed to open file %s.\n", filename);
    return -1;
  }
  /* Print some of the info, and figure out how much data to read. */
  printf("/* wav2float input file: %s\n", filename);
  printf("frames=%d\n", (int)info.frames);
  printf("samplerate=%d\n", info.samplerate);
  printf("format=0x%06x\n", info.format);
  printf("channels=%d\n", info.channels);
  /* samples = samples > info.frames ? info.frames : samples; */
  printf("Writing %d channels, %d samples\n", channels, samples);
  /* Allocate space for the data to be read, then read it. */
  buf = (float*)malloc(samples*sizeof(float));
  num = sf_read_float(sf, buf, samples*info.channels);
  /* sf_close(sf); */
  printf("Read %d items\n*/\n", num);
  /* Write the data to std out */
  printf("float %s[%d][%d] = {\n", varname, channels, samples);
  while(channels--){
    printf("{");
    for (i = 0; i < num; i += info.channels){
      for (j = 0; j < info.channels; ++j)
	if(j == channels)
	  printf("%.9g, ",buf[i+j]);
    }
    printf("},\n");
    /* printf("\n"); */
  }
  printf("};\n");
  return 0;
}
