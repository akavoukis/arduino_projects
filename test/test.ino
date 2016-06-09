volatile int state = LOW;
volatile int PrevState= LOW;

void setup()
{
    Serial.begin(9600);

  attachInterrupt(0, blink, CHANGE);
}

void loop()
{
 if (PrevState != state)
 {
     Serial.println(state);
 }
}

void blink()
{
  PrevState = state;
  state = !state;
}
