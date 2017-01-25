/*
 * IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <IRremote.h>
#include "Keyboard.h"

int RECV_PIN = 11;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  Keyboard.begin();

}

void loop() {
  if (irrecv.decode(&results)) 
  {
    switch (results.value)
    {
      // button 1
      case 0x1CF:
      case 0x9CF:
        break;
            
      // button 2
      case 0x1D5:
      case 0x9D5:
        break;

      // button 3
      case 0x1D0:
      case 0x9D0:
        break;
      
      // button 4
      case 0x1D8:
      case 0x9D8:
        Keyboard.press(KEY_LEFT_ARROW);
        break;   

      // button 5
      case 0x1DB:
      case 0x9DB:
        Keyboard.press(KEY_RETURN);
        break;  

      // button 6
      case 0x1DE:
      case 0x9DE:
        Keyboard.press(KEY_RIGHT_ARROW);
        break;  

      // button 7
      case 0x1D1:
      case 0x9D1:
        Keyboard.press(KEY_BACKSPACE);
        break; 

      // button 8
      case 0x1E1:
      case 0x9E1:
        break; 
        
      // button 9
      case 0x1D2:
      case 0x9D2:
        break;  

      // source menu
      case 0x1E4:
      case 0x9E4:
        // open context menu
        Keyboard.press('C');      
        break; 
         
      // button 0
      case 0x1E7:
      case 0x9E7:
        break; 

      // button T
      case 0x1EA:
      case 0x9EA:
        break; 
        
      // button Mute
      case 0x1C0:
      case 0x9C0:
        Keyboard.press(KEY_F8);
        break; 

      // button pinnacle
      case 0x1C1:
      case 0x9C1:
        break; 

      // button volume up
      case 0x1C3:
      case 0x9C3:
        Keyboard.press('+');
        break;

      // button volume down
      case 0x1C9:
      case 0x9C9:
        Keyboard.press('-');
        break;

      // button program up
      case 0x1C6:
      case 0x9C6:
        Keyboard.press(KEY_UP_ARROW);
        break;

      // button program down
      case 0x1CC:
      case 0x9CC:
        Keyboard.press(KEY_DOWN_ARROW);
        break;

      // button rewind
      case 0x1ED:
      case 0x9ED:
        break;

      // button forward
      case 0x1F3:
      case 0x9F3:
        break;

      // button play pause
      case 0x1F0:
      case 0x9F0:
        Keyboard.press(' ');
        break;

      // button record
      case 0x1F6:
      case 0x9F6:
        break;

      // button stop
      case 0x1FC:
      case 0x9FC:
        Keyboard.press('X');
        break;

      // button help
      case 0x1FF:
      case 0x9FF:
        break;
    }
    Keyboard.releaseAll();
    Serial.println(results.value, HEX);
    irrecv.resume(); // Receive the next value
  }
delay(100);
}
