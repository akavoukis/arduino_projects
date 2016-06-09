#include <idDHT11.h>
#include <Keypad.h>
#include <Password.h> 
#include <GSM.h>
#define PINNUMBER "0000"
GSM gsmAccess;
GSM_SMS sms;
GSMVoiceCall vcs;

int idDHT11pin = 18; //Digital pin for comunications
int idDHT11intNumber = 5; //interrupt number (must be the one that use the previus defined pin (see table above)

//declaration
void dht11_wrapper(); // must be declared before the lib initialization

// Lib instantiate
idDHT11 DHT11(idDHT11pin,idDHT11intNumber,dht11_wrapper);



//PIR Sensor variables
int PinSensorPin = 50;               // choose the input pin (for PIR sensor)
int PirSensorState = LOW;             // we start, assuming no motion detected
int PirSensorValue = 0;                    // variable for reading the pin status

//Gas Sensor variables
int GasSensorPin = A0;    // select the input pin for the potentiometer
int GasSensorValue = 0; 
int GasSensorState = LOW;

//Temperature Sensor variables
float TemperatureSensorValue= 0;
int TemperatureSensorState = LOW;
unsigned long previousMillis = 0;
unsigned long intervalTemperatureReading = 0;

//Knock sensor variables
int KnockSensorPin = A4; // the piezo is connected to analog pin 0
int KnockThreshold = 300;  // threshold value to decide when the detected sound is a knock or not
int KnockSensorValue = 0;      // variable to store the value read from the sensor pin
int KnockSensorState = LOW;

//water sensor
int WaterSensorPin = 8;
int WaterSensorValue = 0;
int WaterSensorState = LOW;

//Photoresistor sensor
int PhotoResistorPin = A2;
int PhotoResistorValue = 0;
int PhotoResistorState = LOW;

//Magnetic touch sensor
int MagneticSensorPin1 = 22;
int MagneticSensorState1 = LOW;
int MagneticSensorPin2 = 23;
int MagneticSensorState2 = LOW;

//Button sensor
int ButtonSensorPin = 24;
int ButtonSensorState = LOW;
int PreviousButtonState = LOW;

int ActiveActivationState = 0;
int ActiveAlarmState = 0;

//BUZZER
#define BUZZER 25
unsigned long previousMillisBuzzer = 0;
unsigned long intervalBuzzer = 10;
int BuzzerState = LOW;

//4 LEDS
int ActiveRedLedPin = 46;
int NotActiveGreenLedPin = 47;
int FireWhiteLed = 48;
int AlarmActivated = 49;

unsigned long previousMillisLedAlarm = 0;        // will store last time LED was updated
unsigned long intervalLedAlarm = 100;           // intervalLedAlarm at which to blink (milliseconds)
int ledStateRedAlarmActivated = LOW;

unsigned long IntervalDisableAlarm = 30000;
unsigned long PreviousMilisDisableAlarm = 0;
unsigned long TimeActivated ;
unsigned long TimeActivated2;

int ResetGSMPin = 11;
int PowerGSMPin = 12;
char remoteNum[20] = "6949303692";  // telephone number to send sms
char txtMsg[ ] = "";
String MySMS = "" ;
boolean smsSent = false;
char c;
char senderNumber[20];  
char SMS[200]; 
String remoteNumber = "6949303692";  // the number you will call

unsigned long intervalReceiveSms = 5000;
unsigned long previousMillisReceiveSms = 0; 

enum AlarmStates
{
  NotAlarming,
  AlarmMode1,
  AlarmMode2,
  EmergencyAlarm,
  AlarmMode3,
  AlarmMode4
};


enum ActivationStates 
{
  AlarmNotActive,
  AlarmActive
};

