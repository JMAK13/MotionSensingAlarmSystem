//We always have to include the library
#include <Arduino.h>
#include "LedControl.h"

const int BUTTON = 4;

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to LOAD(CS)
 pin 10 is connected to the CLK 
 We have only a single MAX72XX.
 */
LedControl lc=LedControl(12,10,11,1);

/* we always wait a bit between updates of the display */
unsigned long delaytime1=500;
unsigned long delaytime2=50;
void setup() {

  pinMode(BUTTON, INPUT_PULLUP);
  
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);
}

/*
 This method will display the characters for the
 word "Arduino" one after the other on the matrix. 
 (you need at least 5x7 leds to see the whole chars)
 Arduino: 1.8.8 (Windows 10), Board: "Arduino/Genuino Mega or Mega 2560, ATmega2560 (Mega 2560)"

Sketch uses 2966 bytes (1%) of program storage space. Maximum is 253952 bytes.
Global variables use 97 bytes (1%) of dynamic memory, leaving 8095 bytes for local variables. Maximum is 8192 bytes.
An error occurred while uploading the sketch

This report would have more information with
"Show verbose output during compilation"
option enabled in File -> Preferences.

 */
byte a[5]={B01111110,B10001000,B10001000,B10001000,B01111110};
// byte d[5]={B11111110,B00010010,B00100010,B00100010,B00011100};
byte d[5]={B00111000,B01000100,B10000010,B10000010,B11111110};

void loop() {
  if(digitalRead(BUTTON) == HIGH) {
    lc.setRow(0,1,a[0]);
    lc.setRow(0,2,a[1]);
    lc.setRow(0,3,a[2]);
    lc.setRow(0,4,a[3]);
    lc.setRow(0,5,a[4]);
    delay(delaytime1);
}
  else {
    lc.setRow(0,1,d[0]);
    lc.setRow(0,2,d[1]);
    lc.setRow(0,3,d[2]);
    lc.setRow(0,4,d[3]);
    lc.setRow(0,5,d[4]);
    delay(delaytime1);
  }
  
}
