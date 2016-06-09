#include <Stepper.h>
#include <NewPing.h>

//pins gia ta ultrasonic rangers
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
const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution

//objects gia ta moter ths vivliothikhs stepper
Stepper StepperL(stepsPerRevolution, 22,24,26,28);            
Stepper StepperM(stepsPerRevolution, 30,32,34,36);            
Stepper StepperR(stepsPerRevolution, 38,40,42,44);      



boolean blinkrun = false;


String inputString = "";         
boolean stringComplete = false;  

boolean Medicine1Full = true;
boolean Medicine2Full = true;
boolean Medicine3Full = true;

int LDistance = 0;
int MDistance = 0;
int RDistance = 0;

//objects gia ta ultrasonic ths vivliothikhs NewPing
NewPing Lsonar(LTRIGGER_PIN, LECHO_PIN, LMAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing Msonar(MTRIGGER_PIN, MECHO_PIN, MMAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing Rsonar(RTRIGGER_PIN, RECHO_PIN, RMAX_DISTANCE); // NewPing setup of pins and maximum distance.
      

void setup()
{
 
  Serial.begin(115200);
  inputString.reserve(200);
  pinMode(led, OUTPUT);  
}

void loop()
{
//kathe fora pou trexei to loop 
// stelnei ena status ok sto pc
SendStatusOk();
//diavazei ta data pou tou stelnei to pc
ReadData();
//Diavazei ta epipeda apo tous ultrasonic
ReadMedicineLevels();
//stelnei to status piso sto pc
SendStatusMedicineLevels();
delay(500);  
}

//methodos pou ekteleitai kathe fora pou erxontai dedomena sto serial com
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 

    if (inChar == '\n') 
    {
      //an erthei to \n dhl to ender (kenouria grammh) thn diaxwrizei
      stringComplete = true;
    }
    else
    {
      //an den diavasei kenouria grammh prosthetei ton xarakthra pou esteile to pc sto string
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


// Methodos read data diavazei to string pou tou stelnei to pc kai analoga me to string
// kanei kai thn antistoixh leitourgeia
void ReadData()
{
  if (stringComplete) 
  {
    Serial.println(inputString); 
    //dokimastiko... an to pc tou steilei L anavei to led
    if (inputString == "L")
    {
     blinkrun = true;
    }
    //sto LLeft kounaei to aristero moter 10 vhmata aristera.
    //xrhsimopoieitai gia calibration
    else if (inputString == "LLeft")
    {
        StepperL.step(10);
    }
    //sto LRight kounaei to aristero moter 10 vhmata deksia.
    //xrhsimopoieitai gia calibration
    else if (inputString == "LRight")
    {
        StepperL.step(-10);
    }
    //sto MLeft kounaei to kentriko moter 10 vhmata aristera.
    //xrhsimopoieitai gia calibration
    else if (inputString == "MLeft")
    {
        StepperM.step(10);
    }
    //sto MRight kounaei to kentriko moter 10 vhmata deksia.
    //xrhsimopoieitai gia calibration
    else if (inputString == "MRight")
    {              
        StepperM.step(-10);
    }
    //sto RLeft kounaei to kentriko moter 10 vhmata aristera.
    //xrhsimopoieitai gia calibration
    else if (inputString == "RLeft")
    {
        StepperR.step(10);
    }
    //sto RRight kounaei to deksi moter 10 vhmata deksia.
    //xrhsimopoieitai gia calibration
    else if (inputString == "RRight")
    {
       StepperR.step(-10);
    }
    
    //se periptosh pou h edolh pou erxetai apo to pc periexei thn leksi motor
    // tote tha kanei oloklhrh peristrofh tou moter analoga
    // me to pio moter exei epilextei
    // px motor1 motor2 h motor 3
    //epishs diavazei kai ton arithmo twn peristorfwn xwrizontas ta me to symvolo +
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
           StepperL.step(360);
          delay(1000);
        }
      }
      else if (firstString == "Motor2")
      {
        for (int i=0;i<secondAsInt;i++)
        {
           StepperM.step(360);
          delay(1000);
        }
      }
      else if (firstString == "Motor3")
      {
        for (int i=0;i<secondAsInt;i++)
        {
           StepperR.step(360);
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
// diavazei apo tous ultrasonic ta epipeda kai stelnei sto pc 
// an ta epipeda einai megalytera apo 7 mexri k 12 stelnei oti einai adeio
//alliws stelnei oti einai gemato
// merikes fores stelnei megala noumera k exoume apokleisei tis ypoloipes times
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

//ftianxei to string pou tha xrhsimopoihthei gia thn apostolh tou string sto pc 
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

