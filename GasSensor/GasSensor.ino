int mqx_analogPin = A0; // connected to the output pin of MQ-X

void setup(){
  Serial.begin(9600); // open serial at 9600 bps
}

void loop()
{
  //975 alarm
  // give ample warmup time for readings to stabilize

  int mqx_value = analogRead(mqx_analogPin);
  Serial.println(mqx_value);

  delay(100); //Just here to slow down the output.
}

