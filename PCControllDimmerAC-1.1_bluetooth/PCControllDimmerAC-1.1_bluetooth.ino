#include <TimerOne.h>
#include <EEPROM.h>
#include <Wire.h>  // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>


//#include <cstring>
//#include <iostream>

// set the LCD address to 0x27 for a 16 chars 2 line display
// A FEW use address 0x3F
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address


volatile int i=0;               // Variable to use as a counter
volatile int i2=0;
volatile boolean zero_cross=false;  // Boolean to store a "switch" to tell us if we have crossed zero
volatile boolean zero_cross2=false;  // Boolean to store a "switch" to tell us if we have crossed zero
int AC_pin = 11;                // Output to Opto Triac
int AC_pin2 = 12;                // Output to Opto Triac
volatile int dim = 50;                    // Dimming level (0-128)  0 = on, 128 = 0ff
volatile int dim2 = 50;                    // Dimming level (0-128)  0 = on, 128 = 0ff

volatile int OnPeak = 128;
volatile int OffPeak = 0;

int freqStep = 70;    // This is the delay-per-brightness step in microseconds.
boolean stringComplete = false;  // whether the string is complete
char inChar[20]; // Allocate some space for the string
char ReadedChar;
byte index = 0; // Index into array; where to store the character
int VolumeLevel1=0;
int VolumeLevel2=0;
int EEPROMaddr = 0;

//LCD block all pixels on
byte block[8] = {  B111111,  B11111,  B11111,  B11111,  B11111,  B11111,  B11111,};

void zero_cross_detect() 
{    
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
    if(i>=dim) 
    {                     
      digitalWrite(AC_pin, HIGH); // turn on light       
      i=0;  // reset time step counter                         
      zero_cross = false; //reset zero cross detection
    } 
    else 
    {
      i++; // increment time step counter                  
    }
  }
  if(zero_cross2 == true) 
  {          
    if(i2>=dim2) 
    {                     
      digitalWrite(AC_pin2, HIGH); // turn on light       
      i2=0;  // reset time step counter                         
      zero_cross2 = false; //reset zero cross detection
    }   
    else 
    {
      i2++; // increment time step counter                  
    }     
  }
}        

//setup
void setup() 
{
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  pinMode(AC_pin, OUTPUT);                          // Set the Triac pin as output
  pinMode(AC_pin2, OUTPUT);                          // Set the Triac pin as output

  Serial.println("Serial started");
  OnPeak = EEPROM.read(EEPROMaddr);
  OffPeak = EEPROM.read(EEPROMaddr+8);
  Serial.print("On peak: ");
  Serial.println(OnPeak);
  Serial.print("Off peak: ");
  Serial.println(OffPeak);
  lcd.begin(16,2);   // initialize the lcd for 16 chars 2 lines, turn on backlight
  lcd.backlight();

  attachInterrupt(digitalPinToInterrupt(2), zero_cross_detect, RISING);   // Attach an Interupt to Pin 2 (interupt 0) for Zero Cross Detection

  Timer1.initialize(freqStep);                      // Initialize TimerOne library for the freq we need
  Timer1.attachInterrupt(dim_check, freqStep);   

  lcd.setCursor(0,0);
  lcd.print("Arduino Lights");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("Booting");
  delay(1000);
  lcd.clear();
  
  String OnString="On peak: ";
  OnString += OnPeak;
  lcd.setCursor(0,0);
  lcd.print(OnString);
  
  String OffString="Off peak: ";
  OffString += OffPeak;
  lcd.setCursor(0,1);
  lcd.print(OffString);

  delay(1000);

    lcd.clear();

    lcd.setCursor(0,0);
    lcd.write("L1:   ");
    int bar1 = 50;
    for (int i = 0; i<bar1/10;i++) {  lcd.write(byte(0)); }

    lcd.setCursor(0,1);
    lcd.write("L2:   ");
    int bar2 = 50;
    for (int i = 0; i<bar2/10;i++) {  lcd.write(byte(0)); }
}