//Keypad code
Password password = Password( "1234" );
int PasswordCodeCorrect = LOW;
const byte ROWS = 4; // Four rows
const byte COLS = 3; // columns
// Define the Keymap
char keys[ROWS][COLS] = {
 {'1','2','3'},
 {'4','5','6'},
 {'7','8','9'},
 {'*','0','#'}
};
//Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = { 34, 35, 36, 37 };// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = { 38, 39, 40 };
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void keypadEvent(KeypadEvent eKey){
 switch (keypad.getState()){
  case PRESSED:
    Serial.print("Pressed: ");
    Serial.println(eKey);
    switch (eKey){
     case '*': checkPassword(); break;
     case '#': password.reset(); break;
     default: password.append(eKey);
  }
 }
}

void checkPassword()
{
 if (password.evaluate())
 {
  Serial.println("Success");
  if (ActiveActivationState == 0)
  ActiveActivationState = 1;
  else
  ActiveActivationState = 0;
 }
 else
 {
  Serial.println("Wrong");
  //add code to run if it did not work
 }
}


void soundAlarm()
{
  unsigned long currentMillisBuzzer = millis();
  if(currentMillisBuzzer - previousMillisBuzzer > intervalBuzzer) 
  {
    for(int i=0;i<100;i++) //Sound effect 2
      {
        BuzzerState = !BuzzerState;
        delay(2);//Delay 2ms
        digitalWrite(BUZZER,BuzzerState);//Make some sound
      }	
    previousMillisBuzzer = currentMillisBuzzer;    
  }
  
}


void setup() 
{
  Serial.begin(9600); 
  pinMode(BUZZER, OUTPUT);
  pinMode(WaterSensorPin, INPUT);
  pinMode(PinSensorPin, INPUT);
  pinMode(MagneticSensorPin1, INPUT_PULLUP);
  pinMode(MagneticSensorPin2, INPUT_PULLUP);
  pinMode(ButtonSensorPin, INPUT);
  
  pinMode(ActiveRedLedPin, OUTPUT);
  pinMode(NotActiveGreenLedPin, OUTPUT);
  pinMode(FireWhiteLed, OUTPUT);
  pinMode(AlarmActivated, OUTPUT);
  
  pinMode(ResetGSMPin, OUTPUT);
  pinMode(PowerGSMPin, OUTPUT);

  
  digitalWrite(ActiveRedLedPin,LOW);
  digitalWrite(NotActiveGreenLedPin,LOW);
  digitalWrite(FireWhiteLed,LOW);
  digitalWrite(AlarmActivated,LOW);

  digitalWrite(PowerGSMPin,HIGH);

  
  digitalWrite(ResetGSMPin,HIGH);
  delay(100);
  digitalWrite(ResetGSMPin,LOW);
  
  keypad.addEventListener(keypadEvent); //add an event listener for this keypad
  boolean notConnected = true;
  
  digitalWrite(ActiveRedLedPin,LOW);

  
  while(notConnected)
  {
    
    if(gsmAccess.begin(PINNUMBER)==GSM_READY)
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }
}
 
void FlashAlarmOnLed()
 {
   
  unsigned long currentMillisLedAlarm = millis();
 
  if(currentMillisLedAlarm - previousMillisLedAlarm > intervalLedAlarm) 
  {
    // save the last time you blinked the LED 
    previousMillisLedAlarm = currentMillisLedAlarm;   

    // if the LED is off turn it on and vice-versa:
    if (ledStateRedAlarmActivated == LOW)
      ledStateRedAlarmActivated = HIGH;
    else
      ledStateRedAlarmActivated = LOW;

    // set the LED with the ledState of the variable:
    digitalWrite(AlarmActivated, ledStateRedAlarmActivated);
  }
}
  
  
 void dht11_wrapper() {
  DHT11.isrCallback();
}

