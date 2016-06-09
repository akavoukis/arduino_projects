/*
  Arduino & OpenEnergyMonitor 
  
  This sketch connects to an emoncms server and makes a request using
  Arduino Ethernet shield (or other based on Wiznet W5100) or an 
  Arduino Wifi Shield
  
  author Mirco Piccin aka pitusso
  
  based on 
  http://arduino.cc/en/Tutorial/WebClientRepeating
*/

char foo;  //without a simple variable declaration, use of #ifdef at the top of your code raises an error!

//boolean GSMnotConnected = true;

//if using a W5100 based Ethernet shield, comment out the following line; 
//leave untouched if using Arduino Wifi Shield
//#define WIFI


//kanei include tis vivliothikes gia to ethernet to gsm k to aisthithra
#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h>

// Include Emon Library
#include "EmonLib.h"                   

//network configuration, WIRED or WIFI

  //if using WIRED
  byte mac[] = {0x90, 0xA2, 0xDA, 0x00, 0x69, 0xD5};
  
  // fill in an available IP address on your network here,
  // for auto configuration:
  IPAddress ip(10, 10, 10, 187);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress DNS(8, 8, 8, 8);
  IPAddress gw(10, 10, 10, 107);
  EthernetClient client;

// Sensor pins
const int currentSensorPin = A2;

float realPower      = 0;
float apparentPower  = 0;
float powerFActor   = 0; 
float supplyVoltage  = 0;
float Irms = 0;

EnergyMonitor ct1;
const int LEDpin = 9;                                                    
int onModulePin= 9;

SoftwareSerial SIM900(7, 8); // configure software serial port


int bytesread = 0;
int  val = 0;

//Emoncms configurations
char server[] = "emoncms.org";     // name address for emoncms.org

String apikey = "c99907870f3eab4b699db677be5244e3"; //e-graphic

int node = 1; //if 0, not used

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 10*1000;  // delay between updates, in milliseconds
boolean SMSSend = false;

void setup() {
  // start serial port:
  Serial.begin(9600);

  // Display a welcome message
  Serial.println("Emoncms client starting...");

  
  // (ADC input, calibration, phase_shift)
  ct1.voltage(0, 228.268, 1.7);  
  // Calibration factor = CT ratio / burden resistance = (100A / 0.05A) / 33 Ohms = 60.606             
  ct1.current(1, 60.6);        // Current: input pin, calibration.
  //CT Ratio / Burden resistance = (100A / 0.05A) / 33 Ohms = 60.6 (for the emonTx Shield)

  // Setup indicator LED
  //pinMode(LEDpin, OUTPUT);                                              
  //digitalWrite(LEDpin, HIGH);        
  
    //ETHERNET START
  if (!Ethernet.begin(mac)) {
    // if DHCP fails, start with a hard-coded address:
    Serial.println("Failed to get an IP address using DHCP, forcing manually");
    Ethernet.begin(mac, ip, dns, gw, subnet);
  }
      //GSM  
  SIM900.begin(19200);  
  SMSPower_on(); 
  sendATcommand("AT+CMGD=0,4", "OK", 6000);   
  
  //SIM900powerUpOrDown();
  //delay(5000);  // give time to log on to network. 
  
  printStatus();
}

void loop() 
{
  
  
  
  if ((realPower < 20) & (SMSSend == true)) { SMSSend = false;}
  
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) 
  {
    char c = client.read();
    Serial.print(c);
  }

  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && lastConnected) 
  {
    Serial.println();
    Serial.println("Disconnecting...");
    client.stop();
  }
  
  // if you're not connected, and at least <postingInterval> milliseconds have
  // passed sinceyour last connection, then connect again and
  // send data:
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) 
  {
  //read sensors    
  ct1.calcVI(20,2000);                                                  
  ct1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
  realPower       = ct1.realPower;        //extract Real Power into variable
  apparentPower   = ct1.apparentPower;    //extract Apparent Power into variable
  powerFActor     = ct1.powerFactor;      //extract Power Factor into Variable
  supplyVoltage   = ct1.Vrms;             //extract Vrms into Variable
  Irms            = ct1.Irms;             //extract Irms into Variable  
  
  
  Serial.print("CT1 Realpower:"); 
  Serial.print(realPower);     
  Serial.print(" CT1 apparentPower:"); 
  Serial.print(apparentPower);
  Serial.print(" CT1 powerFactor:"); 
  Serial.print(powerFActor);
  Serial.print(" CT1 Vrms:"); 
  Serial.print(supplyVoltage);
  Serial.print(" CT1 Irms:"); 
  Serial.print(Irms);
  Serial.println();
      
    //send values
    sendData();
    SIM900ReadSMS();
   
    if ((realPower > 20) && (SMSSend == false))
    {
      String temp = "ALERT!!! Realpower is: " ;
      char charVal[10];    
      dtostrf(realPower, 5, 0, charVal);
      temp += charVal;
      temp += "W";
      Serial.println("");
      Serial.print("Sending sms: ");
      Serial.println(temp);

      SIM900SendSMS(temp);
      
      SMSSend = true;
    }
  }
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
}

