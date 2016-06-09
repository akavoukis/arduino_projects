int Pin0 = 7; 
int Pin1 = 6; 
int Pin2 = 5; 
int Pin3 = 4; 
void setup() 
{ 
 pinMode(Pin0, OUTPUT);  
 pinMode(Pin1, OUTPUT);  
 pinMode(Pin2, OUTPUT);  
 pinMode(Pin3, OUTPUT);  
} 
 void loop() 
{ 
  MoveMotor(-100, 5, Pin0,  Pin1, Pin2,  Pin3) ;
  //delay(1000);
  //MoveMotor(100, 10, Pin0,  Pin1, Pin2,  Pin3) ;
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


