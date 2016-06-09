#include <Keypad.h>
#include <LiquidCrystal.h>

//kordela Motors
int Pin0 = 31; 
int Pin1 = 33; 
int Pin2 = 35; 
int Pin3 = 37; 

int Pin4 = 39;
int Pin5 = 41;
int Pin6 = 43;
int Pin7 = 45;
//End motors

//motor Sygolishs
int SygolishPin0 = 47;
int SygolishPin1 = 49;
int SygolishPin2 = 51;
int SygolishPin3 = 53;

int RedLed = 3;
int GreenLed= 4;
int YellowLed = 5;

int buzzer = 2;




//End motor Sygolishs

//Keyboard
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};
byte rowPins[ROWS] = {38, 36, 34, 32}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {30, 28, 26, 24}; //connect to the column pinouts of the keypad    

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
//End Keyboard

//LCD
LiquidCrystal lcd(52, 50, 48, 46, 44, 42);
int contrast = 12;                 // PWM pin (contrast level)

char key;
  /*The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)*/
//End LCD

//misc
int OnBoardLed = 13;
int SygolishLed = 8;
int SygolishTimer = 1000;
int counterSygoliseis = 0;
int TotalcounterSygoliseis = 0;
//OP MODE NORMAL 5
int OperationMode = 2; // 1 = normal operation  2= Service mode  10 = testing 4 = eject mode

//End misc
int InterruptPin = 18;

boolean OperationChanged = false;


void setup() 
{ 
 pinMode(Pin0, OUTPUT);  
 pinMode(Pin1, OUTPUT);  
 pinMode(Pin2, OUTPUT);  
 pinMode(Pin3, OUTPUT);  
 
 pinMode(Pin4, OUTPUT);  
 pinMode(Pin5, OUTPUT);  
 pinMode(Pin6, OUTPUT);  
 pinMode(Pin7, OUTPUT); 
 
 pinMode(SygolishPin0, OUTPUT);  
 pinMode(SygolishPin1, OUTPUT);  
 pinMode(SygolishPin2, OUTPUT);  
 pinMode(SygolishPin3, OUTPUT); 
 
 Serial.begin(9600);
 
 pinMode(RedLed, OUTPUT);  
 pinMode(GreenLed, OUTPUT);  
 pinMode(YellowLed, OUTPUT); 
 
 pinMode(InterruptPin,INPUT_PULLUP);

 pinMode(OnBoardLed, OUTPUT);
 pinMode(SygolishLed, OUTPUT);
 
 // set up the LCD's number of columns and rows: 
 analogWrite(contrast, 50);       // 0: maximum contrast  255: minimum
 lcd.begin(16, 2);
 
 attachInterrupt(5, ActivateService, CHANGE);
  ActivateService();
  
    pinMode(buzzer, OUTPUT);
} 

void ActivateService()
{  
  int test =  digitalRead(18);
  
  if (test == LOW)
  {

    OperationMode = 2;
  }
  else if (test == HIGH)
  {

     OperationMode = 5;
  }
}

void ColorLed(int color)
{
 switch (color)
 {
  case 0:
    digitalWrite(RedLed, LOW);
    digitalWrite(GreenLed, LOW);
    digitalWrite(YellowLed, LOW); 
    break;
  case 1:
    digitalWrite(RedLed, HIGH);
    digitalWrite(GreenLed, LOW);
    digitalWrite(YellowLed, LOW); 
    break;
  case 2:
    digitalWrite(RedLed, LOW);
    digitalWrite(GreenLed, HIGH);
    digitalWrite(YellowLed, LOW);
   break; 
  case 3:
    digitalWrite(RedLed, LOW);
    digitalWrite(GreenLed, LOW);
    digitalWrite(YellowLed, HIGH); 
    break; 

 }
}

