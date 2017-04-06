#include "StompBox.h"
// #include "errorhandlers.h"
// #include "stm32f7xx_hal.h"

// bool tr1(){
//   return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) != GPIO_PIN_SET;
// }
// bool tr2(){
//   return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10) != GPIO_PIN_SET;
// }
// bool sw1(){
//   return HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_14) != GPIO_PIN_SET;
// }
// bool sw2(){
//   return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) != GPIO_PIN_SET;
// }

class SplashPatch : public Patch {
private:
  float x = 0;
  float y = 0;
  float ratex = 0.8;
  float ratey = 0.8;
public:
  SplashPatch(){
    // debugMessage("ctor");
  }
  void encoderChanged(PatchParameterId pid, int32_t dir, uint16_t samples){
    debugMessage("enc", pid, dir);
    if(pid == PARAMETER_A){
      ratex += dir/20.0f;
    }else{
      ratey -= dir/20.0f;
    }
  }
  void processAudio(AudioBuffer& samples){
    // animate
    x += ratex;
    if(x < 0){
      x = 0.1;
      ratex *= -0.9;
    }else if(x > 58){
      x = 58;
      ratex *= -0.9;
    }
    y += ratey;
    if(y < 0){
      y = 0.1;
      ratey *= -0.9;
    }else if(y > 48){
      y = 48;
      ratey *= -0.9;
    }
    // debugMessage("x/y", x, y);

    // debugMessage("a/b", getParameterValue(PARAMETER_A), getParameterValue(PARAMETER_B));
  }

  void processScreen(ScreenBuffer& screen){
    screen.clear();

    screen.setTextSize(2);
    screen.print(x, y, "Splash");

    screen.setPixel(getParameterValue(PARAMETER_A)*screen.getWidth(), 
		    getParameterValue(PARAMETER_B)*screen.getHeight(), BLACK);

    screen.setTextSize(1);
    // screen.print(20, screen.getHeight()-8, "Rebel Technology");
    screen.print(20, 0, "Rebel Technology");
  }
};
