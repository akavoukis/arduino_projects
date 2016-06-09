#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance.


enum StatusCode {
  unknown,
  RED,
  GREEN,
  ARED,
  AGREEN,
  YELLOW,
  AYELLOW
};

#define LED_PIN 13
byte x = 0;
int DataReceived;
int myStatus = RED;
int RemoteStatus = unknown;

boolean Transmit;
boolean Received ;
boolean StartAsMaster = true;

int I2CAddress ;
int I2CRemoteAddress ;
long I2CpreviousMillis = 0;        // will store last time LED was updated
long interval100 = 100;           // interval at which to blink (milliseconds)
long intervaTrafficLight = 5000;           // interval at which to blink (milliseconds)
long intervalBetweenReds = 1000;
long intervalYellow = 2000;


int GreenLed = 8;
int RedLed = 7;
int YellowLed = 6;


long TimeStartedAsGreen;
long TimeStartedAsYellow;
long TimeStartedAsAYellow;
long TimeStartedAsAGreen;
long TimeBetweenReds = 0;
boolean  REDDelay = false;

boolean CanChangeToGreen = true;
boolean Alarm = false;
boolean RAlarm =false;

void ChangeMode()
{
  if (Transmit == true)
  {
    Wire.begin(); // Start I2C Bus as Master
  }
  else if (Transmit == false)
  {
  Wire.begin(I2CAddress);                // Start I2C Bus as a Slave (Device Number 9)
  Wire.onReceive(receiveEvent); // register event
  }
}

void SetInitialMode()
{
  if (StartAsMaster == true)  
  {
     Wire.begin(); // Start I2C Bus as Master 
  }
  else if (StartAsMaster == false)
  {
    Wire.begin(I2CAddress);                // Start I2C Bus as a Slave (Device Number 9)
    Wire.onReceive(receiveEvent); // register event
  }
}

void setup()
{
 
    Serial.begin(9600);
 
  //Sets address acording to the initial state
  
  if (StartAsMaster == true) {I2CAddress = 1; } 
  else if (StartAsMaster == false) {I2CAddress = 2; } 
  
  if (StartAsMaster == true) {I2CRemoteAddress = 2; } 
  else if (StartAsMaster == false) {I2CRemoteAddress = 1; } 
  
  if (StartAsMaster == true) {Transmit = true;}
  else if (StartAsMaster == false)  {Transmit = false;}
  
  if (StartAsMaster == true) {Received = false;}
  else if (StartAsMaster == false)  {Received = true;}
  
  SetInitialMode();
  
  pinMode(GreenLed, OUTPUT);
  pinMode(RedLed, OUTPUT);
  pinMode(YellowLed, OUTPUT);

  
  digitalWrite(GreenLed, HIGH);
  digitalWrite(RedLed, HIGH);
  digitalWrite(YellowLed, HIGH);

  
  SPI.begin();			// Init SPI bus
  mfrc522.PCD_Init();	// Init MFRC522 card
  Serial.println("Scan PICC to see UID and type...");
  
}


void loop()
{
  unsigned long currentMillis = millis();
 
  if(currentMillis - I2CpreviousMillis > interval100) 
  {
    // save the last time you blinked the LED 
    I2CpreviousMillis = currentMillis;   
    
    if (Transmit == true)
    {
      Wire.beginTransmission(I2CRemoteAddress); // transmit to device #9
          
      Wire.write(myStatus);              // sends x 
      Wire.endTransmission();    // stop transmitting
      Transmit = false;
      ChangeMode();
    }
    if (Received == true)
    {
      Transmit = true;
      Received = false;
      ChangeMode();
    }
  }
  
  scanfrid();
  DecideLights();
   
  Serial.print("myStatus: ");
  Serial.print(myStatus);
  Serial.print(" RemoteStatus ");
  Serial.print(RemoteStatus);
  Serial.print(" Can Change ");
  Serial.print(CanChangeToGreen);
  Serial.print(" REDDelay ");
  Serial.println(REDDelay);
  
}

