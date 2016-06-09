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

//if using a W5100 based Ethernet shield, comment out the following line; 
//leave untouched if using Arduino Wifi Shield
//#define WIFI

#include <SPI.h>
#include <Ethernet.h>


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

//Calibrations
//const int volt = 220;
//const float ct_calibration = 29;
//const float temp_offset = 0;

// Sensor pins
const int tempSensorPin = A0;
const int lightSensorPin = A1;
const int currentSensorPin = A2;

float realPower      = 0;
float apparentPower  = 0;
float powerFActor   = 0; 
float supplyVoltage  = 0;
float Irms = 0;

EnergyMonitor ct1,ct2,ct3, ct4;
const int LEDpin = 9;                                                    



//Emoncms configurations
char server[] = "emoncms.org";     // name address for emoncms.org
//char server[] = "e-graphic.it";

//IPAddress server(213, 138, 101, 177);  // numeric IP for emoncms.org (no DNS)

//String apikey = "c0bc5860f43de0ea7ddbfeac88462237";  //api key cloud
String apikey = "c99907870f3eab4b699db677be5244e3"; //e-graphic

int node = 1; //if 0, not used

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 10*1000;  // delay between updates, in milliseconds

void setup() {
  // start serial port:
  Serial.begin(9600);

  // Display a welcome message
  Serial.println("Emoncms client starting...");

// Calibration factor = CT ratio / burden resistance = (100A / 0.05A) / 33 Ohms = 60.606
  
  //ct1.current(1, 111.1);       // Current: input pin, calibration.
  
  //ct2.current(2, 60.606);                                     
  //ct3.current(3, 60.606);
  //ct4.current(4, 60.606); 
  
  // (ADC input, calibration, phase_shift)
  ct1.voltage(0, 228.268, 1.7);        
  //ct1.voltage(1, 234.26, 1.7);  // Voltage: input pin, calibration, phase_shift  
                            
  
  //ct2.voltage(0, 300.6, 1.7);                                
  //ct3.voltage(0, 300.6, 1.7);
  //ct4.voltage(0, 300.6, 1.7);

  //ct1.voltage(2, 234.26, 1.7);  // Voltage: input pin, calibration, phase_shift
  ct1.current(1, 60.6);        // Current: input pin, calibration.
  //CT Ratio / Burden resistance = (100A / 0.05A) / 33 Ohms = 60.6 (for the emonTx Shield)

  // Setup indicator LED
  pinMode(LEDpin, OUTPUT);                                              
  digitalWrite(LEDpin, HIGH);        
  
    //ETHERNET START
  if (!Ethernet.begin(mac)) {
    // if DHCP fails, start with a hard-coded address:
    Serial.println("Failed to get an IP address using DHCP, forcing manually");
    Ethernet.begin(mac, ip, dns, gw, subnet);
  }


  printStatus();
}

void loop() {
  
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("Disconnecting...");
    client.stop();
  }
  
  // if you're not connected, and at least <postingInterval> milliseconds have
  // passed sinceyour last connection, then connect again and
  // send data:
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
   
    //read sensors
  
    
  ct1.calcVI(20,2000);                                                  
  //ct2.calcVI(20,2000);
  //ct3.calcVI(20,2000);
  //ct4.calcVI(20,2000);
  
  ct1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
  //ct2.serialprint();
  //ct3.serialprint();
  //ct4.serialprint();
  
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
  else {
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
