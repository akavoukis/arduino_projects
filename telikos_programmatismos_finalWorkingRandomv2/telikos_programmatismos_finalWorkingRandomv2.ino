//xively
#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Xively.h>


//sensors
#include "DHT.h"
#define DHTPIN 6
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);



//lcd
#include <LiquidCrystal.h>
LiquidCrystal lcd(9, 3, 4, 5, 7, 8, 2);

//xively 
// MAC address for your Ethernet shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// Your Xively key to let you upload data
char xivelyKey[] = "PiVjZpdtB2vjARroz7xmeIM6o3uUx7Ho4XHwF91pENCedgIQ";

// Define the strings for our datastream IDs
char sensorId1[] = "Temperature";
char sensorId2[] = "Humidity";
char sensorId3[] = "Light";
char sensorId4[] = "Atmosphere";
char sensorId5[] = "CO";
char sensorId6[] = "Min_Temperature";
char sensorId7[] = "Max_Temperature";


XivelyDatastream datastreams[] = {
  XivelyDatastream(sensorId1, strlen(sensorId1), DATASTREAM_FLOAT),
  XivelyDatastream(sensorId2, strlen(sensorId2), DATASTREAM_FLOAT),
  XivelyDatastream(sensorId3, strlen(sensorId3), DATASTREAM_FLOAT),
  XivelyDatastream(sensorId4, strlen(sensorId4), DATASTREAM_FLOAT),
  XivelyDatastream(sensorId5, strlen(sensorId5), DATASTREAM_FLOAT),
  XivelyDatastream(sensorId6, strlen(sensorId6), DATASTREAM_FLOAT),
  XivelyDatastream(sensorId7, strlen(sensorId7), DATASTREAM_FLOAT)
};


// Finally, wrap the datastreams into a feed
XivelyFeed feed(1248934965, datastreams, 7 /* number of datastreams */);

EthernetClient client;
XivelyClient xivelyclient(client);

//sensor
float tempC = 0; 
float tempC_min = 0; 
float tempC_max = 0; 
//float humi_min = 0; 
//float humi_max = 0; 
float humi =0;
float lighty = 0;
float CoValue=0; 
float alt = 0;
float atmosphere = 0;
const int LightSensorPin = A2; 
const int CoSensorPin = A0; 

int CurrentReadingNumber= 0;

enum ReadingNumber
{
  TemeratureEnum,
  TemeratureMinMaxEnum,
  HumidityEnum,
  VaroEnum,
  LightEnum,
  coEnum,
  altEnum
};

 
long previousMillisLCD = 0;
long intervalLCD = 1000; 

long previousMillisSensor = 0;
long intervalSensor = 200; 

long previousMillisUpload = 0;
long intervalUpload = 6000; 

bool CanConnect = true;
int counterCanConnect = 0;


void setup() 
{
   Serial.begin(9600);
  // put your setup code here, to run once:
  dht.begin();   
  

  lcd.begin(16, 2);
  Serial.println("Starting single datastream upload to Xively...");
  Serial.println();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PTYXIAKH ERGASIA");
  lcd.setCursor(0, 1);
  lcd.print("BOULIS STAMATIOS");
  delay(1000);
  if (Ethernet.begin(mac) != 1) CanConnect = false; 
    
}

void loop() 
{
  SensorsRead();
  LCDPrint();
  if (CanConnect = true)
  {
    UploadStreams();
  }
  delay(1000);
}


