//oi times pou an kseperasastoun tha energopoihthei o aisthithras
int LDR1AlarmValue = 875;
int LDR2AlarmValue = 864;
int LDR3AlarmValue = 850;
int MQ7AlarmValue = 800;



//pins pou einai syndedemo to right motor
int Pin0 = 46; 
int Pin1 = 48; 
int Pin2 = 50; 
int Pin3 = 52; 


//pins pou einai syndedemeno to left motor
int Pin4 = 47;
int Pin5 = 49;
int Pin6 = 51;
int Pin7 = 53;

//pins pou einai syndedemeno to moter ths portas
int PinDoor1 = 22;
int PinDoor2 = 24;
int PinDoor3 = 26;
int PinDoor4 = 28;


//pins pou einai syndedemenoi oi diakoptes ths orofhs
int CloseStopSensorL = 3;
int CloseStopSensorR = 2;
int OpenStopSensorL = 5;
int OpenStopSensorR = 4;

// arxikes times gia tous diakoptes ths orofhs
boolean CloseStopLeft = false;
boolean CloseStopRight = false;
boolean OpenStopLeft = false;
boolean OpenStopRight = false;

// dhlwsh twn aisthithrwn
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

//pins gia ta led
int GreenLedsPin = 6;
int RedLedsPin = 7;
int White1Pin = 9;
int White2Pin = 10;

//pins gia ta buttons
int SliderPin = 14;
int ButtonLTPin = 18;
int ButtonLBPin = 17;
int ButtonRTPin = 15;
int ButtonRBPin = 16;

int ArduinoLed = 13;

boolean ServiceMode = false;
boolean Alarm = false;

//enumeration gia tis katastashs ths skephs
//typou topstatusmode gia tis katastaseis ths skephs
// TopStatus ;
enum TopStatusMode {
  Mid,
  Closed,
  Open
} TopStatus;

