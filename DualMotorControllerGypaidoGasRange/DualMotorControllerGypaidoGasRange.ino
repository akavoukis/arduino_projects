int LDR1AlarmValue = 875;
int LDR2AlarmValue = 864;
int LDR3AlarmValue = 850;
int MQ7AlarmValue = 800;
int MQ7ValuePrev = 700;

//right motor
int Pin0 = 46; 
int Pin1 = 48; 
int Pin2 = 50; 
int Pin3 = 52; 


//left motor
int Pin4 = 47;
int Pin5 = 49;
int Pin6 = 51;
int Pin7 = 53;

int PinDoor1 = 22;
int PinDoor2 = 24;
int PinDoor3 = 26;
int PinDoor4 = 28;



int CloseStopSensorL = 3;
int CloseStopSensorR = 2;

int OpenStopSensorL = 5;
int OpenStopSensorR = 4;

boolean CloseStopLeft = false;
boolean CloseStopRight = false;
boolean OpenStopLeft = false;
boolean OpenStopRight = false;


int mqx_analogPin = A0; // connected to the output pin of MQ-X
int LDR1Pin = A1;
int LDR2Pin = A2;
int LDR3Pin = A3;
int WaterSensorPin = 8;

int mqxValue = 0;
int LDR1Value = 0;
int LDR2Value = 0;
int LDR3Value = 0;
boolean WaterSensorValue = false;
boolean RainedOnce = false;

int GreenLedsPin = 6;
int RedLedsPin = 7;
int White1Pin = 9;
int White2Pin = 10;

int SliderPin = 14;
int ButtonLTPin = 18;
int ButtonLBPin = 17;
int ButtonRTPin = 15;
int ButtonRBPin = 16;

int ArduinoLed = 13;

boolean ServiceMode = false;
boolean Alarm = false;

long previousMillis = 0;        // will store last time LED was updated
long interval = 3000;           // interval at which to blink (milliseconds)



// TopStatus ;

enum TopStatusMode {
  Mid,
  Closed,
  Open
} TopStatus;

