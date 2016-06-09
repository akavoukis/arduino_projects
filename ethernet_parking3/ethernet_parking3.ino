#include <SPI.h>
#include <Ethernet.h>
#include <NewPing.h>

#define Red  1
#define Green  0

int XS1LedGreen = 46;
int XS1LedRed = 44;

int XS2LedGreen = 42;
int XS2LedRed = 40;

int XS3LedGreen = 38;
int XS3LedRed = 36;

int XS4LedGreen = 34;
int XS4LedRed = 32;

#define TRIG_DISTANCE 7

#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.


#define XS1TRIGGER_PIN  30  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define XS1ECHO_PIN     28  // Arduino pin tied to echo pin on the ultrasonic sensor.

#define XS2TRIGGER_PIN  26  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define XS2ECHO_PIN     24  // Arduino pin tied to echo pin on the ultrasonic sensor.

#define XS3TRIGGER_PIN  31  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define XS3ECHO_PIN     33  // Arduino pin tied to echo pin on the ultrasonic sensor.

#define XS4TRIGGER_PIN  35  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define XS4ECHO_PIN     37  // Arduino pin tied to echo pin on the ultrasonic sensor.

NewPing XS1sonar(XS1TRIGGER_PIN, XS1ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing XS2sonar(XS2TRIGGER_PIN, XS2ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing XS3sonar(XS3TRIGGER_PIN, XS3ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing XS4sonar(XS4TRIGGER_PIN, XS4ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

int XS1Distance  = 0;
int XS2Distance  = 0;
int XS3Distance  = 0;
int XS4Distance  = 0;


boolean XS1Reserved = false;
boolean XS2Reserved = false;
boolean XS3Reserved = false;
boolean XS4Reserved = false;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1,100);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

int x = 0;


String readString = String(100); //string for fetching data from address

void setup()
{
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.

  pinMode(XS1LedGreen, OUTPUT);  
  pinMode(XS1LedRed, OUTPUT);     

  pinMode(XS2LedGreen, OUTPUT);
  pinMode(XS2LedRed, OUTPUT);

  pinMode(XS3LedGreen, OUTPUT);
  pinMode(XS3LedRed, OUTPUT);

  pinMode(XS4LedGreen, OUTPUT);
  pinMode(XS4LedRed, OUTPUT);

  Serial.println(F("Initiaizing ethernet..."));

  // this uses a fixed address
  Ethernet.begin(mac, ip);

  // get an address with DHCP
  //if (Ethernet.begin(mac) == 0)
  //  Serial.println("Failed to configure Ethernet using DHCP");

  // give the card a second to initialize 
  delay(1000);

  server.begin();

  //Serial.print(F("Garage Door Opener Control Ready at IP address "));
  Serial.println(Ethernet.localIP());
}

void loop()
{

  XS1Distance = GetDistance(XS1sonar);
  delay(100);
  XS2Distance = GetDistance(XS2sonar);
    delay(100);
  XS3Distance = GetDistance(XS3sonar);
    delay(100);
  XS4Distance = GetDistance(XS4sonar);
    delay(100);

  if (( XS1Reserved == true) && (XS1Distance < TRIG_DISTANCE))
  {
    XS1Reserved = false;
  }
  if (( XS2Reserved == true) && (XS2Distance < TRIG_DISTANCE))
  {
    XS2Reserved = false;
  }
  if (( XS3Reserved == true) && (XS3Distance < TRIG_DISTANCE))
  {
    XS3Reserved = false;
  }
  if (( XS4Reserved == true) && (XS4Distance < TRIG_DISTANCE))
  {
    XS4Reserved = false;
  }


  if ( XS1Reserved == false)
  {
    if (XS1Distance > TRIG_DISTANCE) SetLed(1,Green);
    else SetLed(1,Red);
  }
  else
  {
    SetLed(1,Red);
  }

  if ( XS2Reserved == false)
  {
    if (XS2Distance > TRIG_DISTANCE) SetLed(2,Green);
    else SetLed(2,Red);
  }
  else
  {
    SetLed(2,Red);
  }

  if ( XS3Reserved == false)
  {
    if (XS3Distance > TRIG_DISTANCE) SetLed(3,Green);
    else SetLed(3,Red);
  }
  else
  {
    SetLed(3,Red);  
  }

  if ( XS4Reserved == false)
  {
    if (XS4Distance > TRIG_DISTANCE) SetLed(4,Green);
    else SetLed(4,Red);
  }
  else
  {
    SetLed(4,Red);  
  }


  Serial.print("Sensor 1: ");
  Serial.print(XS1Distance);
   Serial.print("\t");
  Serial.print("Sensor 2: ");
  Serial.print(XS2Distance);
     Serial.print("\t");
  Serial.print("Sensor 3: ");
  Serial.print(XS3Distance);
     Serial.print("\t");
  Serial.print("Sensor 4: ");
  Serial.println(XS4Distance);

  // command received (one character)  '1' - activate garage door button
  char cmd = 0;          // 1 - pulse button
  boolean done = false;  // set to indicate that response is complete

    // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {

    //Serial.println(F("new client"));
    readString = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //Serial.write(c);
        //int i = c;
        //Serial.print("(");
        //Serial.print(i);
        //Serial.print(")");

        // store character received in receive string
        if (readString.length() < 100) {
          readString += (c);
        }

        // check for end of line
        if (c == '\n') {
          //Serial.print("Receved line: ");
          //Serial.print(readString);  

          // process line if its the "GET" request
          // a request looks like "GET /?1" or "GET /?2"
          if (readString.indexOf("GET") != -1) {
            if (readString.indexOf("?1") != -1)
              cmd = '1';
            // check for other commands here. ie turn on light, etc.
            if (readString.indexOf("?2") != -1)
              cmd = '2';  
            if (readString.indexOf("?3") != -1)
              cmd = '3'; 
            if (readString.indexOf("?4") != -1)
              cmd = '4'; 
            if (readString.indexOf("?5") != -1)
              cmd = '5'; 
            if (readString.indexOf("?6") != -1)
              cmd = '6'; 
            if (readString.indexOf("?7") != -1)
              cmd = '7';
            if (readString.indexOf("?8") != -1)
              cmd = '8'; 

          }              

          // if a blank line was received (just cr lf, length of 2), then its the end of the request
          if (readString.length() == 2) {
            if (cmd == '1')
            {
              XS1Reserved = true;
              //Serial.println(F("Activate Button"));
              //digitalWrite(6, HIGH);
            }
            if (cmd == '2')
            {
              XS1Reserved = false;
              //Serial.println(F("Activate Button"));
              //digitalWrite(6, LOW);
            }
            if (cmd == '3')
            {
              XS2Reserved = true;
              //Serial.println(F("Activate Button"));
              //digitalWrite(5, HIGH);
            }
            if (cmd == '4')
            {
              XS2Reserved = false;
              //Serial.println(F("Activate Button"));
              //digitalWrite(5, LOW);
            }
            if (cmd == '5')
            {
              XS3Reserved = true;
              //Serial.println(F("Activate Button"));
              //digitalWrite(7, HIGH);
            }
            if (cmd == '6')
            {
              XS3Reserved = false;
              //Serial.println(F("Activate Button"));
              //digitalWrite(7, LOW);
            }
            if (cmd == '7')
            {
              XS4Reserved = true;
              //Serial.println(F("Activate Button"));
              //digitalWrite(8, HIGH);
            }
            if (cmd == '8')
            {
              XS4Reserved = false;              
              //Serial.println(F("Activate Button"));
              //digitalWrite(8, LOW);
            }

            // add other commands here

              // send web page back to client 
            //Serial.println(F("sending web page"));
            SendWebPage(client); 
            //Serial.println(F("web page sent"));

            cmd = 0;

            // break out and disconnect. This will tell the browser the request is complete without   having to specify content-length
            break;

          }  // end of request reached

          // start line over            
          readString = "";
        }  // end of line reached
      }  // end data is available from client
    }  // end cient is connected
    // give the web browser time to receive the data
    //Serial.println(F("delay before disconnect"));
    delay(100);
    // close the connection:
    client.stop();
    //Serial.println(F("client disonnected")); 
  }  // end client has been created
}