void loop() 
{
  // print the string when a newline arrives:
  if (stringComplete) 
  {
    if (inChar[0] == 'V' && inChar[1] == '1' && inChar[2] == ':')
    {
      char Volume[3];
      Volume[0] = inChar[4];
      Volume[1] = inChar[5];
      Volume[2] = inChar[6];

      VolumeLevel1 = atoi(Volume);
      VolumeLevel1 = constrain(VolumeLevel1, 0, 100);
          Serial.println(VolumeLevel1);

      dim = map (VolumeLevel1,0,100,OffPeak,OnPeak);
                Serial.println(dim);

      dim = 128 - dim;
      Serial.println(dim);
    }
    if (inChar[0] == 'V' && inChar[1] == '2' && inChar[2] == ':')
    {
      char Volume[3];
      Volume[0] = inChar[4];
      Volume[1] = inChar[5];
      Volume[2] = inChar[6];

      VolumeLevel2 = atoi(Volume);
      VolumeLevel2 = constrain(VolumeLevel2, 0, 100);

      dim2 = map (VolumeLevel2,0,100,OffPeak,OnPeak);
      
      dim2 = 128 - dim2;
    }
    else if (inChar[0] == 'P' && inChar[1] == 'N')
    {
      char OnPeakChar[3];

      OnPeakChar[0] = inChar[4];
      OnPeakChar[1] = inChar[5];
      OnPeakChar[2] = inChar[6];
      
      int OnPeak_premap = atoi(OnPeakChar);
      OnPeak = map (OnPeak_premap,0,100,0,128);
      EEPROM.write(EEPROMaddr, OnPeak);

    }
    else if (inChar[0] == 'P' && inChar[1] == 'F')
    {
      char OffPeakChar[3];
      OffPeakChar[0] = inChar[4];
      OffPeakChar[1] = inChar[5];
      OffPeakChar[2] = inChar[6];

      int OffPeak_premap = atoi(OffPeakChar);
      OffPeak = map (OffPeak_premap,0,100,0,128);
      EEPROM.write(EEPROMaddr+8, OffPeak);
    }
    else if  (inChar[0] == 'O' && inChar[1] == 'N' && inChar[2] == ':' && inChar[4] == '1' )
    {
      dim = 0;
    }
    else if  (inChar[0] == 'O' && inChar[1] == 'F' && inChar[3] == ':' && inChar[5] == '1')
    {
      dim = 150;
    }
    else if  (inChar[0] == 'O' && inChar[1] == 'N' && inChar[2] == ':' && inChar[4] == '2')
    {
      dim2 = 0;
    }
    else if  (inChar[0] == 'O' && inChar[1] == 'F' && inChar[3] == ':' && inChar[5] == '2')
    {
      dim2 = 150;
    }
    stringComplete = false;
    inChar[0]='\0';

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.write("L1:   ");
    int bar_dim = 0;
    
    bar_dim = map (dim,128,0,0,100);
    bar_dim = roundToTenths(bar_dim);
    for (int i = 0; i<bar_dim/10;i++) {  lcd.write(byte(0)); }


    lcd.setCursor(0,1);
    lcd.write("L2:   ");
    
    bar_dim = map (dim2,128,0,0,100);
    bar_dim = roundToTenths(bar_dim);
    for (int i = 0; i<bar_dim/10;i++) {  lcd.write(byte(0)); }

  }
}

float roundToTenths(float x)
{
    x /=10;
    return floor(x + 0.5) * 10;
}

/*

 SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */

void serialEvent() 
{
  while (Serial.available()) 
  {
    if(index < 19) // One less than the size of the array
    {
      ReadedChar = (char)Serial.read(); // Read a character
      //debug incoming data from serial1 to serial
      Serial.write(ReadedChar);
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