void setup() 
{ 
 Serial.begin(9600);
 pinMode(Pin0, OUTPUT);  
 pinMode(Pin1, OUTPUT);  
 pinMode(Pin2, OUTPUT);  
 pinMode(Pin3, OUTPUT);  
 
 pinMode(Pin4, OUTPUT);  
 pinMode(Pin5, OUTPUT);  
 pinMode(Pin6, OUTPUT);  
 pinMode(Pin7, OUTPUT); 
 
pinMode(PinDoor1, OUTPUT);  
pinMode(PinDoor2, OUTPUT);  
pinMode(PinDoor3, OUTPUT);  
pinMode(PinDoor4, OUTPUT); 
 
 
 pinMode(CloseStopSensorL, INPUT_PULLUP);
 pinMode(CloseStopSensorR, INPUT_PULLUP);
 
 pinMode(OpenStopSensorL, INPUT_PULLUP);
 pinMode(OpenStopSensorR, INPUT_PULLUP);
 
 pinMode (GreenLedsPin, OUTPUT);
 pinMode (RedLedsPin, OUTPUT);
 pinMode (ArduinoLed, OUTPUT);
 
 
 pinMode (SliderPin ,   INPUT_PULLUP);
 pinMode (ButtonLTPin , INPUT_PULLUP);
 pinMode (ButtonLBPin , INPUT_PULLUP);
 pinMode (ButtonRTPin , INPUT_PULLUP);
 pinMode (ButtonRBPin , INPUT_PULLUP);
 
 
 pinMode(WaterSensorPin, INPUT);
 
  digitalWrite (White1Pin, HIGH);
      digitalWrite (White2Pin, HIGH);
} 
 void loop() 
{ 
  CheckStopSensor();
  SliderCheck ();
  if (ServiceMode == true)
  {
    digitalWrite (ArduinoLed , HIGH);
    
    
    if (Alarm == true)
    {
       MoveMotor(-127, 3, PinDoor1,  PinDoor2, PinDoor3,  PinDoor4) ;
    }
    
       Alarm = false;
    //Aristero koubi panw anoigei aristerh skeph
    if (digitalRead(ButtonLTPin) ==  LOW)
    {
      if (OpenStopLeft == false)
      MoveMotor(5, 3, Pin4,  Pin5, Pin6,  Pin7) ;
    }
    //Aristero koubi katw kleinei aristerh skeph
    else if (digitalRead(ButtonLBPin) ==  LOW)
    {
      if (CloseStopLeft == false)
      MoveMotor(-5, 3, Pin4,  Pin5, Pin6,  Pin7) ;
    }
    //Deksi koubi panw anoigei deksia skeph
    if (digitalRead(ButtonRTPin) ==  LOW)
    {
      if (OpenStopRight == false)
      MoveMotor(-5, 3, Pin0,  Pin1, Pin2,  Pin3) ;
    }
    //Deksi koubi katw kleinei deksia skeph
    else if (digitalRead(ButtonRBPin) ==  LOW)
    {
      if (CloseStopRight == false)
      MoveMotor(5, 3, Pin0,  Pin1, Pin2,  Pin3) ;
    }
  }
  else if (ServiceMode == false)
  {
    digitalWrite (ArduinoLed , LOW);

    CheckGasSensor();
    CheckPhotoSensor1();
    CheckPhotoSensor2();
    CheckPhotoSensor3();
    CheckWaterSensor();
    CheckAlarm();
    CheckService();
    LightsOnOFF();
    
    if ((WaterSensorValue == false) && (RainedOnce == false))
    {
      RainedOnce = true;
       while(TopStatus != Closed )
       {

          CloseTop();
       }
    }
    
    else if ((WaterSensorValue == true) && (RainedOnce == true)) 
    {
       RainedOnce = false;
       while(TopStatus != Open )
       {
          OpenTop();
       }
    }
    
    if (LDR1Value > LDR1AlarmValue)
    {
      
      while (CloseStopRight == false)
      {
      CheckStopSensor();
      MoveMotor(5, 3, Pin0,  Pin1, Pin2,  Pin3) ;
      }
      
    }
    
    if (LDR2Value > LDR2AlarmValue)
    {
      while (CloseStopLeft == false)
      {
        CheckStopSensor();
        MoveMotor(-5, 3, Pin4,  Pin5, Pin6,  Pin7) ;
      }
    }
    
    if (LDR3Value > LDR3AlarmValue)
    {
      while (TopStatus != Closed)
      {
         CloseTop();
      }
    }
    
    if ((mqxValue - MQ7ValuePrev)  >= 20)
    {
      Alarm = true;
      TopStatus = Mid;
      while (TopStatus != Open)
      {
         OpenTop();
      }
    }
    
    
    if (Alarm == false) 
    {
     digitalWrite(RedLedsPin , HIGH);
     digitalWrite(GreenLedsPin , LOW);     
    }
    
    else if (Alarm == true) 
    {
     digitalWrite(RedLedsPin , LOW);
     digitalWrite(GreenLedsPin , HIGH);
     MoveMotor(127, 3, PinDoor1,  PinDoor2, PinDoor3,  PinDoor4) ;
     while (ServiceMode == false)
     {
        CheckGasSensor();
        CheckPhotoSensor1();
        CheckPhotoSensor2();
        CheckPhotoSensor3();
        CheckWaterSensor();
        CheckAlarm();
        CheckService();
        SliderCheck ();
     }
    }
  }
  
}


void SliderCheck ()
{
  if (digitalRead(SliderPin) == HIGH) ServiceMode = true;
  else if (digitalRead(SliderPin) == LOW) ServiceMode = false;
  
}
void CloseTop()
{
  CheckStopSensor();
  if ((CloseStopRight == false) && (CloseStopLeft == false))
  {
    DualMoveMotor(5, 3, Pin0,  Pin1, Pin2,  Pin3, Pin4,  Pin5, Pin6,  Pin7) ;
    TopStatus = Mid;
  }
  else if ((CloseStopRight == false ) && (CloseStopLeft == true))
  {
    MoveMotor(5, 3, Pin0,  Pin1, Pin2,  Pin3) ;
    TopStatus = Mid;
  }
  else if ((CloseStopRight == true ) && (CloseStopLeft == false))
  {
    MoveMotor(-5, 3, Pin4,  Pin5, Pin6,  Pin7) ;
    TopStatus = Mid; 
  }
  else if ((CloseStopRight == true) && (CloseStopLeft = true))
  {
     TopStatus = Closed; 
  } 
}