void loop() 
{ 
  //Read Keypad
  char key = keypad.getKey();
  
//  OperationMode = 10;
  //Normal operation mode
  if (OperationMode == 1)
  {
    if (OperationMode != 1){ return;}
     
    ColorLed(2);
    key = keypad.getKey();
    digitalWrite(SygolishLed, LOW);
    
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("Scanning object");
    
    
    if (ObjectFoundIR() == true)
    {
     digitalWrite(OnBoardLed, HIGH); 
     OperationMode = 3;
     
    }
    else
    {
      digitalWrite(OnBoardLed, LOW); 
      DualMoveMotor(10, 5, Pin0,  Pin1, Pin2,  Pin3, Pin4,  Pin5, Pin6, Pin7) ; 
       
    }
    
   
  }
  //Service mode
  else if (OperationMode == 2)
  {
    
    if (OperationMode != 2){ return;}
    
    ColorLed(3);
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("Service Mode");
    lcd.setCursor(0,1); 
    lcd.print("<- 4    6 ->");
    
    char key = keypad.getKey();
    
    while (!key)
     {
      if (OperationMode != 2)
        {
          
          break;
        }
       key = keypad.getKey();
       
     }
     switch (key)
     {
      case '4':
              DualMoveMotor(-200, 5, Pin0,  Pin1, Pin2,  Pin3, Pin4,  Pin5, Pin6, Pin7) ; 

       break;
    
       case '6':
              DualMoveMotor(200, 5, Pin0,  Pin1, Pin2,  Pin3, Pin4,  Pin5, Pin6, Pin7) ; 

       break; 
     }
    
  }
  //choose Sygolish mode
  else if (OperationMode == 3)
  {
    if (OperationMode != 3){ return;}
    
    ColorLed(2);
    //key = keypad.getKey();
    
    //lcd
    lcd.setCursor(0,0); 
    lcd.print("Sygolish  !!!!!");
    //telos lcd

    MoveMotor(250, 3 , SygolishPin0,SygolishPin1,SygolishPin2,SygolishPin3);
    
    
    digitalWrite(SygolishLed, HIGH); 
    analogWrite(buzzer, 200);       // 0 turns it off
    delay(SygolishTimer);
    analogWrite(buzzer, 0);       // 0 turns it off
    digitalWrite(SygolishLed, LOW); 
     
    MoveMotor(-250, 3 , SygolishPin0,SygolishPin1,SygolishPin2,SygolishPin3);
    
    counterSygoliseis++;
    TotalcounterSygoliseis++;
    
    lcd.setCursor(0,0); 
    lcd.print("Sygolhseis:    ");
    lcd.setCursor(12,0); 
    lcd.print(counterSygoliseis);
    if (OperationMode != 3){return;}
    else{OperationMode = 4;}
  }
  //Eject Mode
  else if (OperationMode == 4)
  {
   if (OperationMode != 4){ return;}
   
    ColorLed(2);
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("Ejecting");
    
    key = keypad.getKey();
    
    DualMoveMotor(250, 5, Pin0,  Pin1, Pin2,  Pin3, Pin4,  Pin5, Pin6, Pin7) ; 
    //DualMoveMotor(250, 5, Pin0,  Pin1, Pin2,  Pin3, Pin4,  Pin5, Pin6, Pin7) ; 
    
    
    key = keypad.getKey();
    
    unsigned long StartedMillis = millis();
    while (!key)
     {
       key = keypad.getKey();
       unsigned long  temp = StartedMillis + 3000 - millis();
       lcd.clear();
       lcd.setCursor(0,0); 
       lcd.print("Press # to stop");
       lcd.setCursor(0,1); 
       lcd.print(temp); 
       delay(100);
       
       if (millis() > StartedMillis + 3000)
       {
         break;
       }
     }
     
     if (key == '#')
     {
        if (OperationMode != 4){return;} 
        else{OperationMode = 6;}
     }
     else
     {
       if (OperationMode != 4){return;} 
        else{OperationMode = 1;}
     }
  }
  //Menu Select
  else if (OperationMode == 5)
  {
    ColorLed(1);
    if (OperationMode != 5){ return;}
    
     counterSygoliseis = 0;
    
     char key = keypad.getKey();;
     lcd.clear();
     lcd.setCursor(0,0); 
     lcd.print("Epelekse A,B,C,D");
     lcd.setCursor(0,1); 
     lcd.print("Epilogh:");
     while (!key)
     {
       key = keypad.getKey();

        if (OperationMode != 5)
        {
        
          break;
        }
     }
     switch (key)
     {
      case 'A':
        lcd.clear();
        lcd.setCursor(0,0); 
        lcd.print("Epilekses A");
        SygolishTimer = 1000;
        lcd.setCursor(0,1); 
        lcd.print("Sygolish 1sec");
        delay(1000);
        OperationMode = 1;
       break;
    
       case 'B':
        lcd.clear();
        lcd.setCursor(0,0); 
        lcd.print("Epilekses B");
        SygolishTimer = 2000;
        lcd.setCursor(0,1); 
        lcd.print("Sygolish 2sec");
        delay(1000);
        OperationMode = 1;
       break; 
       
        case 'C':
        lcd.clear();
        lcd.setCursor(0,0); 
        lcd.print("Epilekses C");
        SygolishTimer = 3000;
        lcd.setCursor(0,1); 
        lcd.print("Sygolish 3sec");
        delay(1000);
        OperationMode = 1;
       break; 
       
        case 'D':
        lcd.clear();
        lcd.setCursor(0,0); 
        lcd.print("Epilekses D");
        SygolishTimer = 4000;
        lcd.setCursor(0,1); 
        lcd.print("Sygolish 4sec");
        delay(1000);        
        OperationMode = 1;
       break; 
       
       default:
         break;
         }
  }
  
  //test Display Total counter
  else if (OperationMode == 6)
  {
    
    if (OperationMode != 6){ return;}
    
    ColorLed(1);
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("Synolo");
    lcd.setCursor(0,1); 
    lcd.print("Sygolhsewn: ");
    lcd.print(TotalcounterSygoliseis);
    delay(3000);
     if (OperationMode != 6){return;}
     else {OperationMode = 5;}
  }
  
  //test mode
  else if (OperationMode == 10)
  {
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("Test mode");
     
    MoveMotor(200, 3 , SygolishPin0,SygolishPin1,SygolishPin2,SygolishPin3);
    
    
    //digitalWrite(SygolishLed, HIGH); 
    delay(SygolishTimer);
    //digitalWrite(SygolishLed, LOW); 
    MoveMotor(-200, 3 , SygolishPin0,SygolishPin1,SygolishPin2,SygolishPin3);
    delay(SygolishTimer);
  }
}