//trexei mia fora mono gia thn arxikopoihsh twn timwn
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
 
 //gia na ksekinakane ta lefka ledakia on
  digitalWrite (White1Pin, HIGH);
  digitalWrite (White2Pin, HIGH);
} 
 void loop() 
{ 
  //opoio mode service h kanoniko einai energo na diavazei to switch gia na borei na allaksei to mode
  //kai se oles periptoseis na kanei update tis times gia ta stop buttons
  CheckStopSensor();
  SliderCheck ();
  
  
  //an einai se service mode
  if (ServiceMode == true)
  {
    //anavei to led tou arduino sto pin 13 gia na kseroume oti eimaste se service mode
    digitalWrite (ArduinoLed , HIGH);
    
    
    // an eixe xtyphsei alarm prin boume se service mode tote ksanagyrnaw to moter ths portas sto arxiko tou shmeio
    //kai mhdenizei to alarm
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
  
  // kanoniko mode
  else if (ServiceMode == false)
  {
    // grafei pali sto led oti den einai se service mode
    digitalWrite (ArduinoLed , LOW);

    // diavazei oles tis times apo tous aisthithres
    CheckGasSensor();
    CheckPhotoSensor1();
    CheckPhotoSensor2();
    CheckPhotoSensor3();
    CheckWaterSensor();
    CheckAlarm();
    CheckService();
    LightsOnOFF();
    
    // an exei xtyphsei o aisthirhras vroxhs 
    //OSO h stegh den einai kleisth thn kleinei
    // an eixe xtyphsei o aisthithras prohgoumenos den thn kleinei
    if ((WaterSensorValue == false) && (RainedOnce == false))
    {
      RainedOnce = true;
       while(TopStatus != Closed )
       {

          CloseTop();
       }
    }
    
    // an eixe xtyphsei o aisthithras vroxhs kai pleon den xtypaei.. tote thn anoigei
    else if ((WaterSensorValue == true) && (RainedOnce == true)) 
    {
       RainedOnce = false;
       while(TopStatus != Open )
       {
          OpenTop();
       }
    }
    
    //elenxei oles tis photoantistaseis an oi times tous exoun kseperasei tis prokathorismenes times
    //analoga me ton aisthithra kleinei kai thn analogh skeph
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
    
    
    // elenxei an o aisthithras kapnou perase thn prokathorismenh timh
    // an synvei auto anoigei thn orofh kai thetei oti xtypaei alarm wste na ginoun k alles energeies
    if (mqxValue > MQ7AlarmValue)
    {
      Alarm = true;
      TopStatus = Mid;
      while (TopStatus != Open)
      {
         OpenTop();
      }
    }
    
    // an den xtypaei alarm anavei ta kokkina led
    if (Alarm == false) 
    {
     digitalWrite(RedLedsPin , HIGH);
     digitalWrite(GreenLedsPin , LOW);     
    }
    //alliws anavei ta prasina kai 
    //anoigei thn porta kai
    //den synexizei kamia leitourgeia mexri na bei se service mode
    //kathos perimenei gia to service mode synexizei na diavazei tous aisthithres wste na vlepoume sthn othonh
    //tis times twn aisthithrwn kai na vlepoume an o dikopths tou service mode allakse
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
///////////TELOS PROGRAMMATOS 


//SYNARTHSEIS
//elenxei kai apothikevei to service mode switch
void SliderCheck ()
{
  if (digitalRead(SliderPin) == HIGH) ServiceMode = true;
  else if (digitalRead(SliderPin) == LOW) ServiceMode = false;
  
}

//kleisimo skephs
//kounaei vhma vhma ta moter kathos synxronos elenxei an exoun paththei oi diakoptes
// se periptosh pou exei paththei mono enas apo tous 2 tote synexizei na kleinei mono ton enas apo tous 2 pou den exei kleisei akoma
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

//antoisthxh methodos gia na anoiksei me thn diafora oti gia tous diakoptes den elenxoun an exoun paththei alla an exoun
//stamathsei na patiountai gia na katalavoume oti exei anoiksei
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

//methodos pou elenxei tous diakoptes kai grafei sto status tous wste na xrhsimopoihthei apo tis alles methodous
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

//diavazei apo ton aisthithra kapnou analogika k to apothikevei
void CheckGasSensor()
{
  mqxValue = analogRead(mqx_analogPin);
  Serial.print("mqxValue:");
  Serial.println(mqxValue);
  Serial.print("\t");

}
// diavazei ola ta photoresistors
void CheckPhotoSensor1()
{
  LDR1Value = analogRead(LDR1Pin);
  Serial.print("LDR1:");
  Serial.print(LDR1Value);
  Serial.print("\t");
}
void CheckPhotoSensor2()
{
  LDR2Value = analogRead(LDR2Pin);
  Serial.print("LDR2:");
  Serial.print(LDR2Value);
  Serial.print("\t");
}
void CheckPhotoSensor3()
{
  LDR3Value = analogRead(LDR3Pin);
  Serial.print("LDR3:");
  Serial.print(LDR3Value);
  Serial.print("\t");
}

void CheckAlarm()
{
  Serial.print("Alarm:");
  if (Alarm == true) Serial.print("true");
  else  Serial.print("false");
  Serial.print("\t");
}

void CheckService()
{
  Serial.print("Service:");
  if (ServiceMode == true) Serial.print("true");
  else  Serial.print("false");
  Serial.print("\t");
}

//epistefei ena bool gia to status tou aisthithra vroxhs
boolean CheckWaterSensor()
{
  WaterSensorValue = digitalRead(WaterSensorPin);
}

//Diavazei apo thn LDR3 (mid) kai an h fothrnothta einai megalyterh apo 600 tote svinei ta lefka led
//alliws na anavei 
// alla se periptosh pou h skeph einai kleisth ta krataei anamena
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




// h motor move pairnei ws metavlhtes ton aritmo twn vhmaton pou theloume na kanei
//thn taxythta pou tha kanei tis enalages
// kai ta pins pou tha xrhsimopoiei gia na metakinhsei to moter

//se periptosh pou ta vhmata pou ths dinoume einai thetika tote kanei tosa group enalagwn osa einai ta vhmata me mia apo tis 2 fores(meries)
// an ta vhmata einai arnhtika tote kanei tis enalages antitheta apo thn prohgoumenh fora... alla xrhsimopoiei thn apolyth timh gia na metrhsei 
// ta vhmata. to speed delay xrhsimopoieitai gia na exei kathisterhsh kathos allazei thn polikothta sta moter

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
      // o kwdikas einai idios me ton kwdika gia ta thetika me thn diafora oti anti gia na ta pernei thn seira 0-1-2-3 ta ektelei me thn seira 3-2-1-0 wste na kanei
      // thn antistrofh kinhsh
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


//h dual motor kanei akrivws oti kai h move motor me thn difora oti gia kathe vhma xrhsimopoiei kai ta dyo moter me thn idia kinhsh
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