void ReadSensors()
{
  //PIR sensor
  PirSensorValue = digitalRead(PinSensorPin);  
  if (PirSensorValue == HIGH) 
  {          
    if (PirSensorState == LOW) 
    {
      PirSensorState = HIGH;
    }
  } 
  else 
  {
    if (PirSensorState == HIGH)
    {
      PirSensorState = LOW;
    }
  }
  
   //Gas sensor  
   GasSensorValue = analogRead(GasSensorPin);  
   if (GasSensorValue > 200 ) 
   {
     GasSensorState = HIGH;
   }
   else
   {
     GasSensorState = LOW;
   }

  //temperature humidity sensor
  unsigned long currentMillis = millis();
  
   if(currentMillis - previousMillis > intervalTemperatureReading) 
   {
     intervalTemperatureReading = 1000;
    // save the last time you blinked the LED 
      previousMillis = currentMillis;   
      {
          DHT11.acquire();
          while (DHT11.acquiring())
            ;
          int result = DHT11.getStatus();
            
          TemperatureSensorValue = DHT11.getCelsius(), 2;
          
          
          if (TemperatureSensorValue > 35)
          {
            TemperatureSensorState = HIGH;
          }
          else
          {
            TemperatureSensorState = LOW;
          }
      }
    }
   
  //Water Sensor
  WaterSensorValue = digitalRead(WaterSensorPin) ;
  if (WaterSensorValue == LOW)
  {
    WaterSensorState = HIGH;
  }
  else
  {
    WaterSensorState = LOW;
  }
   
   //Knock Sensor
  KnockSensorValue = analogRead(KnockSensorPin);  
  if (KnockSensorValue >= KnockThreshold) 
  {
    KnockSensorState = HIGH;
  }
  else
  {
    KnockSensorState = LOW;
  }
   
  //Photoresistor
  PhotoResistorValue = analogRead(PhotoResistorPin);
  if ( PhotoResistorValue > 900 )
  {
    PhotoResistorState = LOW;
  }
  else 
  {
    PhotoResistorState = HIGH;
  }
  
  
  //Magnetic Sensor
  
  MagneticSensorState1 = digitalRead(MagneticSensorPin1);  
  MagneticSensorState2 = digitalRead(MagneticSensorPin2);  
  
  //Button Sensor
   ButtonSensorState = digitalRead(ButtonSensorPin);  
   ButtonSensorState = !ButtonSensorState;


}

void DebugSensor()

{
  Serial.print(" PIR: ");   Serial.print(PirSensorValue);           Serial.print(" S: "); Serial.print(PirSensorState);
  Serial.print(" Gas: ");   Serial.print(GasSensorValue);           Serial.print(" S: "); Serial.print(GasSensorState);
  Serial.print(" TEMP: ");  Serial.print(TemperatureSensorValue);   Serial.print(" S: "); Serial.print(TemperatureSensorState);
  Serial.print(" WATER: "); Serial.print(WaterSensorValue);         Serial.print(" S: "); Serial.print(WaterSensorState);
  Serial.print(" Knock: "); Serial.print(KnockSensorValue);         Serial.print(" S: "); Serial.print(KnockSensorState);
  Serial.print(" Photo: "); Serial.print(PhotoResistorValue);       Serial.print(" S: "); Serial.print(PhotoResistorState);
  Serial.print(" Magn1: "); Serial.print(MagneticSensorState1);     Serial.print(" S: "); Serial.print(MagneticSensorState1);
  Serial.print(" Magn2: "); Serial.print(MagneticSensorState2);     Serial.print(" S: "); Serial.print(MagneticSensorState2);
  Serial.print(" Butto: "); Serial.print(ButtonSensorState);        Serial.print(" S: "); Serial.print(ButtonSensorState);
  Serial.print(" AlarmState: "); Serial.print(ActiveAlarmState);
  Serial.print(" C: "); Serial.print(TimeActivated2);               Serial.print(" D: "); Serial.print(TimeActivated2 + IntervalDisableAlarm);   
  Serial.print(" Activation: ");                     Serial.println(ActiveActivationState);
  Serial.println(MySMS); 
  
}

