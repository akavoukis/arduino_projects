#include <TimerOne.h>
#include <EEPROM.h>


//#include <cstring>
//#include <iostream>

//#include <TimerOne.h>           // Avaiable from http://www.arduino.cc/playground/Code/Timer1

volatile int i=0;               // Variable to use as a counter
volatile int i2=0;
volatile boolean zero_cross=false;  // Boolean to store a "switch" to tell us if we have crossed zero
volatile boolean zero_cross2=false;  // Boolean to store a "switch" to tell us if we have crossed zero
int AC_pin = 11;                // Output to Opto Triac
int AC_pin2 = 12;                // Output to Opto Triac
volatile int dim = 50;                    // Dimming level (0-128)  0 = on, 128 = 0ff
volatile int OnPeak = 125;
volatile int OffPeak = 0;

int freqStep = 70;    // This is the delay-per-brightness step in microseconds.
boolean stringComplete = false;  // whether the string is complete
char inChar[20]; // Allocate some space for the string
char ReadedChar;
byte index = 0; // Index into array; where to store the character
int VolumeLevel=0;
int EEPROMaddr = 0;

void zero_cross_detect() {    
  zero_cross = true;               // set the boolean to true to tell our dimming function that a zero cross has occured
  zero_cross2 = true;               // set the boolean to true to tell our dimming function that a zero cross has occured
  i=0;
  i2=0;
  digitalWrite(AC_pin, LOW);       // turn off TRIAC (and AC)
  digitalWrite(AC_pin2, LOW);       // turn off TRIAC (and AC)
}                                 

// Turn on the TRIAC at the appropriate time
void dim_check() 
{                   
  if(zero_cross == true) 
  {         
    if(i>=dim) {                     
      digitalWrite(AC_pin, HIGH); // turn on light       
      i=0;  // reset time step counter                         
      zero_cross = false; //reset zero cross detection
    } 
    else 
    {
      i++; // increment time step counter                     
    }
  }               
}        

void setup() 
{
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  pinMode(AC_pin, OUTPUT);                          // Set the Triac pin as output
  Serial.println("Serial started");
  OnPeak = EEPROM.read(EEPROMaddr);
  OffPeak = EEPROM.read(EEPROMaddr+8);
  Serial.print("On peak: ");
  Serial.println(OnPeak);
  Serial.print("Off peak: ");
  Serial.println(OffPeak);
    
  attachInterrupt(0, zero_cross_detect, RISING);   // Attach an Interupt to Pin 2 (interupt 0) for Zero Cross Detection
  Timer1.initialize(freqStep);                      // Initialize TimerOne library for the freq we need
  Timer1.attachInterrupt(dim_check, freqStep);   
}

void loop() 
{
  // print the string when a newline arrives:
  if (stringComplete) 
  {
    if (inChar[0] == 'V' && inChar[1] == ':')
    {
      char Volume[3];
      Volume[0] = inChar[3];
      Volume[1] = inChar[4];
      Volume[2] = inChar[5];

      VolumeLevel = atoi(Volume);
      VolumeLevel = constrain(VolumeLevel, 0, 125);
      
      dim = map (VolumeLevel,0,100,OffPeak,OnPeak);
      
      dim = 125 - dim;
      dim = constrain(dim, OffPeak, OnPeak);
    }
    else if (inChar[0] == 'P' && inChar[1] == 'N')
    {
      char OnPeakChar[3];

      OnPeakChar[0] = inChar[4];
      OnPeakChar[1] = inChar[5];
      OnPeakChar[2] = inChar[6];
      
      int OnPeak_premap = atoi(OnPeakChar);
      OnPeak = map (OnPeak_premap,0,100,0,125);
      EEPROM.write(EEPROMaddr, OnPeak);
    }
    else if (inChar[0] == 'P' && inChar[1] == 'F')
    {
      char OffPeakChar[3];
      OffPeakChar[0] = inChar[4];
      OffPeakChar[1] = inChar[5];
      OffPeakChar[2] = inChar[6];

      int OffPeak_premap = atoi(OffPeakChar);
      OffPeak = map (OffPeak_premap,0,100,0,125);
      EEPROM.write(EEPROMaddr+8, OffPeak);
    }
    else if  (inChar[0] == 'O' && inChar[1] == 'N')
    {
      dim = 0;
    }
    else if  (inChar[0] == 'O' && inChar[1] == 'F')
    {
      dim = 150;
    }
    stringComplete = false;
    inChar[0]='\0';
  }
}

/*

 SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */

void serialEvent() {
  while (Serial.available()) 
  {
    if(index < 19) // One less than the size of the array
    {
      ReadedChar = (char)Serial.read(); // Read a character
      inChar[index] = ReadedChar; // Store it
      index++; // Increment where to write next
    }
    else
    {
      stringComplete = true;
      index = 0;
    }
    if (ReadedChar == '\n' || ReadedChar == '\r' || ReadedChar == '\n\r' || ReadedChar == '\r\n' ) 
    {
      index = 0;
      stringComplete = true;

    }
  }
}