// send web page
void SendWebPage(EthernetClient client)
{
  client.println(F("HTTP/1.1 200 OK")); 
  client.println(F("Content-Type: text/html")); 
  // to specify the length, wooul have to construct the entire string and then get its length
  //client.println("Content-Length: 1234"); 
  client.println(F("Connnection: close")); 
  client.println(); 



  client.println(F("<!DOCTYPE HTML>"));
  client.println(F("<html>"));
  client.println(F("<head>"));
  client.print("<meta http-equiv=\"refresh\" content=\"2;url=/\">");

  client.println(F("<title>Home Automation Control</title>"));

  client.println(F("<style type='text/css'>"));
  client.println(F(".label {font-size: 30px; text-align:center;}"));
  client.println(F("button {width: 160px; height: 70px; font-size: 30px; -webkit-appearance: none; background-color:#dfe3ee; }"));
  client.println(F("</style>"));

  client.println(F("<script type='text/javascript'>"));

  client.println(F("function OnButtonClicked(parm) { window.location.href=\"X?\" + parm; }"));

  client.println(F("</script>"));

  client.println(F("</head>"));


  client.println(F("<body style=\"background-color:#3b5998\">"));

  client.println(F("<div class=\"label\">"));

  client.println(F("<font color ='#FF0000' >Tei kavalas Parking Control </font> <br/><br/>"));

  // future idea: could read a limit switch on the garage door here and tell the user if the door is currently open or closed
  /*
  if (digitalRead(DOOR_OPEN_INPUT) == HIGH)
   client.println("Door is Open"); 
   else  
   client.println("Door is Closed"); 
   client.println("<br>");
   */

  client.println("<table  border=\"1\" style=\"width:100%\">");

  client.println("<tr>");
  client.println(" <td> <font size=\"10\"> Parking Names </font> </td>");   
  client.println(" <td> <font size=\"10\"> Parking Control Buttons </font>  </td>");   
  client.println(" <td> <font size=\"10\"> Status </font> </td>");   
  client.println(" </tr>");




  //HTML FOR XS1
  client.println("<tr>");
  client.println(" <td> Parking slot 1 </td>");     

  if ((XS1Reserved == false) && (XS1Distance > TRIG_DISTANCE))
  {
    client.println(F("<td> <button onclick=\"OnButtonClicked('1');\">Reserve Slot 1</button><br/><br/> </td> "));
  }
  else if ((XS1Reserved == false) && (XS1Distance <= TRIG_DISTANCE))
  {
    client.println(F("<td> <button disabled onclick=\"OnButtonClicked('1');\">Reserve Slot 1</button><br/><br/> </td> "));
  }
  else if (XS1Reserved == true)
  {
    client.println(F("<td> <button onclick=\"OnButtonClicked('2');\">Release</button><br/><br/> </td>"));
  }   

  client.println("<td>");
  client.println("<canvas id=\"myCanvas1\" width=\"200\" height=\"100\"  ");
  client.println("style=\"border:0px solid #000000;\">              ");
  client.println("</canvas>                                       ");

  client.println("<script>");
  client.println("var c = document.getElementById(\"myCanvas1\");");
  client.println("var ctx = c.getContext(\"2d\");");
  client.println("ctx.beginPath();");
  client.println("ctx.arc(95,50,40,0,2*Math.PI);");

  if (XS1Reserved == true) 
  {
    client.println("ctx.fillStyle = 'red'; ");
  }
  else if (XS1Distance > TRIG_DISTANCE)
  {
    client.println("ctx.fillStyle = 'green'; ");
  }
  else if (XS1Distance <= TRIG_DISTANCE)
  {
    client.println("ctx.fillStyle = 'red'; ");
  }
  client.println("ctx.fill();              ");
  client.println("ctx.stroke();");
  client.println("</script> ");


  client.println("</td>");

  client.println("  </tr>");

  //HTML FOR XS2
  client.println("<tr>");
  client.println(" <td> Parking slot 2 </td> ");


  if ((XS2Reserved == false) && (XS2Distance > TRIG_DISTANCE))
  {
    client.println(F(" <td> <button onclick=\"OnButtonClicked('3');\">Reserve Slot 2</button><br/><br/> </td>"));
  }
  else if ((XS2Reserved == false) && (XS2Distance <= TRIG_DISTANCE))
  {
    client.println(F(" <td> <button disabled onclick=\"OnButtonClicked('3');\">Reserve Slot 2</button><br/><br/> </td>"));
  }  
  else if (XS2Reserved == true)
  {
    client.println(F(" <td> <button onclick=\"OnButtonClicked('4');\">Release</button><br/><br/> </td>"));
  }    

  client.println("<td>");
  client.println("<canvas id=\"myCanvas2\" width=\"200\" height=\"100\"  ");
  client.println("style=\"border:0px solid #000000;\">              ");
  client.println("</canvas>                                       ");

  client.println("<script>");
  client.println("var c = document.getElementById(\"myCanvas2\");");
  client.println("var ctx = c.getContext(\"2d\");");
  client.println("ctx.beginPath();");
  client.println("ctx.arc(95,50,40,0,2*Math.PI);");

  if (XS2Reserved == true)
  {
    client.println("ctx.fillStyle = 'red'; ");
  }
  else if (XS2Distance > TRIG_DISTANCE)
  {
    client.println("ctx.fillStyle = 'green'; ");
  }
  else if (XS2Distance <= TRIG_DISTANCE)
  {
    client.println("ctx.fillStyle = 'red'; ");
  }
  client.println("ctx.fill();              ");
  client.println("ctx.stroke();");
  client.println("</script> ");

  client.println("  </tr>");

  //HTML FOR XS3
  client.println("<tr>");
  client.println(" <td> Parking slot 3 </td>");


  if ((XS3Reserved == false) && (XS3Distance > TRIG_DISTANCE))
  {
    client.println(F(" <td> <button onclick=\"OnButtonClicked('5');\">Reserve Slot 3</button><br/><br/> </td>"));
  }
  else if ((XS3Reserved == false) && (XS3Distance <= TRIG_DISTANCE))
  {
    client.println(F(" <td> <button disabled onclick=\"OnButtonClicked('5');\">Reserve Slot 3</button><br/><br/> </td>"));
  }  
  else if (XS3Reserved == true)
  {
    client.println(F("<td> <button onclick=\"OnButtonClicked('6');\">Release</button><br/><br/> </td>"));
  } 

  client.println("<td>");
  client.println("<canvas id=\"myCanvas3\" width=\"200\" height=\"100\"  ");
  client.println("style=\"border:0px solid #000000;\">              ");
  client.println("</canvas>                                       ");

  client.println("<script>");
  client.println("var c = document.getElementById(\"myCanvas3\");");
  client.println("var ctx = c.getContext(\"2d\");");
  client.println("ctx.beginPath();");
  client.println("ctx.arc(95,50,40,0,2*Math.PI);");

  if (XS3Reserved == true)
  {
    client.println("ctx.fillStyle = 'red'; ");
  }
  else if (XS3Distance > TRIG_DISTANCE)
  {
    client.println("ctx.fillStyle = 'green'; ");
  }
  else if (XS3Distance <= TRIG_DISTANCE)
  {
    client.println("ctx.fillStyle = 'red'; ");
  }
  client.println("ctx.fill();              ");
  client.println("ctx.stroke();");
  client.println("</script> ");


  client.println("  </tr>");

  //HTML FOR XS4
  client.println("<tr>");
  client.println("<td> Parking slot 4 </td>");


  if ((XS4Reserved == false) && (XS4Distance > TRIG_DISTANCE))
  { 
    client.println(F("<td> <button onclick=\"OnButtonClicked('7');\">Reserve Slot 4</button><br/><br/> </td> "));
  }
  if ((XS4Reserved == false) && (XS4Distance <= TRIG_DISTANCE))
  { 
    client.println(F("<td> <button disabled onclick=\"OnButtonClicked('7');\">Reserve Slot 4</button><br/><br/> </td> "));
  }  
  if (XS4Reserved == true)
  {
    client.println(F(" <td> <button onclick=\"OnButtonClicked('8');\">Release</button><br/><br/> </td>"));
  }

  client.println("<td>");
  client.println("<canvas id=\"myCanvas4\" width=\"200\" height=\"100\"  ");
  client.println("style=\"border:0px solid #000000;\">              ");
  client.println("</canvas>                                       ");

  client.println("<script>");
  client.println("var c = document.getElementById(\"myCanvas4\");");
  client.println("var ctx = c.getContext(\"2d\");");
  client.println("ctx.beginPath();");
  client.println("ctx.arc(95,50,40,0,2*Math.PI);");

  if (XS4Reserved == true)
  {
    client.println("ctx.fillStyle = 'red'; ");
  }
  else if (XS4Distance > TRIG_DISTANCE)
  {
    client.println("ctx.fillStyle = 'green'; ");
  }
  else if (XS4Distance <= TRIG_DISTANCE)
  {
    client.println("ctx.fillStyle = 'red'; ");
  }
  client.println("ctx.fill();              ");
  client.println("ctx.stroke();");
  client.println("</script> ");


  client.println("  </tr>");
  client.println("</table>");


  client.println(F("</div>"));

  client.println(F("</body>"));
  client.println(F("</html>"));

  client.println("");
}