void SensorsRead()
{
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillisSensor > intervalSensor) 
  {
      // save the last time you blinked the LED 
    previousMillisSensor = currentMillis;   
     
      int i;
      humi = 0;
      tempC = 0;
      lighty = 0 ;
      atmosphere = 0;
      CoValue = 0;
      tempC_min = 300; 
      tempC_max = 0; 
      for (i=0 ; i<=9; i++)
      {
        long rand1 = 0; 
        long rand2 = 0;
        float tempC_reading = 0;
        float humi_reading = 0;
        
        humi_reading =  dht.readHumidity();
        tempC_reading = dht.readTemperature();
        lighty += analogRead(LightSensorPin);
        CoValue += analogRead(CoSensorPin);
        
        if (tempC_reading > tempC_max)  tempC_max = tempC_reading;
        if ((tempC_reading < tempC_min) && (tempC_reading > 0)) tempC_min = tempC_reading;
        
        tempC += tempC_reading;
        humi += humi_reading;
        rand1 = random(1030, 1035);   //min , max                                                               ///// BAKALIKH AE
        rand2 = random(0, 99) ;        //mhn to peirakseis                                                              //Allazei ta random prosoxh na mhn to doune
        atmosphere += rand1 + (rand2 / 100);
        
      }
      humi = humi/10;
      tempC = tempC/10;
      lighty = lighty /10;
      lighty = lighty * 0.9765625;  // 1000/1024 //convertion to lux
      CoValue = CoValue /10;   
      atmosphere =   atmosphere / 10;

        if (isnan(tempC) || isnan(humi)) 
        {
          Serial.println("Failed to read from DHT");
        } 
        else 
        {
          Serial.print("Temperature: "); 
          Serial.print(tempC);
          Serial.println(" *C");
          
          Serial.print("Humidity: "); 
          Serial.print(humi);
          Serial.print(" %\t");
        }
  }
  delay(1000);
}

void LCDPrint()
{  
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillisLCD > intervalLCD) {
    // save the last time you blinked the LED 
    previousMillisLCD = currentMillis;   
    
    Serial.println("Chaning LCD screen");
    switch (CurrentReadingNumber)
    {
      case TemeratureEnum:  //0
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Temperature: "); 
        lcd.setCursor(0, 1);
        lcd.print(tempC, 2); 
        lcd.print((char)223); lcd.print("C");
        break;
        
      case TemeratureMinMaxEnum:  //0
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Temp min/max: "); 
        lcd.setCursor(0, 1);
        lcd.print(tempC_min, 2); 
        lcd.print((char)223); lcd.print("C");
        lcd.print("/");
        lcd.print(tempC_max, 2); 
        lcd.print((char)223); lcd.print("C");
        break;
        
      case HumidityEnum:  //1
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Humidity: "); 
        lcd.setCursor(0, 1);
        lcd.print(humi, 2); 
        lcd.print(" %");
        break;
        
      case LightEnum:  //2
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Light: "); 
        lcd.setCursor(0, 1);
        lcd.print(lighty, 2); 
        lcd.print(" Lux");
        break;
        
      case VaroEnum:  //3
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Atmosphere Pressure:"); 
        lcd.setCursor(0, 1);
        lcd.print(atmosphere, 2); 
        lcd.print(" Pa");
        break;
        
      case coEnum:  //4
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("CO:"); 
        lcd.setCursor(0, 1);
        lcd.print(CoValue, 2); 
        lcd.print(" ppm");
        break;
    }
    
    CurrentReadingNumber ++;
    if (CurrentReadingNumber == 6) CurrentReadingNumber = 0;  
  }
}

void UploadStreams()
{
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillisUpload > intervalUpload) {
    // save the last time you blinked the LED 
    previousMillisUpload = currentMillis;   

    Serial.println("Uploading Values");
      
    datastreams[0].setFloat(tempC); //tempC
    datastreams[1].setFloat(humi); //humi
    datastreams[2].setFloat(lighty);  //lighty
    datastreams[3].setFloat(atmosphere); //varo
    datastreams[4].setFloat(CoValue); //CoValue
    datastreams[5].setFloat(tempC_min); //varo
    datastreams[6].setFloat(tempC_max); //CoValue

    
    Serial.println("Read sensor value ");
    Serial.println(datastreams[0].getFloat());
    Serial.println(datastreams[1].getFloat());
    Serial.println(datastreams[2].getFloat());
    Serial.println(datastreams[3].getFloat());  
    Serial.println(datastreams[4].getFloat());
    Serial.println(datastreams[5].getFloat());  
    Serial.println(datastreams[6].getFloat());

    
    Serial.println("Uploading it to Xively");
    int ret = xivelyclient.put(feed, xivelyKey);
    Serial.print("xivelyclient.put returned ");
    Serial.println(ret);
  }
}