void SetAlarmCase()
{
  if (ActiveAlarmState != EmergencyAlarm)
  {
    if  ((PirSensorState == HIGH) || (GasSensorState == HIGH) || (TemperatureSensorState == HIGH) || (WaterSensorState == HIGH) || (KnockSensorState == HIGH) || (PhotoResistorState == HIGH) ||
        (MagneticSensorState1 == HIGH) || (MagneticSensorState2 == HIGH) )
    {
      if (smsSent == false) SetSMSMsg(); 
      if (ActiveAlarmState == NotAlarming)
      {
        TimeActivated = millis();
      }
      ActiveAlarmState = AlarmMode1;
    }
    else
    {    
        if(TimeActivated +  IntervalDisableAlarm < millis() ) 
        {
        ActiveAlarmState = NotAlarming;
        }
    }  
    if (TemperatureSensorState == HIGH) 
    {
      if (smsSent == false) SetSMSMsg(); 
      ActiveAlarmState = AlarmMode2;  
    }
    else
    {
      if(TimeActivated +  IntervalDisableAlarm < millis() ) 
        {   
            if(TimeActivated +  IntervalDisableAlarm < millis() ) 
            {
            ActiveAlarmState = NotAlarming;
            }
        }
    }
    if (PhotoResistorState == HIGH)
    {
      if (smsSent == false) SetSMSMsg(); 
      ActiveAlarmState = AlarmMode3;  
    }
    else
    {
      if(TimeActivated +  IntervalDisableAlarm < millis() ) 
        {   
            if(TimeActivated +  IntervalDisableAlarm < millis() ) 
            {
            ActiveAlarmState = NotAlarming;
            }
        }
    }
    
    if ((PhotoResistorState == HIGH) && (TemperatureSensorState == HIGH))
    {
      if (smsSent == false) SetSMSMsg(); 
      ActiveAlarmState = AlarmMode4;  
    }
    else
    {
      if(TimeActivated +  IntervalDisableAlarm < millis() ) 
        {   
            if(TimeActivated +  IntervalDisableAlarm < millis() ) 
            {
            ActiveAlarmState = NotAlarming;
            }
        }
    }
    
  }
}

void AlarmCase()
{
  switch(ActiveAlarmState)
  {
    case NotAlarming:
    //Serial.println("State Set to NotAlarming");
    digitalWrite(AlarmActivated, LOW);
    digitalWrite(FireWhiteLed, LOW);
    ActiveAlarmState = NotAlarming;
    smsSent = false;  
    break;
    
    case AlarmMode1:
    //Serial.println("State Set to AlarmMode1");
    digitalWrite(FireWhiteLed, LOW);
    FlashAlarmOnLed();    
    soundAlarm();
    if (smsSent == false) SendSMS();
    //delay(100);    
    break;
    
    case AlarmMode2:
    //Serial.println("State Set to AlarmMode2");
    digitalWrite(FireWhiteLed, HIGH);
    FlashAlarmOnLed();    
    soundAlarm();
    if (smsSent == false) SendSMS();   
    //delay(100); 
    break;
    
    case EmergencyAlarm:
    digitalWrite(FireWhiteLed, LOW);
    FlashAlarmOnLed();    
    soundAlarm();
    break;
    
    case AlarmMode3:
    digitalWrite(FireWhiteLed, LOW);
    FlashAlarmOnLed(); 
    if (smsSent == false) SendSMS();  
    //delay(100); 
    break;
    
    case AlarmMode4: //photo + temp
    FlashAlarmOnLed(); 
    soundAlarm();
    digitalWrite(FireWhiteLed, HIGH);
    if (smsSent == false) SendSMS();   
    //delay(100); 
    break;
  }
}