int GetDistance(NewPing SensorToRead)
{
  unsigned int uS = SensorToRead.ping(); // Send ping, get ping time in microseconds (uS).
  return uS / US_ROUNDTRIP_CM;
  //Serial.print("Ping: ");
  //Serial.print(uS / US_ROUNDTRIP_CM); // Convert ping time to distance in cm and print result (0 = outside set distance range)
  //Serial.println("cm");
}

void SetLed(int XS, int Color)
{
  switch (XS) 
  {
  case 1:    // your hand is on the sensor
    if (Color == Green)
    {
      digitalWrite(XS1LedGreen, HIGH);
      digitalWrite(XS1LedRed, LOW);
    }
    else if (Color == Red)
    {
      digitalWrite(XS1LedGreen, LOW);
      digitalWrite(XS1LedRed, HIGH);
    }
    break;
  case 2:    // your hand is on the sensor
    if (Color == Green)
    {
      digitalWrite(XS2LedGreen, HIGH);
      digitalWrite(XS2LedRed, LOW);
    }
    else if (Color == Red)
    {
      digitalWrite(XS2LedGreen, LOW);
      digitalWrite(XS2LedRed, HIGH);
    }
    break;
  case 3:    // your hand is on the sensor
    if (Color == Green)
    {
      digitalWrite(XS3LedGreen, HIGH);
      digitalWrite(XS3LedRed, LOW);
    }
    else if (Color == Red)
    {
      digitalWrite(XS3LedGreen, LOW);
      digitalWrite(XS3LedRed, HIGH);
    }
    break;
  case 4:    // your hand is on the sensor
    if (Color == Green)
    {
      digitalWrite(XS4LedGreen, HIGH);
      digitalWrite(XS4LedRed, LOW);
    }
    else if (Color == Red)
    {
      digitalWrite(XS4LedGreen, LOW);
      digitalWrite(XS4LedRed, HIGH);
    }
    break;
  }
}