void OpenTop()
{
  CheckStopSensor();
  if ((OpenStopRight == false) && (OpenStopLeft == false))
  {
    DualMoveMotor(-5, 3, Pin0,  Pin1, Pin2,  Pin3, Pin4,  Pin5, Pin6,  Pin7) ;
    TopStatus = Mid;
  }
  else if ((OpenStopRight == false ) && (OpenStopLeft == true))
  {
    MoveMotor(-5, 3, Pin0,  Pin1, Pin2,  Pin3) ;
    TopStatus = Mid;
  }
  else if ((OpenStopRight == true ) && (OpenStopLeft == false))
  {
    MoveMotor(5, 3, Pin4,  Pin5, Pin6,  Pin7) ;
    TopStatus = Mid;
  }
  else if ((OpenStopLeft == true) && (OpenStopRight = true))
  {
    TopStatus = Open; 
  } 
}

void CheckStopSensor()
{
  if (digitalRead(CloseStopSensorL) == LOW ) {CloseStopLeft = true;}
  else {CloseStopLeft = false;}
  if (digitalRead(CloseStopSensorR) == LOW ) {CloseStopRight = true;}
  else {CloseStopRight = false;}
  
  if (digitalRead(OpenStopSensorL) == HIGH ) {OpenStopLeft = true;}
  else {OpenStopLeft = false;}
  if (digitalRead(OpenStopSensorR) == HIGH ) {OpenStopRight = true;}
  else {OpenStopRight = false;}
}

void CheckGasSensor()
{
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > interval) 
       {
          MQ7ValuePrev = mqxValue;
          previousMillis = currentMillis;   
          mqxValue = analogRead(mqx_analogPin);
          Serial.print("mqxValue:");
          Serial.println(mqxValue);
          Serial.print("\t");
       }
  //delay(100); //Just here to slow down the output. 
}
void CheckPhotoSensor1()
{
  LDR1Value = analogRead(LDR1Pin);
  Serial.print("LDR1:");
  Serial.print(LDR1Value);
  Serial.print("\t");

  //delay(100); //Just here to slow down the output. 
}
void CheckPhotoSensor2()
{
  LDR2Value = analogRead(LDR2Pin);
  Serial.print("LDR2:");
  Serial.print(LDR2Value);
  Serial.print("\t");
  //Serial.println(LDR2Value);

  //delay(100); //Just here to slow down the output. 
}
void CheckPhotoSensor3()
{
  LDR3Value = analogRead(LDR3Pin);
  Serial.print("LDR3:");
  Serial.print(LDR3Value);
  Serial.print("\t");
  //Serial.println(LDR3Value);

  //delay(100); //Just here to slow down the output. 
}

void CheckAlarm()
{
  Serial.print("Alarm:");
  if (Alarm == true) Serial.print("true");
  else  Serial.print("false");
  Serial.print("\t");
  //Serial.println(LDR3Value);

  //delay(100); //Just here to slow down the output. 
}

void CheckService()
{
  Serial.print("Service:");
  if (ServiceMode == true) Serial.print("true");
  else  Serial.print("false");
  Serial.print("\t");
}

boolean CheckWaterSensor()
{
  WaterSensorValue = digitalRead(WaterSensorPin);
}

void LightsOnOFF()
{
  if ((TopStatus == Open) && (LDR3Value > 600))
  {     
    digitalWrite (White1Pin, LOW);
    digitalWrite (White2Pin, LOW);
  }
  else if ((TopStatus == Open) && (LDR3Value < 600))
  {
    digitalWrite (White1Pin, HIGH);
    digitalWrite (White2Pin, HIGH);
  }
  if (TopStatus == Closed) 
  {     
    digitalWrite (White1Pin, HIGH);
    digitalWrite (White2Pin, HIGH);
  }
}

