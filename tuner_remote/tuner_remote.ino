/*
 * IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <IRremote.h>
#include "Keyboard.h"

int RECV_PIN = 10;

IRrecv irrecv(RECV_PIN);

decode_results results;
int last_received;
int same_counter=0;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  Keyboard.begin();
}

void loop() {
  if (irrecv.decode(&results)) 
  { 
    results.value = results.value & 0xFF;
    if (last_received == results.value) 
      same_counter++ ;
    else
      same_counter=0;
      
    if ((same_counter > 4) || (last_received != results.value))
    {
      switch (results.value)
      {
        // button 1
        case 0xCF:
          break;
              
        // button 2
        case 0xD5:
          Keyboard.press(KEY_UP_ARROW);
          break;
  
        // button 3
        case 0xD0:
          break;
        
        // button 4
        case 0xD8:
          Keyboard.press(KEY_LEFT_ARROW);
          break;   
  
        // button 5
        case 0xDB:
          Keyboard.press(KEY_RETURN);
          break;  
  
        // button 6
        case 0xDE:
          Keyboard.press(KEY_RIGHT_ARROW);
          break;  
  
        // button 7
        case 0xD1:
          Keyboard.press(KEY_BACKSPACE);
          break; 
  
        // button 8
        case 0xE1:
          Keyboard.press(KEY_DOWN_ARROW);
          break; 
          
        // button 9
        case 0xD2:
          break;  
  
        // source menu
        case 0xE4:
          // open context menu
          Keyboard.press('C');      
          break; 
           
        // button 0
        case 0xE7:
          break; 
  
        // button T
        case 0xEA:
            Keyboard.press('T');
          break; 
          
        // button Mute
        case 0xC0:
          Keyboard.press(KEY_F8);
          break; 
  
        // button pinnacle
        case 0xC1:
          Keyboard.press('e');
          break; 
  
        // button volume up
        case 0xC3:
          Keyboard.press('+');
          break;
  
        // button volume down
        case 0xC9:
          Keyboard.press('- ');
          break;
  
        // button program up
        case 0xC6:
          Keyboard.press(KEY_UP_ARROW);
          break;
  
        // button program down
        case 0xCC:
          Keyboard.press(KEY_DOWN_ARROW);
          break;
  
        // button rewind
        case 0xED:
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.press(KEY_LEFT_ARROW);
          break;
  
        // button forward
        case 0xF3:
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.press(KEY_RIGHT_ARROW);
          break;
  
        // button play pause
        case 0xF0:
          Keyboard.press(' ');
          break;
  
        // button record
        case 0xF6:
          Keyboard.press('K');
          break;
  
        // button stop
        case 0xFC:
          Keyboard.press('X');
          break;
  
        // button help
        case 0xFF:
          Keyboard.press('O');
          break;
      }
      Keyboard.releaseAll();
    }
    last_received = results.value;
    Serial.println(results.value, HEX);
    irrecv.resume(); // Receive the next value
  }
}
