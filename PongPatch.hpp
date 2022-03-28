#ifndef __PongPatch_hpp__
#define __PongPatch_hpp__

#include "MonochromeScreenPatch.h"

static constexpr float scaleX = 128;
static constexpr float scaleY = 64;
static constexpr float LEFT_GOAL = 4/scaleX;
static constexpr float RIGHT_GOAL = 124/scaleX;

class Ball {
public:
  float x = 0.5;
  float y = 0.5;
  float speedx = 0;
  float speedy = 0;
  static constexpr float radius = 2 / scaleX;
  void reset(){
    x = 0.5;
    y = 0.5;
    speedx = 0;
    speedy = 0;
  }
  void draw(MonochromeScreenBuffer& screen){
    screen.fillCircle(x*scaleX, y*scaleY, radius*scaleX, WHITE);
  }
  void move(){
    if((y + speedy + radius >= 1) ||
       (y + speedy - radius <= 0)){
      speedy *= -1;
    }
    y += speedy;
    x += speedx;
  }
  void serve(float speed){
    if(x < 0.5){
      x = RIGHT_GOAL - 4*radius;
      speedx = -speed;
    }else{
      x = LEFT_GOAL + 4*radius;
      speedx = speed;
    }
    y = 0.5;
    speedy *= 0.5;
    // speedy = 0.01 * randf()/scaleY;
    // serve_speed += 0.2/scaleX;
  };
};

class Paddle {
public:
  float x = 0;
  float y = 0.5;
  int points = 0;
  static constexpr float width = 3 / scaleX;
  static constexpr float height = 12 / scaleY;
  void draw(MonochromeScreenBuffer& screen){
    screen.drawRectangle(x*scaleX, y*scaleY, width*scaleX, height*scaleY, WHITE);
  }
  void move(float dy){
    y = clamp(dy, 0.0f, 1 - height);
  }
  bool check(Ball& ball){
    if(abs(ball.x - x) <= width/2 + ball.radius){
      // if((ball.y + ball.radius > y) ||
      // 	 (ball.y - ball.radius < y + height)){
      if((ball.y > y) && (ball.y < y + height)){
	ball.speedx *= -1;
	float angle = ball.y - (y + height/2);
	// ball.speedy = clamp(angle / scaleY, -0.001, 0.001);
	ball.speedy = 0.04 * angle;
	return true;
      }
    }
    return false;
    // collision detection
    // if we are touching ball, change ball speed
    // and emit a trigger
    // vertical speed change depends on how/where we hit the ball
  }
  void score(){
    points++;
  }
};

class PongPatch : public MonochromeScreenPatch {
private:
  Ball ball;
  Paddle left;
  Paddle right;
  float serve_speed = 0.0004;
  
public:
  PongPatch(){
    left.x = LEFT_GOAL;
    right.x = RIGHT_GOAL;

    registerParameter(PARAMETER_A, "Pad L");
    registerParameter(PARAMETER_B, "Pad R");
    registerParameter(PARAMETER_C, "Serve");

    registerParameter(PARAMETER_F, "Ball X>");
    registerParameter(PARAMETER_G, "Ball Y>");

    setParameterValue(PARAMETER_A, 0.3);
    setParameterValue(PARAMETER_B, 0.3);
    setParameterValue(PARAMETER_C, 0.5);
  }

  ~PongPatch(){
  }

  void processMidi(MidiMessage msg){
    switch (msg.getStatus()) {
    case NOTE_OFF:
      break;
    case NOTE_ON:
      setParameterValue(PARAMETER_A, (msg.getNote()-20)/80.0); // set basenote parameter
      break;
    }
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(value){
      switch(bid){
      case PUSHBUTTON:
      case BUTTON_1:
	// start one player game
	ball.serve(serve_speed);
	left.points = 0;
	right.points = 0;
	break;
      case BUTTON_2:
	// start two player game
	ball.reset();
	break;
      }
    }
  }

  void processAudio(AudioBuffer& buffer){
    // FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    // FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    left.move(1-getParameterValue(PARAMETER_A));
    right.move(1-getParameterValue(PARAMETER_B));
    if(left.check(ball)){
      setButton(BUTTON_1, true);
    }else if(right.check(ball)){
      setButton(BUTTON_2, true);
    }else{
      setButton(BUTTON_1, false);
      setButton(BUTTON_2, false);
    }          
    ball.move();
    if(ball.x >= RIGHT_GOAL){
      left.score();
      ball.serve(serve_speed);
    }else if(ball.x <= LEFT_GOAL){
      right.score();
      ball.serve(serve_speed);
    }
    setParameterValue(PARAMETER_F, ball.x);
    setParameterValue(PARAMETER_G, 1 - ball.y);
    serve_speed = getParameterValue(PARAMETER_C)*0.0004*5+0.00001;
  }

  void processScreen(MonochromeScreenBuffer& screen){
    screen.clear();
    ball.draw(screen);
    left.draw(screen);
    right.draw(screen);
    screen.setTextSize(1);
    screen.setCursor(12, 8);
    screen.print(left.points);
    screen.setCursor(112, 8);
    screen.print(right.points);
  }
};

#endif   // __PongPatch_hpp__
