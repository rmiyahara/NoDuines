//This section is for user inputted data to make the code personalized to the particular controller
 
//shield drop
#define sw_notch_x_value -.7875
#define sw_notch_y_value -.6000
#define se_notch_x_value  .7625
#define se_notch_y_value -.6375
//record southwest and southeast notch values
//to switch to dolphin mode hold dpad right for 5 seconds
//to return to a stock controller hold dpad left for 10 seconds
 

#include "Nintendo.h"
CGamecubeController controller(2); //sets D2 on arduino to read data from controller
CGamecubeConsole console(3);       //sets D3 on arduino to write data to console
Gamecube_Report_t gcc;             //structure for controller state
bool shield, tilt, dolphin = 0, off = 0;
byte axm, aym, cxm, cym, cycles;
char ax, ay, cx, cy, buf;
float swang, seang;
word mode, toggle;
unsigned long n;
int multishine_counter, ledgedashl_counter, ledgedashr_counter, double_laser = 0;
unsigned long delta, last = 0;
 
void convertinputs(){
  perfectangles(); //reduces deadzone of cardinals and gives steepest/shallowest angles when on or near the gate
  maxvectors();    //snaps sufficiently high cardinal inputs to vectors of 1.0 magnitude of analog stick and c stick
  shielddrops();   //allows shield drops down and gives a 6 degree range of shield dropping centered on SW and SE gates
  backdash();      //fixes dashback by imposing a 1 frame buffer upon tilt turn values
  dolphinfix();    //ensures close to 0 values are reported as 0 on the sticks to fix dolphin calibration and allows user to switch to dolphin mode for backdash
  multishine();
}
 
void perfectangles(){
  if(axm>75){gcc.xAxis = (ax>0)?204:52; if(aym<23) gcc.yAxis = (ay>0)?151:105;}
  if(aym>75){gcc.yAxis = (ay>0)?204:52; if(axm<23) gcc.xAxis = (ax>0)?151:105;}
}
 
void maxvectors(){
  if(axm>75&&aym< 9){gcc.xAxis  = (ax>0)?255:1; gcc.yAxis  = 128;}
  if(aym>75&&axm< 9){gcc.yAxis  = (ay>0)?255:1; gcc.xAxis  = 128;}
  if(cxm>75&&cym<23){gcc.cxAxis = (cx>0)?255:1; gcc.cyAxis = 128;}
  if(cym>75&&cxm<23){gcc.cyAxis = (cy>0)?255:1; gcc.cxAxis = 128;}
}


void shielddrops(){
  shield = gcc.l||gcc.r||gcc.left>74||gcc.right>74||gcc.z;
  if(shield){
    if(ay<0&&mag(ax,ay)>75){
      if(ax<0&&abs(ang(axm,aym)-swang)<4){gcc.yAxis = 73; gcc.xAxis =  73;}
      if(ax>0&&abs(ang(axm,aym)-seang)<4){gcc.yAxis = 73; gcc.xAxis = 183;}
    }else if(abs(ay+39)<17&&axm<23) gcc.yAxis = 73;
  }
}
 
void backdash(){
  if(aym<23){
    if(axm<23)buf = cycles;
    if(buf>0){buf--; if(axm<64) gcc.xAxis = 128+ax*(axm<23);}
  }else buf = 0;
}
 
void dolphinfix(){
  if(axm<5&&aym<5){gcc.xAxis  = 128; gcc.yAxis  = 128;}
  if(cxm<5&&cym<5){gcc.cxAxis = 128; gcc.cyAxis = 128;}
  if(gcc.dright&&mode<2500) dolphin = dolphin||(mode++>2000);
  else mode = 0;
  cycles = 3 + (6*dolphin);
}

void multishine() {
  if (gcc.ddown || multishine_counter) {
    if (multishine_counter < (8)) {
      gcc.yAxis = -255;
      gcc.b = true;
      Serial.println("Shine");
    } else if (multishine_counter < (12)) {
      gcc.y = true;
      Serial.println("Jump");
    } else {
      multishine_counter = 0 - delta;
    }
    multishine_counter += delta;
  }
}
 
void ledgedashl() {
  if (gcc.left || ledgedashl_counter) {
    if (ledgedashl_counter < 8) {
      gcc.yAxis = -255;
      Serial.println("Drop");
    } else if (ledgedashl_counter < 10) {
      gcc.yAxis = 0;
      gcc.y = true;
      Serial.println("Jump");
    } else if (ledgedashl_counter < 18) {
      gcc.yAxis = -128;
      gcc.xAxis = -128;
      gcc.r = true;
      Serial.println("Waveland");
    } else {
      ledgedashl_counter = 0 - delta;
    }
    ledgedashl_counter += delta;
  }
}

float ang(float xval, float yval){return atan(yval/xval)*57.2958;} //function to return angle in degrees when given x and y components
float mag(char  xval, char  yval){return sqrt(sq(xval)+sq(yval));} //function to return vector magnitude when given x and y components
 
void setup(){
  gcc.origin=0; gcc.errlatch=0; gcc.high1=0; gcc.errstat=0;  //init values
  swang = ang(abs(sw_notch_x_value), abs(sw_notch_y_value)); //calculates angle of SW gate based on user inputted data
  seang = ang(abs(se_notch_x_value), abs(se_notch_y_value)); //calculates angle of SE gate based on user inputted data
  Serial.begin(9600);
}
 
void loop(){
  controller.read();
  gcc = controller.getReport();
  ax = gcc.xAxis -128; ay = gcc.yAxis -128; //offsets from nuetral position of analog stick
  cx = gcc.cxAxis-128; cy = gcc.cyAxis-128; //offsets from nuetral position of c stick
  axm = abs(ax); aym = abs(ay);             //magnitude of analog stick offsets
  cxm = abs(cx); cym = abs(cy);             //magnitude of c stick offsets
  delta = millis() - last;
  last = millis();
  if(!off) convertinputs();                 //implements all the fixes (remove this line to unmod the controller)
  console.write(gcc);                       //sends controller data to the console
}
