/*macro definition of water sensor and the buzzer*/
#define WATER_SENSOR 8
#define LED 13
void setup()
{
	pins_init();
Serial.begin(9600);
}
void loop()
{
	if(isExposedToWater() == true)
        {
          Serial.println("true");
          
        }
        else
        {
          Serial.println("false");

        }
        
		
}
void pins_init()
{
	pinMode(WATER_SENSOR, INPUT);
	pinMode(LED, OUTPUT);
}
				
/************************************************************************/
/*Function: Determine whether the sensor is exposed to the water		*/
/*Parameter:-void           											*/
/*Return:	-boolean,if it is exposed to the water,it will return ture. */
boolean isExposedToWater()
{
	if(digitalRead(WATER_SENSOR) == LOW)
		return true;
	else return false;
}
