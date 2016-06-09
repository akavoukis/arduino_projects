//Kanei include tis vivliothikes gia to gsm, ton aisthithra thermokrasias kai thn LCD
#include <dht.h>
#include <GSM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

dht DHT;

#define DHT11_PIN 6
#define Relay 9
#define SoilPin A0
#define SoilDigitalPin 10
#define GSMPowerPin 7
#define PINNUMBER ""

//enum me tis entoles pou diavazei apo ta sms
enum CommandSet
{
  None,
  Auto,
  Read,
  Start,
  Stop,
  tensec,
};

//global metavlhtes gia oles tis methodous
boolean RelayStatus = false;
int SoilValue = 0;
boolean SoilDigitalValue = false;
GSM gsmAccess(true);
GSM_SMS sms;
boolean notConnected = true;

// ksekina thn lcd se dieuthinsh 0x27 kai dhmhourghse mia lcd me 16 xarakthres epi 2 grames
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
CommandSet  CommandRecieved = None;


//h setup trexei gia mia fora otan arxizei to programma
void setup()
{

  //dhlwnoume pia pins einai eksodoi
  // dinoume arxikh timh sto relay
  Serial.begin(9600);
  pinMode(Relay, OUTPUT);
  digitalWrite(Relay, HIGH);
  pinMode(GSMPowerPin, OUTPUT);

  // arxizei thn lcd
  lcd.init();                      // initialize the lcd

  // Print a message to the LCD.
  lcd.backlight();
  lcd.print("Thermokoipio");
  //dinei revma ston gsm kai ksekinaei na tou stelnei tis entoles ekinhshs 
  GSMPower_on();
  GSMStart();

}

//svinei to relay gia na stamathsei to  moter
void RelayOff()
{
  RelayStatus = HIGH;
  digitalWrite(Relay, HIGH);   // turn the LED on (HIGH is the voltage level)
}

//arxizei to relay gia na arxisei to moter
void RelayOn()
{
  RelayStatus = LOW;
  digitalWrite(Relay, LOW);   // turn the LED on (HIGH is the voltage level)
}

//allazei thn katastash sto relay
void RelayToggle()
{
  RelayStatus = !RelayStatus;
  digitalWrite(Relay, RelayStatus);   // turn the LED on (HIGH is the voltage level)
}

//dinei revma sto GSM power pin wste na anoiksei to gsm
void GSMPower_on()
{
  digitalWrite(GSMPowerPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(2000);
  digitalWrite(GSMPowerPin, LOW);   // turn the LED on (HIGH is the voltage level)
}

//ksekinaei to gsm dinontas tou to pin ths sim to opoio einai keno
void GSMStart()
{

  lcd.clear();
  lcd.print("Thermokoipio");
  lcd.setCursor(0, 1);
  lcd.print("Starting GSM");

  while (notConnected)
  {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY)
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }
}


//stelnei sms me metavlhth to keimeno pou tou vazoume
void mySendSMS(String txtMsg)
{

  lcd.clear();
  lcd.print("Sending SMS");

  char remoteNum[20] = {'+', '3', '0', '6', '9', '4', '8', '5', '5', '1', '2', '2', '0'}; // telephone number to send sms
  sms.beginSMS(remoteNum);
  sms.print(txtMsg);
  sms.endSMS();
}

//Diavazei thn thermokrasia kai thn deixnei sto serial monitor
void ReadTemp()
{
  // READ DATA Temperature
  //Serial.print("DHT11, \t");
  int chk = DHT.read11(DHT11_PIN);
  // DISPLAY DATA
  Serial.print("Temp: ");
  Serial.print(DHT.temperature, 1);
  Serial.print(" \t Humidity: ");
  Serial.print(DHT.humidity, 1);

}

//diavazei thn ygrasia tou xwmatos kai thn apothikevei sto SoilValue kai thn psifiakh timh kai thn apothikevei sto SoilDigitalValue
void ReadSoil()
{
  SoilValue = analogRead(SoilPin);
  SoilDigitalValue = digitalRead(SoilDigitalPin);
  Serial.print(" \t Soil Humidity: ");
  Serial.print(SoilValue);
}

//Diavazei thn katastash tou Relay kai thn emfanizei sto serial monitor
void ReadRelay()
{
  Serial.print(" \t Relay Status: ");
  if (RelayStatus == true)   Serial.println("ON");
  else   Serial.println("OFF");
}


