
uint16_t calculate(uint8_t index, uint32_t t){
  uint16_t out;
  switch(index){
  case 1:
// trivial minimum: plain sawtooth
    out = t;
  break;
  case 2:
// minimal sierpinski harmony
    out = t&t>>8;
  break;
  case 3:
// "the 42 melody", separately discovered by several people on irc etc
    out = t*(42&t>>10);
  break;
  case 4:
// danharaj 2011-10-03 http://www.reddit.com/r/programming/comments/kyj77/algorithmic_symphonies_from_one_line_of_code_how/ "fractal trees", 216's version
    out = t|t%255|t%257;
  break;
  case 5:
// droid 2011-10-05 http://pouet.net/topic.php?which=8357&page=10
    out = t>>6&1?t>>5:-t>>4;
  break;
  case 6:
// Niklas_Roy 2011-10-14 http://countercomplex.blogspot.com/2011/10/algorithmic-symphonies-from-one-line-of.html
    out = t*(t>>9|t>>13)&16;
  break;
  case 7:
// krcko 2011-10-04 http://rafforum.rs/index.php/topic,123.0.html
    out = (t&t>>12)*(t>>4|t>>8);
  break;
  case 8:
// viznut 2011-10-10 http://www.youtube.com/watch?v=tCRPUv8V22o
    out = (t*5&t>>7)|(t*3&t>>10);
  break;
  case 9:
// tejeez 2011-09-18 http://www.youtube.com/watch?v=GtQdIYUtAHg
    out = (t*(t>>5|t>>8))>>(t>>16);
  break;
  case 10:
// miiro 2011-09-30 http://www.youtube.com/watch?v=qlrs2Vorw2Y
    out = t*5&(t>>7)|t*3&(t*4>>10);
  break;
  case 11:
// robert 2011-10-11 http://countercomplex.blogspot.com/2011/10/algorithmic-symphonies-from-one-line-of.html
    out = (t>>13|t%24)&(t>>7|t%19);
  break;
  case 12:
// Niklas_Roy 2011-10-14 http://countercomplex.blogspot.com/2011/10/algorithmic-symphonies-from-one-line-of.html
    out = (t*((t>>9|t>>13)&15))&129;
  break;
  case 13:
// viznut 2011-10-10 http://www.youtube.com/watch?v=tCRPUv8V22o  
    out = (t&t%255)-(t*3&t>>13&t>>6);
  break;
  case 14:
// krcko 2011-10-04 http://rafforum.rs/index.php/topic,123.0.html
    out = (t&t>>12)*(t>>4|t>>8)^t>>6;
  break;
  case 15:
// blueberry 2011-10-05 http://pouet.net/topic.php?which=8357&page=12 11kHz
    out = t*(((t>>9)^((t>>9)-1)^1)%13);
  break;
  case 16:
// rrola 2011-10-04 http://pouet.net/topic.php?which=8357&page=9 optimized by ryg
    out = t*(0xCA98>>(t>>9&14)&15)|t>>8;
  break;
  case 17:
// tonic 2011-10-01 http://pouet.net/topic.php?which=8357&page=5 "mr. arpeggiator playing a solo"
    out = (t/8)>>(t>>9)*t/((t>>14&3)+4);
  break;
  case 18:
// FreeFull 2011-10-12 http://countercomplex.blogspot.com/2011/10/algorithmic-symphonies-from-one-line-of.html
    out = (~t/100|(t*3))^(t*3&(t>>5))&t;
  break;
  case 19:
// red- 2011-09-30 http://www.youtube.com/watch?v=qlrs2Vorw2Y
    out = (t|(t>>9|t>>7))*t&(t>>11|t>>9);
  break;
  case 20:
// harism 2011-10-09 http://0xa.kuri.mu/2011/10/09/bitop-videos/
    out = ((t>>1%128)+20)*3*t>>14*t>>18 ;
  break;
  case 21:
// droid 2011-10-04 http://pouet.net/topic.php?which=8357&page=9
/*     out = t&(sin(t&t&3)*t>>5)/(t>>3&t>>6); */
  break;
  case 22:
// viznut 2011-09-18 http://www.youtube.com/watch?v=GtQdIYUtAHg
    out = t*(((t>>12)|(t>>8))&(63&(t>>4)));
  break;
  case 23:
// visy 2011-09-18 http://www.youtube.com/watch?v=GtQdIYUtAHg
    out = t*(((t>>9)|(t>>13))&(25&(t>>6)));
  break;
  case 24:
// 216 2011-10-10 http://www.youtube.com/watch?v=tCRPUv8V22o
    out = t*(t^t+(t>>15|1)^(t-1280^t)>>10);
  break;
  case 25:
// tejeez 2011-09-18 http://www.youtube.com/watch?v=GtQdIYUtAHg
    out = t*(((t>>11)&(t>>8))&(123&(t>>3)));
  break;
  case 26:
// viznut 2011-09-30 http://www.youtube.com/watch?v=qlrs2Vorw2Y (xpansive+varjohukka)
    out = (t>>7|t|t>>6)*10+4*(t&t>>13|t>>6);
  break;
  case 27:
// stephth 2011-10-03 http://news.ycombinator.com/item?id=3063359
    out = (t*9&t>>4|t*5&t>>7|t*3&t/1024)-1;
  break;
  case 28:
// visy 2011-09-18 http://www.youtube.com/watch?v=GtQdIYUtAHg "Space Invaders vs Pong"
    out = t*(t>>((t>>9)|(t>>8))&(63&(t>>4)));
  break;
  case 29:
// viznut 2011-09-30 http://www.youtube.com/watch?v=qlrs2Vorw2Y
    out = (t>>6|t|t>>(t>>16))*10+((t>>11)&7);
  break;
  case 30:
// yumeji 2011-10-04 http://pouet.net/topic.php?which=8357&page=9
    out = (t>>1)*(0xbad2dea1>>(t>>13)&3)|t>>5;
  break;
  case 31:
// ryg 2011-10-04 http://pouet.net/topic.php?which=8357&page=8
    out = (t>>4)*(13&(0x8898a989>>(t>>11&30)));
  break;
  case 32:
// marmakoide 2011-10-04 http://pouet.net/topic.php?which=8357&page=8
    out = (t>>(t&7))|(t<<(t&42))|(t>>7)|(t<<5);
  break;
  case 33:
// robert 2011-10-11 http://countercomplex.blogspot.com/2011/10/algorithmic-symphonies-from-one-line-of.html
    out = (t>>7|t%45)&(t>>8|t%35)&(t>>11|t%20);
  break;
  case 34:
// lucasvb 2011-10-03 http://www.reddit.com/r/programming/comments/kyj77/algorithmic_symphonies_from_one_line_of_code_how/
    out = (t>>6|t<<1)+(t>>5|t<<3|t>>3)|t>>2|t<<1;
  break;
  case 35:
// bear @ celephais
    out = t+(t&t^t>>6)-t*((t>>9)&(t%16?2:6)&t>>9);
  break;
  case 36:
// xpansive 2011-09-29 http://pouet.net/topic.php?which=8357&page=2 "Lost in Space"
    out = ((t*(t>>8|t>>9)&46&t>>8))^(t&t>>13|t>>6);
  break;
  case 37:
// marmakoide 2011-10-03 http://pouet.net/topic.php?which=8357&page=7 "Lemmings March"
    out = (t>>5)|(t<<4)|((t&1023)^1981)|((t-67)>>4);
  break;
  case 38:
// droid 2011-10-04 http://pouet.net/topic.php?which=8357&page=9
    out = t>>4|t&(t>>5)/(t>>7-(t>>15)&-t>>7-(t>>15));
  break;
  case 39:
// rez 2011-10-03 http://pouet.net/topic.php?which=8357&page=7
    out = t*(t/256)-t*(t/255)+t*(t>>5|t>>6|t<<2&t>>1);
  break;
  case 40:
// viznut 2011-10-06 #countercomplex "moon scanner generalization", based on jaffa's formula
    out = ((t>>5&t)-(t>>5)+(t>>5&t))+(t*((t>>14)&14));
  break;
  case 41:
// viznut 2011-10-04 http://pouet.net/topic.php?which=8357&page=9
    out = (t*((3+(1^t>>10&5))*(5+(3&t>>14))))>>(t>>8&3);
  break;
  case 42:
// bst 2011-10-10 http://www.youtube.com/watch?v=tCRPUv8V22o
    out = (int)(t/1e7*t*t+t)%127|t>>4|t>>5|t%127+(t>>16)|t;
  break;
  case 43:
// tangent128 2011-10-09 http://0xa.kuri.mu/2011/10/09/bitop-videos/
    out = t*(((t>>9)&10)|((t>>11)&24)^((t>>10)&15&(t>>15)));
  break;
  case 44:
// tejeez 2011-10-05 #countercomplex
    out = (~t>>2)*((127&t*(7&t>>10))<(245&t*(2+(5&t>>14))));
  break;
  case 45:
// lokori 2011-10-04 #suomiscene
    out = (t+(t>>2)|(t>>5))+(t>>3)|((t>>13)|(t>>7)|(t>>11));
  break;
  case 46:
// visy 2011-09-18 http://www.youtube.com/watch?v=GtQdIYUtAHg
    out = t*(t>>8*((t>>15)|(t>>8))&(20|(t>>19)*5>>t|(t>>3)));
  break;
  case 47:
// Aaron_Krister_Johnson 2011-10-14 http://countercomplex.blogspot.com/2011/10/algorithmic-symphonies-from-one-line-of.html
    out = (t>>4)|(t%10)|(((t%101)|(t>>14))&((t>>7)|(t*t%17)));
  break;
  case 48:
// jounim 2011-10-04 #suomiscene
    out = ((t&((t>>5)))+(t|((t>>7))))&(t>>6)|(t>>5)&(t*(t>>7));
  break;
  case 49:
// spikey 2011-10-04 #suomiscene based on jounim's formula
    out = ((t&((t>>23)))+(t|(t>>2)))&(t>>3)|(t>>5)&(t*(t>>7));
  break;
  case 50:
// akx 2011-10-05 http://twitter.com/#!/akx
    out = (((((t*((t>>9|t>>13)&15))&255/15)*9)%(1<<7))<<2)%6<<4;
  break;
  case 51:
// bst 2011-10-05 http://pouet.net/topic.php?which=8357&page=10
    out = ((t%42)*(t>>4)|(0x15483113)-(t>>4))/(t>>16)^(t|(t>>4));
  break;
  case 52:
// skurk 2011-10-04 http://pouet.net/topic.php?which=8357&page=8
    out = t*(t>>((t&4096)?((t*t)/4096):(t/4096)))|(t<<(t/256))|(t>>4);
  break;
  case 53:
// skurk+raer 2011-09-30 http://www.youtube.com/watch?v=qlrs2Vorw2Y
    out = ((t&4096)?((t*(t^t%255)|(t>>4))>>1):(t>>3)|((t&8192)?t<<2:t));
  break;
  case 54:
// yumeji 2011-10-06 http://pouet.net/topic.php?which=8357&page=12 "badbeats & safe"
    out = t*((0xbadbea75>>((t>>12)&30)&3)*0.25*(0x5afe5>>((t>>16)&28)&3));
  break;
  case 55:
// bst 2011-10-11 http://pouet.net/topic.php?which=8357&page=18
    out = t>>16|((t>>4)%16)|((t>>4)%192)|(t*t%64)|(t*t%96)|(t>>16)*(t|t>>5);
  break;
  case 56:
// bear @ celephais
    out = t>>6^t&37|t+(t^t>>11)-t*((t%24?2:6)&t>>11)^t<<1&(t&598?t>>4:t>>10) ;
  break;
  case 57:
// kb 2011-10-04 http://pouet.net/topic.php?which=8357&page=8 44kHz
    out = ((t/2*(15&(0x234568a0>>(t>>8&28))))|t/2>>(t>>11)^t>>12)+(t/16&t&24);
  break;
  case 58:
// bst 2011-10-05 http://pouet.net/topic.php?which=8357&page=12
    out = (t>>5)|(t>>4)|((t%42)*(t>>4)|(0x15483113)-(t>>4))/(t>>16)^(t|(t>>4));
  break;
  case 59:
// tejeez 2011-09-18 http://www.youtube.com/watch?v=GtQdIYUtAHg
    out = (-t&4095)*(255&t*(t&(t>>13))>>12)+(127&t*(234&t>>8&t>>3)>>(3&t>>14));
  break;
  case 60:
// ultrageranium 2011-10-12 http://0xa.kuri.mu/2011/10/09/bitop-videos/
    out = (t*t/256)&(t>>((t/1024)%16))^t%64*(0xC0D3DE4D69LL>>(t>>9&30)&t%32)*t>>18;
  break;
  case 61:
// visy 2011-10-06 http://pouet.net/topic.php?which=8357&page=13
    out = (t%25-(t>>2|t*15|t%227)-t>>3)|((t>>5)&(t<<5)*1663|(t>>3)%1544)/(t%17|t%2048);
  break;
  case 62:
// ryg 2011-10-10 http://www.youtube.com/watch?v=tCRPUv8V22o 44.1 kHz
    out = ((t*("36364689"[t>>13&7]&15))/12&128)+(((((t>>12)^(t>>12)-2)%11*t)/4|t>>13)&127);
  break;
  case 63:
// Ola 2011-10-11 http://countercomplex.blogspot.com/2011/10/algorithmic-symphonies-from-one-line-of.html
    out = ((1-(((t+10)>>((t>>9)&((t>>14))))&(t>>4&-2)))*2)*(((t>>10)^((t+((t>>6)&127))>>10))&1)*32+128;
  break;
  case 64:
// raer 2011-10-07 http://pouet.net/topic.php?which=8357&page=16 stereo 11kHz
/*  L: ((t&4096)?((t*(t^t%255)|(t>>4))>>1):(t>>3)|((t&8192)?t<<2:t)) R: t*(((t>>9)^((t>>9)-1)^1)%13) */
    out = ((t&4096)?((t*(t^t%255)|(t>>4))>>1):(t>>3)|((t&8192)?t<<2:t));
  break;
  case 65:
// ryg 2011-10-04 http://pouet.net/topic.php?which=8357&page=8
    out = ((t>>4)*(13&(0x8898a989>>(t>>11&30)))&255)+((((t>>9|(t>>2)|t>>8)*10+4*((t>>2)&t>>15|t>>8))&255)>>1);
    break;
/*   case 66: */
/* /\* mu6k http://www.youtube.com/watch?v=tCRPUv8V22o 32.0 kHz *\/ */
/*     out = (3e3/(y=t&16383)&1)*35 +(x=t*"6689"[t>>16&3]/24&127)*y/4e4 +((t>>8^t>>10|t>>14|x)&63); */
/*   break; */
/*   case 67: */
/* /\* droid 2011-10-10 http://www.youtube.com/watch?v=tCRPUv8V22o *\/ */
/*     t>>4|t&DIV((t>>5),(t>>7-(t>>15)&-t>>7-(t>>15))); */
/*   break; */
/*   case 68: */
/* /\* pyryp 2011-09-30 http://www.youtube.com/watch?v=qlrs2Vorw2Y *\/ */
/*     out = v=(v>>1)+(v>>4)+t*(((t>>16)|(t>>6))&(69&(t>>9))); */
/*   break; */
  }
  return out;
}