void ActivationCase()
{
  switch(ActiveActivationState)
  {
    case AlarmNotActive:
    
    digitalWrite(ActiveRedLedPin,LOW);
    digitalWrite(NotActiveGreenLedPin,HIGH);
    if ( ActiveAlarmState != EmergencyAlarm)
    {
      ActiveAlarmState = NotAlarming;
    }
    break;
    
    case AlarmActive:
        
    digitalWrite(ActiveRedLedPin,HIGH);
    digitalWrite(NotActiveGreenLedPin,LOW);
    
    SetAlarmCase();
    
    break;
  }
}

void ScanForButton()
{
  if ( ButtonSensorState == HIGH)
  {
    PreviousButtonState = HIGH;
    TimeActivated2 = millis();
    ActiveAlarmState = EmergencyAlarm;
         
  }
  if ((PreviousButtonState == HIGH) && (ButtonSensorState == LOW))
  {
    
    if(TimeActivated2 +  IntervalDisableAlarm < millis() ) 
    {
      ActiveAlarmState = NotAlarming;
      PreviousButtonState = LOW;
    }
  }

}

void SetSMSMsg()
{

  //delete [] txtMsg; 
  char txtMsg[200];

   MySMS = "";
   MySMS = "Aisthithres:";
  if (PirSensorState == HIGH) MySMS += " kinhshs ";
  if (GasSensorState == HIGH) MySMS += " aeriou ";
  if (TemperatureSensorState == HIGH) MySMS += " thermokrasias ";
  if (WaterSensorState == HIGH) MySMS += " nerou ";
  if (KnockSensorState == HIGH) MySMS += " kroushs ";
  if (KnockSensorState == HIGH) MySMS += KnockSensorValue;
  if (PhotoResistorState == HIGH) MySMS += " Laser ";
  if (MagneticSensorState1 == HIGH) MySMS += " magnitikos1 ";
  if (MagneticSensorState2 == HIGH) MySMS += " magnitikos2 ";
  
  MySMS.toCharArray(txtMsg, 200);
  //strcpy(txtMsg,MySMS.c_str());

  
}

void SendSMS()
{
  
  sms.beginSMS(remoteNum);
  //sms.print(txtMsg);
  sms.print(MySMS);
  sms.endSMS(); 
  
  smsSent = true;
  
}

void ReceviveSMS()
{
  unsigned long currentMillisReceiveSms = millis();
 
  if(currentMillisReceiveSms - previousMillisReceiveSms > intervalReceiveSms) 
  {
      if (sms.available())
      {
        sms.remoteNumber(senderNumber, 20);
      
        boolean stringComplete =false;
        char inputChar[200] = "";
        int i = 0;
        while(c=sms.read())
        {
          inputChar[i] = c;
          i++;
          if (c == '\n') 
          { 
            stringComplete = true; 
          }
        }
         
        if ((inputChar[0] == 'O' ) && (inputChar[1] == 'N' ))
        {
    
          ActiveActivationState = AlarmActive;
        }
        if ((inputChar[0] == 'O' ) && (inputChar[1] == 'F' ) && (inputChar[2] == 'F' ) )
        {
    
          ActiveActivationState = AlarmNotActive;
        }
        if ((inputChar[0] == 'C' ) && (inputChar[1] == 'A' ) && (inputChar[2] == 'L' ) && (inputChar[3] == 'L' ))
        {
          Serial.println("CALL");
          char charbuffer[20];
          remoteNumber.toCharArray(charbuffer, 20);
           if(vcs.voiceCall(charbuffer))
            {
              Serial.println("Call Established. Enter line to end");
              // Wait for some input from the line
              while(Serial.read()!='\n' && (vcs.getvoiceCallStatus()==TALKING));          
              // And hang up
              vcs.hangCall();
            }
      }
      sms.flush();
    }
  }
}

void loop()
{
  ReceviveSMS();
  keypad.getKey();
   
  ReadSensors();
  //DebugSensor();
  
  AlarmCase();
  
  ActivationCase();
  ActivationStates();

  ScanForButton();
  
  
}
