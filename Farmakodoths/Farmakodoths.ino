#include <NewPing.h>

#define LTRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define LECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define LMAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define MTRIGGER_PIN  10  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define MECHO_PIN     9  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MMAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define RTRIGGER_PIN  8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define RECHO_PIN     6  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define RMAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

int led = 13;

int LPin0 = 22; 
int LPin1 = 24; 
int LPin2 = 26; 
int LPin3 = 28; 

int MPin0 = 30; 
int MPin1 = 32; 
int MPin2 = 34; 
int MPin3 = 36; 

int RPin0 = 38; 
int RPin1 = 40; 
int RPin2 = 42; 
int RPin3 = 44; 


boolean blinkrun = false;


String inputString = "";         
boolean stringComplete = false;  

boolean Medicine1Full = true;
boolean Medicine2Full = true;
boolean Medicine3Full = true;

int LDistance = 0;
int MDistance = 0;
int RDistance = 0;


NewPing Lsonar(LTRIGGER_PIN, LECHO_PIN, LMAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing Msonar(MTRIGGER_PIN, MECHO_PIN, MMAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing Rsonar(RTRIGGER_PIN, RECHO_PIN, RMAX_DISTANCE); // NewPing setup of pins and maximum distance.


void setup()
{
  pinMode(LPin0, OUTPUT);  
  pinMode(LPin1, OUTPUT);  
  pinMode(LPin2, OUTPUT);  
  pinMode(LPin3, OUTPUT);  
  
  pinMode(MPin0, OUTPUT);  
  pinMode(MPin1, OUTPUT);  
  pinMode(MPin2, OUTPUT);  
  pinMode(MPin3, OUTPUT);  
  
  pinMode(RPin0, OUTPUT);  
  pinMode(RPin1, OUTPUT);  
  pinMode(RPin2, OUTPUT);  
  pinMode(RPin3, OUTPUT);  
 
  Serial.begin(115200);
  inputString.reserve(200);
  pinMode(led, OUTPUT);  
}

void loop()
{

SendStatusOk();
ReadData();
ReadMedicineLevels();
SendStatusMedicineLevels();


//MoveMotor(-100, 5, Pin0,  Pin1, Pin2,  Pin3) ;


delay(500);  
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 

    if (inChar == '\n') 
    {
      stringComplete = true;
    }
    else
    {
      inputString += inChar;
    }
  }
}

void Blink() 
{
  digitalWrite(led, HIGH);   
  delay(1000);               
  digitalWrite(led, LOW);    
  delay(1000);               
  blinkrun = false;
}

void SendStatusOk()
{
    Serial.println("StatusOK");
}

void ReadData()
{
  if (stringComplete) 
  {
    Serial.println(inputString); 
    
    if (inputString == "L")
    {
     blinkrun = true;
    }
    else if (inputString == "LLeft")
    {
      MoveMotor(10, 5, LPin0, LPin1, LPin2, LPin3) ;
    }
    else if (inputString == "LRight")
    {
      MoveMotor(-10, 5, LPin0, LPin1, LPin2, LPin3) ;
    }
    else if (inputString == "MLeft")
    {
      MoveMotor(10, 5, MPin0, MPin1, MPin2, MPin3) ;
    }
    else if (inputString == "MRight")
    {
      MoveMotor(-10, 5, MPin0, MPin1, MPin2, MPin3) ;
    }
    else if (inputString == "RLeft")
    {
      MoveMotor(10, 5, RPin0, RPin1, RPin2, RPin3) ;
    }
    else if (inputString == "RRight")
    {
      MoveMotor(-10, 5, RPin0, RPin1, RPin2, RPin3) ;
    }
    
    if (inputString.substring(0,5) == "Motor")
    {
      int commaIndex = inputString.indexOf('+');
      String firstString = inputString.substring(0, commaIndex);
      String secondString = inputString.substring(commaIndex+1);
      int secondAsInt = secondString.toInt();
      if (firstString == "Motor1")
      {
        for (int i=0;i<secondAsInt;i++)
        {
          MoveMotor(-510, 5, LPin0, LPin1, LPin2, LPin3) ;
          delay(1000);
        }
      }
      else if (firstString == "Motor2")
      {
        for (int i=0;i<secondAsInt;i++)
        {
          MoveMotor(-510, 5, MPin0, MPin1, MPin2, MPin3) ;
          delay(1000);
        }
      }
      else if (firstString == "Motor3")
      {
        for (int i=0;i<secondAsInt;i++)
        {
          MoveMotor(-510, 5, RPin0, RPin1, RPin2, RPin3) ;
          delay(1000);
        }
      }
     }
       inputString = "";
       stringComplete = false;
  }
  
  if (blinkrun == true)
  {
    Blink();
  }
}

void ReadMedicineLevels()
{
  
  unsigned int LuS = Lsonar.ping(); // Send ping, get ping time in microseconds (uS).
  LDistance = Lsonar.convert_cm(LuS); // Convert ping time to distance and print result (0 = outside set distance range, no ping echo)
  
  if (LDistance > 7 && LDistance <= 12 )
  {
    Medicine1Full = false; //Sensor Read 
  }
  else
  {
    Medicine1Full = true; //Sensor Read 
  }
   
  unsigned int MuS = Msonar.ping(); // Send ping, get ping time in microseconds (uS).
  MDistance = Msonar.convert_cm(MuS); // Convert ping time to distance and print result (0 = outside set distance range, no ping echo)
  
  if (MDistance > 7 && MDistance <= 12)
  {
    Medicine2Full = false; //Sensor Read 
  }
  else
  {
    Medicine2Full = true; //Sensor Read 
  }
  
  unsigned int RuS = Rsonar.ping(); // Send ping, get ping time in microseconds (uS).
  RDistance = Rsonar.convert_cm(RuS); // Convert ping time to distance and print result (0 = outside set distance range, no ping echo)
  
  if (RDistance > 7 && RDistance <= 12)
  {
    Medicine3Full = false; //Sensor Read 
  }
  else
  {
    Medicine3Full = true; //Sensor Read 
  }
}

void SendStatusMedicineLevels()
{
  String stringmaker = "";
  if (Medicine1Full == true)
  {
    stringmaker = "Medicine1Full" ;
    stringmaker += " = true";
  }
  else
  {
    stringmaker = "Medicine1Full" ;
    stringmaker += " = false";
  }
  Serial.println(stringmaker); 
  
  
  stringmaker = "";
  if (Medicine2Full == true)
  {
    stringmaker = "Medicine2Full" ;
    stringmaker += " = true";
  }
  else
  {
    stringmaker = "Medicine2Full" ;
    stringmaker += " = false";
  }
  Serial.println(stringmaker); 
  
  
  stringmaker = "";
  if (Medicine3Full == true)
  {
    stringmaker = "Medicine3Full" ;
    stringmaker += " = true";
  }
  else
  {
    stringmaker = "Medicine3Full" ;
    stringmaker += " = false";
  }
  Serial.println(stringmaker); 
  
  stringmaker = "";
  stringmaker = "Medicine1Level=";
  stringmaker += LDistance;
  Serial.println(stringmaker);
  
  stringmaker = "";
  stringmaker = "Medicine2Level=";
  stringmaker += MDistance;
  Serial.println(stringmaker);
  
  stringmaker = "";
  stringmaker = "Medicine3Level=";
  stringmaker += RDistance;
  Serial.println(stringmaker);
  

}

boolean Contains(String s, String search)
{
  int max = s.length() - search.length(); // the searchstring has to fit in the other one  
  for (int i=0; i<= max; i++) 
  {
  if (s.substring(i) == search) return true;  // or i
  }
  return false;  //or -1
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