void DecideLights()
{
 unsigned long currentMillis = millis();
 
 if (Alarm == true)
 {
   myStatus = AGREEN;
 }
 
 if ((myStatus == AGREEN) && (RemoteStatus == ARED) && (Alarm == true))
 {
     Alarm = false;
     AGreenLightOn();
 }
 
 else if ((RemoteStatus == AGREEN))
 {
     ARedLightOn(); 
 }
 
 else if ((myStatus == AGREEN) && (Alarm == false))
 {

   if(currentMillis - TimeStartedAsAGreen > intervaTrafficLight) 
    {
      RedLightOn();
    }
 }
 
 else if ((myStatus == ARED && RemoteStatus == RED) )
 {

    if (REDDelay == true)
   {
      TimeBetweenReds = millis();
   }
        GreenLightOn();
 }
 else if ((RemoteStatus == RED) && (myStatus == RED) && (CanChangeToGreen == true) )
 {
     Serial.print(" edw ??? ");

   
   if (REDDelay == true)
   {
      TimeBetweenReds = millis();
   }
      GreenLightOn();
 }
 else if ((RemoteStatus == RED) && (myStatus == GREEN))
 {

    if(currentMillis - TimeStartedAsGreen > intervaTrafficLight) 
    {
      YellowLightOn();
    }
 }
 else if (((RemoteStatus == RED) && (myStatus == YELLOW)))
 {
   if(currentMillis - TimeStartedAsYellow > intervalYellow) 
    {
      RedLightOn();
    }
 }
 
 //OLA TA FANARIA PRASSINA opote anapse kokina
 else if ((RemoteStatus == GREEN) && (myStatus == GREEN))
 {
   RedLightOn();
 }

}




void GreenLightOn()
{
    REDDelay = false;
    unsigned long currentMillis = millis();
    if(currentMillis - TimeBetweenReds > intervalBetweenReds) 
    {
      digitalWrite(GreenLed, HIGH);
      digitalWrite(RedLed, LOW);
      digitalWrite(YellowLed, LOW);

      TimeStartedAsGreen =  millis();
      CanChangeToGreen = false;
      myStatus = GREEN;
    }
}

void AGreenLightOn()
{
   //REDDelay = false;
    unsigned long currentMillis = millis();
    if(currentMillis - TimeBetweenReds > intervalBetweenReds) 
    {
      digitalWrite(GreenLed, HIGH);
      digitalWrite(RedLed, LOW);
      digitalWrite(YellowLed, LOW);

      TimeStartedAsAGreen =  millis();
      CanChangeToGreen = false;
      myStatus = AGREEN;
     }
}

void RedLightOn()
{
  digitalWrite(GreenLed, LOW);
  digitalWrite(RedLed, HIGH);      
  digitalWrite(YellowLed, LOW);
  TimeStartedAsGreen =  0;
  myStatus = RED;
  RemoteStatus = unknown;
}



void ARedLightOn()
{
  digitalWrite(GreenLed, LOW);
  digitalWrite(RedLed, HIGH);
  digitalWrite(YellowLed, LOW);

  TimeStartedAsGreen =  0;
  RAlarm = true;

  myStatus = ARED;
  RemoteStatus = unknown;
}

void YellowLightOn()
{
  digitalWrite(GreenLed, LOW);
  digitalWrite(RedLed, LOW);
  digitalWrite(YellowLed, HIGH);
  TimeStartedAsYellow =  millis();
  myStatus = YELLOW;
  RemoteStatus = unknown;
}


void receiveEvent(int howMany) {

  int tempRemoteStatus = Wire.read();
  if ((tempRemoteStatus == RED) && (RemoteStatus == YELLOW))
  {
        CanChangeToGreen = true;
        REDDelay = true;

  }
  if ((tempRemoteStatus == RED) && (RemoteStatus == GREEN))
  {
        CanChangeToGreen = true;
        REDDelay = true;

  }
    if ((tempRemoteStatus == RED) && (RemoteStatus == AGREEN))
  {
        CanChangeToGreen = true;
        REDDelay = true;

  }
    if ((tempRemoteStatus == RED) && (myStatus == ARED)  && (RAlarm == true))
  {
        CanChangeToGreen = true;
        REDDelay = true;
        RAlarm = false;
  }
  RemoteStatus = tempRemoteStatus;    // receive byte as an integer
  //RemoteStatus = Wire.read();    // receive byte as an integer
    Received = true;
}

void scanfrid()
{
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	} 
  Alarm = true;
  mfrc522.PICC_HaltA();
}