void DualMoveMotor(int steps, int SpeedDelay, int Pin0, int Pin1, int Pin2, int Pin3, int Pin4, int Pin5, int Pin6, int Pin7) 
{ 
  if (steps > 0)
  {
    for (int i = 0; i < steps; i++) 
    { 
      for (int j = 0 ; j<=3 ; j++)
      {
       switch(j)
       { 
         case 0: 
           digitalWrite(Pin0, HIGH);  
           digitalWrite(Pin1, LOW); 
           digitalWrite(Pin2, LOW); 
           digitalWrite(Pin3, LOW); 
           
           digitalWrite(Pin4, LOW);  
           digitalWrite(Pin5, LOW); 
           digitalWrite(Pin6, LOW); 
           digitalWrite(Pin7, HIGH); 
         break;  
         case 1: 
           digitalWrite(Pin0, LOW);  
           digitalWrite(Pin1, HIGH); 
           digitalWrite(Pin2, LOW); 
           digitalWrite(Pin3, LOW); 
           
           digitalWrite(Pin4, LOW);  
           digitalWrite(Pin5, LOW); 
           digitalWrite(Pin6, HIGH); 
           digitalWrite(Pin7, LOW); 
           
         break;  
         case 2: 
           digitalWrite(Pin0, LOW);  
           digitalWrite(Pin1, LOW); 
           digitalWrite(Pin2, HIGH); 
           digitalWrite(Pin3, LOW); 
           
           digitalWrite(Pin4, LOW);  
           digitalWrite(Pin5, HIGH); 
           digitalWrite(Pin6, LOW); 
           digitalWrite(Pin7, LOW); 
         break;  
         case 3: 
           digitalWrite(Pin0, LOW);  
           digitalWrite(Pin1, LOW); 
           digitalWrite(Pin2, LOW); 
           digitalWrite(Pin3, HIGH); 
           
           digitalWrite(Pin4, HIGH);  
           digitalWrite(Pin5, LOW); 
           digitalWrite(Pin6, LOW); 
           digitalWrite(Pin7, LOW); 
         break;  
         
        }
        delay(SpeedDelay); 
     }
     digitalWrite(Pin0, LOW);  
     digitalWrite(Pin1, LOW); 
     digitalWrite(Pin2, LOW); 
     digitalWrite(Pin3, LOW); 
     
     digitalWrite(Pin4, LOW);  
     digitalWrite(Pin5, LOW); 
     digitalWrite(Pin6, LOW); 
     digitalWrite(Pin7, LOW); 
  }
  }
  else if (steps < 0)
  {
    steps = abs(steps);
    for (int i = 0; i < steps; i++) 
    { 
      for (int j = 3 ; j>=0 ; j--)
      {
       switch(j)
       { 
         case 0: 
           digitalWrite(Pin0, HIGH);  
           digitalWrite(Pin1, LOW); 
           digitalWrite(Pin2, LOW); 
           digitalWrite(Pin3, LOW); 
           
           digitalWrite(Pin4, LOW);  
           digitalWrite(Pin5, LOW); 
           digitalWrite(Pin6, LOW); 
           digitalWrite(Pin7, HIGH); 
         break;  
         case 1: 
           digitalWrite(Pin0, LOW);  
           digitalWrite(Pin1, HIGH); 
           digitalWrite(Pin2, LOW); 
           digitalWrite(Pin3, LOW); 
           
           digitalWrite(Pin0, LOW);  
           digitalWrite(Pin1, HIGH); 
           digitalWrite(Pin2, LOW); 
           digitalWrite(Pin3, LOW); 
           
           digitalWrite(Pin4, LOW);  
           digitalWrite(Pin5, LOW); 
           digitalWrite(Pin6, HIGH); 
           digitalWrite(Pin7, LOW); 
         break;  
         case 2: 
           digitalWrite(Pin0, LOW);  
           digitalWrite(Pin1, LOW); 
           digitalWrite(Pin2, HIGH); 
           digitalWrite(Pin3, LOW); 
           
           digitalWrite(Pin4, LOW);  
           digitalWrite(Pin5, HIGH); 
           digitalWrite(Pin6, LOW); 
           digitalWrite(Pin7, LOW); 
         break;  
         case 3: 
           digitalWrite(Pin0, LOW);  
           digitalWrite(Pin1, LOW); 
           digitalWrite(Pin2, LOW); 
           digitalWrite(Pin3, HIGH); 
           
           digitalWrite(Pin4, HIGH);  
           digitalWrite(Pin5, LOW); 
           digitalWrite(Pin6, LOW); 
           digitalWrite(Pin7, LOW); 
         break;  
        }
        delay(SpeedDelay); 
     }
     digitalWrite(Pin0, LOW);  
     digitalWrite(Pin1, LOW); 
     digitalWrite(Pin2, LOW); 
     digitalWrite(Pin3, LOW); 
     
     digitalWrite(Pin4, LOW);  
     digitalWrite(Pin5, LOW); 
     digitalWrite(Pin6, LOW); 
     digitalWrite(Pin7, LOW); 
    }
  }
}