boolean ObjectFoundIR()
{
    int sensorValue = analogRead(A0);
     Serial.println(sensorValue);
    if (sensorValue <970)
      {
         return true;
         
      }
    else
      {
         return false;
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
           
           digitalWrite(Pin4, HIGH);  
           digitalWrite(Pin5, LOW); 
           digitalWrite(Pin6, LOW); 
           digitalWrite(Pin7, LOW); 
         break;  
         case 1: 
           digitalWrite(Pin0, LOW);  
           digitalWrite(Pin1, HIGH); 
           digitalWrite(Pin2, LOW); 
           digitalWrite(Pin3, LOW); 
           
           digitalWrite(Pin4, LOW);  
           digitalWrite(Pin5, HIGH); 
           digitalWrite(Pin6, LOW); 
           digitalWrite(Pin7, LOW); 
           
         break;  
         case 2: 
           digitalWrite(Pin0, LOW);  
           digitalWrite(Pin1, LOW); 
           digitalWrite(Pin2, HIGH); 
           digitalWrite(Pin3, LOW); 
           
           digitalWrite(Pin4, LOW);  
           digitalWrite(Pin5, LOW); 
           digitalWrite(Pin6, HIGH); 
           digitalWrite(Pin7, LOW); 
         break;  
         case 3: 
           digitalWrite(Pin0, LOW);  
           digitalWrite(Pin1, LOW); 
           digitalWrite(Pin2, LOW); 
           digitalWrite(Pin3, HIGH); 
           
           digitalWrite(Pin4, LOW);  
           digitalWrite(Pin5, LOW); 
           digitalWrite(Pin6, LOW); 
           digitalWrite(Pin7, HIGH); 
         break;  
         
        }
        delay(SpeedDelay); 
     }
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
           
           digitalWrite(Pin4, HIGH);  
           digitalWrite(Pin5, LOW); 
           digitalWrite(Pin6, LOW); 
           digitalWrite(Pin7, LOW); 
         break;  
         case 1: 
           digitalWrite(Pin0, LOW);  
           digitalWrite(Pin1, HIGH); 
           digitalWrite(Pin2, LOW); 
           digitalWrite(Pin3, LOW); 
           
           digitalWrite(Pin4, LOW);  
           digitalWrite(Pin5, HIGH); 
           digitalWrite(Pin6, LOW); 
           digitalWrite(Pin7, LOW); 
         break;  
         case 2: 
           digitalWrite(Pin0, LOW);  
           digitalWrite(Pin1, LOW); 
           digitalWrite(Pin2, HIGH); 
           digitalWrite(Pin3, LOW); 
           
           digitalWrite(Pin4, LOW);  
           digitalWrite(Pin5, LOW); 
           digitalWrite(Pin6, HIGH); 
           digitalWrite(Pin7, LOW); 
         break;  
         case 3: 
           digitalWrite(Pin0, LOW);  
           digitalWrite(Pin1, LOW); 
           digitalWrite(Pin2, LOW); 
           digitalWrite(Pin3, HIGH); 
           
           digitalWrite(Pin4, LOW);  
           digitalWrite(Pin5, LOW); 
           digitalWrite(Pin6, LOW); 
           digitalWrite(Pin7, HIGH); 
         break;  
         default:          
         break;  
        }
        delay(SpeedDelay); 
     }
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

