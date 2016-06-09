int Pin0 = 31; 
int Pin1 = 33; 
int Pin2 = 35; 
int Pin3 = 37; 

int Pin4 = 39;
int Pin5 = 41;
int Pin6 = 43;
int Pin7 = 45;
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
} 
 void loop() 
{ 
 
  DualMoveMotor(200, 3, Pin0,  Pin1, Pin2,  Pin3, Pin4,  Pin5, Pin6,  Pin7) ;
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
         default: 
           digitalWrite(Pin0, LOW);  
           digitalWrite(Pin1, LOW); 
           digitalWrite(Pin2, LOW); 
           digitalWrite(Pin3, LOW); 
         break;  
        }
        delay(SpeedDelay); 
     }
  }
  }
}