//Psaxnei gia eiserxomena sms
//an yparxei tote to apothikevei mesa sthn metavlhth SMSRecieved
void ReadSMS()
{
  char SMSRecieved[200];
  // If there are any SMSs available()
  if (sms.available())
  {

    if (sms.peek() == '#')
    {
      Serial.println("Discarded SMS");
      sms.flush();
    }

    int i = 0;
    // Read message bytes and print them
    char c;
    while (c = sms.read())
    {
      SMSRecieved[i] = c;
      Serial.print(c);
      i++;
    }

    //Diavazei to sms apo thn metavlhth kai psaxnei ton xarakthra '$' an ton vrei diavazei an ta ypoloipa grammata
    //antistoixoun sta grammata twn entolwn.. px. $Auto an vrei mesa se ena sms tous xarakthres $Auto tote allazei thn 
    //metavlhth CommandRecieved kai tis dinei thn antistoixh timh
    String SMSRecievedString = SMSRecieved;
    int indexOfChar = 0;
    indexOfChar = SMSRecievedString.indexOf('$');

    if (SMSRecievedString.substring(indexOfChar + 1, indexOfChar + 5) == "Auto")
    {
      CommandRecieved = Auto;
    }
    else if (SMSRecievedString.substring(indexOfChar + 1, indexOfChar + 5) == "Read")
    {
      CommandRecieved = Read;

    }
    else if (SMSRecievedString.substring(indexOfChar + 1, indexOfChar + 6) == "Start")
    {
      CommandRecieved = Start;

    }
    else if (SMSRecievedString.substring(indexOfChar + 1, indexOfChar + 5) == "Stop")
    {
      CommandRecieved = Stop;

    }
    else if (SMSRecievedString.substring(indexOfChar + 1, indexOfChar + 7) == "tensec")
    {
      CommandRecieved = tensec;

    }
    
    //alliws "mhdenizei to command recieved dinontas tou timh NONE
    else
    {
      CommandRecieved = None;
    }
  

      // emfanizei sthn lcd pia entolh diavase kai pagwnei gia 2 sec oste na prolavoume na to diavasoume sthn othonh
      lcd.clear();  
      lcd.print("Received SMS");
      lcd.setCursor(0, 1);
      //lcd.print("COMMAND: ");
      if (CommandRecieved == Auto )  lcd.print("COMMAND: AUTO");
      if (CommandRecieved == Read )  lcd.print("COMMAND: Read");
      if (CommandRecieved == Start )  lcd.print("COMMAND: Start");
      if (CommandRecieved == Stop )  lcd.print("COMMAND: Stop");
      if (CommandRecieved == tensec )  lcd.print("COMMAND: tensec");
      delay(2000);


  Serial.println("\nEND OF MESSAGE");

  // SVINEI TO SMS apo thn sim
  sms.flush();
  Serial.println("MESSAGE DELETED");
}
}

//h actions sms pairnei thn apofash gia to ti akrivws tha kanei analoga me thn entolh pou diavase apo to sms
void ActionsSMS()
{

  //POTIZEI MEXRI NA DEI YGRASIA kai stelnei sms otan stamathsei to potisma
  if (CommandRecieved == Auto )
  {
    RelayOn();
    while (SoilDigitalValue != LOW)
    {
      ReadSoil();
    }
    RelayOff();
    String SMSTextToSend = "TO POTISMA STAMATHSE";
    mySendSMS(SMSTextToSend);
    CommandRecieved = None;
  }
  //MAS APANTAEI TIS METRHSEIS stelnontas sms
  else if (CommandRecieved == Read )
  {
    //Prosthestei sto sms to T: kai thn thermokrasia
    String SMSTextToSend;
    SMSTextToSend = "T: ";

    char    buffChar[20];
    double temp = DHT.temperature;
    dtostrf(temp, 2, 2, buffChar);
    String tempAsString = buffChar;
    SMSTextToSend += tempAsString;
    SMSTextToSend += " ";

    //prosthestei sto sms H: kai thn ygrasia
    SMSTextToSend += "H: ";

    char  buffChar2[20];
    double humi = DHT.humidity;
    dtostrf(humi, 5, 2, buffChar2);
    String humiAsString = buffChar2;
    SMSTextToSend += humiAsString;
    SMSTextToSend += " ";

    //prosthetei sto SMS to S: kai thn timh ugrasias tou xwmatos
    SMSTextToSend += "S: ";
    SMSTextToSend += SoilValue;

    Serial.println(SMSTextToSend);
    
    //STELNEI TO SMS POU apothikeusame prohgoumenos
    mySendSMS(SMSTextToSend);
    CommandRecieved = None;
  }
  //START POTISMA anoigei to relay kai ksekinaei to moter stelnei epishs sms oti ksekinhse to potisma
  else if (CommandRecieved == Start )
  {
    RelayOn();
    String SMSTextToSend = "TO POTISMA KSEKINHSE";
    mySendSMS(SMSTextToSend);
    CommandRecieved = None;
  }
  //FINISH POTISMA kleinei to relay kai stamataei to potisma epishs stelneis sms oti stamathse to potisma
  else if (CommandRecieved == Stop )
  {
    RelayOff();
    String SMSTextToSend = "TO POTISMA STAMATHSE";
    mySendSMS(SMSTextToSend);
    CommandRecieved = None;
  }
  //TENSEC ksekinaei na potizei gia 10 sec kai meta stamataei
  else if ( CommandRecieved == tensec)
  {
    RelayOn();
    delay(10000);
    RelayOff();
    CommandRecieved = None;
  }


}

//Emfanizei sthn lcd tis metrhseis pou diavazei
void  DisplayLCD()
{

  lcd.clear();
  String tempString = "Temp: ";
  char    buffChar[20];
  double temp = DHT.temperature;
  dtostrf(temp, 2, 2, buffChar);
  String tempAsString = buffChar;
  tempString += tempAsString;
  lcd.print(tempString);

  lcd.setCursor(0, 1);
  String SoilString = "Soil: ";
  SoilString += SoilValue;
  lcd.print(SoilString);

}

//kentrikh loop pou den stamataei na ekteleitai pote
void loop()
{
  //Diavazei tis metrhseis
  ReadTemp();
  ReadSoil();
  ReadRelay();
  //psaxnei na vrei an exei erthei sms
  ReadSMS();
  //ektelei tis entoles tou sms
  ActionsSMS();
  //emfanizei sthn othonh tis metrhseis
  DisplayLCD();
  //perimenei 1 sec xwris na kanei tpt
  delay(1000);
}