// this method makes a HTTP connection to the server:
void sendData() {
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("Connecting...");
    // send the HTTP GET request:
    client.print("GET /api/post?apikey=");
    client.print(apikey);
    if (node > 0) {
      client.print("&node=");
      client.print(node);
    }
    client.print("&json={CT1Realpower");
    client.print(":");
    client.print(realPower);    
    client.print(",CT1apparentPower:");
    client.print(apparentPower);
    client.print(",CT1powerFactor:");
    client.print(powerFActor);
    client.print(",CT1supplyVoltage:");
    client.print(supplyVoltage);
    client.print(",CT1Irms:");
    client.print(Irms);   
    client.println("} HTTP/1.1");
    client.println("Host:emoncms.org");
    client.println("User-Agent: Arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } 
  else 
  {
    // if you couldn't make a connection:
    Serial.println("Connection failed");
    Serial.println("Disconnecting...");
    client.stop();
  }
}


void printStatus() {

    // print your local IP address:
    Serial.print("IP address: ");
    for (byte thisByte = 0; thisByte < 4; thisByte++) {
      // print the value of each byte of the IP address:
      Serial.print(Ethernet.localIP()[thisByte], DEC);
      Serial.print("."); 
    }  
  Serial.println();
}


void SIM900powerUpOrDown()
{
  pinMode(9, OUTPUT); 
  digitalWrite(9,LOW);
  delay(1000);
  digitalWrite(9,HIGH);
  delay(2000);
  digitalWrite(9,LOW);
  delay(3000);
}

void SIM900SendSMS(String Message)
{
  int answer = 1;

  delay(100);
  answer = sendATcommand("AT+CMGF=1\r", "OK", 6000);   
  delay(100);
  answer = sendATcommand("AT + CMGS = \"+306981608084\"", ">", 6000);   
  delay(100);
  SIM900.println(Message);        // message to send
  delay(100);
  SIM900.println((char)26);                       // End AT command with a ^Z, ASCII code 26
  delay(100); 
  SIM900.println();
  delay(100);                                     // give module time to send SMS
}

void SIM900ReadSMS()
{
  int answer;
  
  sendATcommand("AT+CMGF=1", "OK", 6000);   
  answer = sendATcommand("AT+CMGL=\"ALL\"", "UNREAD", 6000);   
  if (answer != 1)
  {
    Serial.println("No Messages");
  }
  else
  {
    String temp = "Information!!! Realpower is: " ;
    char charVal[10];    
    dtostrf(realPower, 5, 0, charVal);
    temp += charVal;
    temp += "W";
    Serial.println("");
    Serial.print("Sending sms: ");
    Serial.println(temp);

    SIM900SendSMS(temp);
    
    Serial.println("New Message arrived. Deleting");
    sendATcommand("AT+CMGD=0,4", "OK", 6000); 
  }

}

int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    memset(response, '\0', 100);    // Initialice the string

    delay(100);

    while( SIM900.available() > 0) SIM900.read();    // Clean the input buffer

    SIM900.println(ATcommand);    // Send the AT command 
    Serial.print("Sending: ");
    Serial.println(ATcommand);

        x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        // if there are data in the UART input buffer, reads it and checks for the asnwer
        if(SIM900.available() != 0)
        {    
            response[x] = SIM900.read();
            x++;
            // check if the desired answer is in the response of the module
            if (strstr(response, expected_answer) != NULL)    
            {
              
              answer = 1;
            }
        }
        // Waits for the asnwer with time out
    }
    while((answer == 0) && ((millis() - previous) < timeout));   
    Serial.println("");
    Serial.print("Response: ");
    Serial.print(response);
    Serial.print(" Expected ");
    Serial.println(expected_answer);
    return answer;
}

boolean SMSPower_on()
{

    uint8_t answer=0;
    
    // checks if the module is started
    answer = sendATcommand("AT", "OK", 2000);
    if (answer == 0)
    {
        // power on pulse
        digitalWrite(onModulePin,HIGH);
        delay(3000);
        digitalWrite(onModulePin,LOW);
    
        // waits for an answer from the module
        while(answer == 0)
        {     // Send AT every two seconds and wait for the answer
            digitalWrite(onModulePin,HIGH);
            delay(3000);
            digitalWrite(onModulePin,LOW);
            answer = sendATcommand("AT", "OK", 6000);    
        }
    }
}



