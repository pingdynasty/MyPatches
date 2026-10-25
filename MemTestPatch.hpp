#include "Patch.h"

// #define TESTSIZE ((size_t)(63.9*1024*1024))
#define TESTSIZE ((size_t)(7.9*1024*1024))
#define BLOCKSIZE (1*1024)

class MemTestPatch : public Patch {
private:
  uint8_t* databuffer;
  uint32_t index;
  int errors;
public:
  MemTestPatch(){
    databuffer = new uint8_t[TESTSIZE];
    index = 0;
    databuffer[index] = index;
    errors = 0;
  }

  void processAudio(AudioBuffer &buffer) {
    for(int i=0; i<BLOCKSIZE; ++i){

      if(index == 0){
	debugMessage("errors/size", errors, (int)TESTSIZE);
      }
      if(databuffer[index] != (index & 0xff)){
	debugMessage("fail", index, databuffer[index]);
	errors++;
      }
      if(++index == TESTSIZE)
	index = 0;
      databuffer[index] = (index & 0xff);
    }

  }
};
