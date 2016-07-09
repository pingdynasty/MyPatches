#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sndfile.h>

#include "ResourceHeader.h"

/*
  gcc wav2res.c -o wav2res  -lsndfile
*/
int main(int argc, char* argv[]){
  SNDFILE *sf;
  SF_INFO info;
  int num_channels;
  int num, num_items;
  float *buf;
  int channels;
  int i, j;
  char* inputfilename;
  char* outputfilename;
  char* resourcename;
  int samples;
  inputfilename = argc > 1 ? argv[1] : "file.wav";
  outputfilename = argc > 2 ? argv[2] : "file.bin";
  resourcename = argc > 3 ? argv[3] : "sample";
  samples = argc > 4 ? atol(argv[4]) : 8*1024; // default size 8kB
  channels = argc > 5 ? atol(argv[5]) : 1;
  /* Open the WAV file. */
  info.format = 0;
  sf = sf_open(inputfilename, SFM_READ,&info);
  if (sf == NULL){
    printf("Failed to open input file %s.\n", inputfilename);
    return -1;
  }
  FILE *outfile;
  outfile = fopen(outputfilename, "wb");
  if (!outfile){
    printf("Failed to open output file %s.\n", outputfilename);
    return -1;
  }

  // prepare and write header
  struct ResourceHeader header;
  header.magic = 0xDADADEED;
  header.size = samples*channels*sizeof(float);
  strncpy(header.name, resourcename, sizeof(header.name)-1);
  header.name[sizeof(header.name)-1] = '\0';
  fwrite(&header, 1, sizeof(header), outfile);

  /* Print some of the info, and figure out how much data to read. */
  printf("/* wav2res input file: %s\n", inputfilename);
  printf("frames=%d\n", (int)info.frames);
  printf("samplerate=%d\n", info.samplerate);
  printf("format=0x%06x\n", info.format);
  printf("channels=%d\n", info.channels);
  printf("Writing %d channels, %d samples\n", channels, samples);
  /* Allocate space for the data to be read, then read it. */
  buf = (float*)malloc(samples*info.channels*sizeof(float));
  num = sf_read_float(sf, buf, samples*info.channels);
  sf_close(sf);
  printf("Read %d items\n*/\n", num);

  while(channels--){
    for (i = 0; i < num; i += info.channels){
      for (j = 0; j < info.channels; ++j)
	if(j == channels){
	  uint8_t* bits = (uint8_t*)(buf+i+j*info.channels);
	  fwrite(bits, 1, 4, outfile);
	  /* fwrite(bits+3, 1, 1, outfile); */
	  /* fwrite(bits+2, 1, 1, outfile); */
	  /* fwrite(bits+1, 1, 1, outfile); */
	  /* fwrite(bits+0, 1, 1, outfile); */
	}
    }
    printf("},\n");
    /* printf("\n"); */
  }
  fclose(outfile);
  return 0;
}