void MoveMotor(int steps, int SpeedDelay, int Pins0, int  Pins1, int  Pins2, int  Pins3 ) 
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
           digitalWrite(Pins0, HIGH);  
           digitalWrite(Pins1, LOW); 
           digitalWrite(Pins2, LOW); 
           digitalWrite(Pins3, LOW); 
         break;  
         case 1: 
           digitalWrite(Pins0, LOW);  
           digitalWrite(Pins1, HIGH); 
           digitalWrite(Pins2, LOW); 
           digitalWrite(Pins3, LOW); 
         break;  
         case 2: 
           digitalWrite(Pins0, LOW);  
           digitalWrite(Pins1, LOW); 
           digitalWrite(Pins2, HIGH); 
           digitalWrite(Pins3, LOW); 
         break;  
         case 3: 
           digitalWrite(Pins0, LOW);  
           digitalWrite(Pins1, LOW); 
           digitalWrite(Pins2, LOW); 
           digitalWrite(Pins3, HIGH); 
         break;  
        }
        delay(SpeedDelay); 
     }
    }
    digitalWrite(Pins0, LOW);  
    digitalWrite(Pins1, LOW); 
    digitalWrite(Pins2, LOW); 
    digitalWrite(Pins3, LOW); 
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
           digitalWrite(Pins0, HIGH);  
           digitalWrite(Pins1, LOW); 
           digitalWrite(Pins2, LOW); 
           digitalWrite(Pins3, LOW); 
         break;  
         case 1: 
           digitalWrite(Pins0, LOW);  
           digitalWrite(Pins1, HIGH); 
           digitalWrite(Pins2, LOW); 
           digitalWrite(Pins3, LOW); 
         break;  
         case 2: 
           digitalWrite(Pins0, LOW);  
           digitalWrite(Pins1, LOW); 
           digitalWrite(Pins2, HIGH); 
           digitalWrite(Pins3, LOW); 
         break;  
         case 3: 
           digitalWrite(Pins0, LOW);  
           digitalWrite(Pins1, LOW); 
           digitalWrite(Pins2, LOW); 
           digitalWrite(Pins3, HIGH); 
         break;  
        }
        delay(SpeedDelay); 
     }
    }
    digitalWrite(Pins0, LOW);  
    digitalWrite(Pins1, LOW); 
    digitalWrite(Pins2, LOW); 
    digitalWrite(Pins3, LOW); 
  }
}