void MoveMotor(int steps, int SpeedDelay, int Pin0, int Pin1, int Pin2, int Pin3 ) 
{ 
  if (steps > 0)
  {
    for (int i = 0; i < steps; i++) 
    { 
      for (int j = 0 ; j<=3 ; j++)
      {
       switch(j)
       { 
         case 0: 
           digitalWrite(Pin0, HIGH);  
           digitalWrite(Pin1, LOW); 
           digitalWrite(Pin2, LOW); 
           digitalWrite(Pin3, LOW); 
         break;  
         case 1: 
           digitalWrite(Pin0, LOW);  
           digitalWrite(Pin1, HIGH); 
           digitalWrite(Pin2, LOW); 
           digitalWrite(Pin3, LOW); 
         break;  
         case 2: 
           digitalWrite(Pin0, LOW);  
           digitalWrite(Pin1, LOW); 
           digitalWrite(Pin2, HIGH); 
           digitalWrite(Pin3, LOW); 
         break;  
         case 3: 
           digitalWrite(Pin0, LOW);  
           digitalWrite(Pin1, LOW); 
           digitalWrite(Pin2, LOW); 
           digitalWrite(Pin3, HIGH); 
         break;  
        }
        delay(SpeedDelay); 
       }
       
       digitalWrite(Pin0, LOW);  
       digitalWrite(Pin1, LOW); 
       digitalWrite(Pin2, LOW); 
       digitalWrite(Pin3, LOW); 
    }
  }
  else if (steps < 0)
  {
    steps = abs(steps);
    for (int i = 0; i < steps; i++) 
    { 
      for (int j = 3 ; j>=0 ; j--)
      {
       switch(j)
       { 
         case 0: 
           digitalWrite(Pin0, HIGH);  
           digitalWrite(Pin1, LOW); 
           digitalWrite(Pin2, LOW); 
           digitalWrite(Pin3, LOW); 
         break;  
         case 1: 
           digitalWrite(Pin0, LOW);  
           digitalWrite(Pin1, HIGH); 
           digitalWrite(Pin2, LOW); 
           digitalWrite(Pin3, LOW); 
         break;  
         case 2: 
           digitalWrite(Pin0, LOW);  
           digitalWrite(Pin1, LOW); 
           digitalWrite(Pin2, HIGH); 
           digitalWrite(Pin3, LOW); 
         break;  
         case 3: 
           digitalWrite(Pin0, LOW);  
           digitalWrite(Pin1, LOW); 
           digitalWrite(Pin2, LOW); 
           digitalWrite(Pin3, HIGH); 
         break;  
         
        }
        delay(SpeedDelay); 
     }
     digitalWrite(Pin0, LOW);  
     digitalWrite(Pin1, LOW); 
     digitalWrite(Pin2, LOW); 
     digitalWrite(Pin3, LOW); 
    }
  }
}


