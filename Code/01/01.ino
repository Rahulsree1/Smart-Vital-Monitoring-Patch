#include <Wire.h>
#include "Protocentral_MAX30205.h"
#include "MAX30105.h"
#include "heartRate.h"
#include "./Functions.h"


MAX30205 temp;
MAX30105 PS;


const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0;

float beatsPerMinute;
int beatAvg;


void setup() {
  Serial.begin(115200);
  Wire.begin();
  Serial.print("Hello");
  while(!temp.scanAvailableSensors()){
    Serial.println("Couldn't find the temperature sensor, please connect the sensor." );
    delay(30000);
  }

  temp.begin();

  if(!PS.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  PS.setup(); //Configure sensor with default settings
  PS.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  PS.setPulseAmplitudeGreen(1); //Turn off Green LED

}

void loop() {
  float temperature = temp.getTemperature();
  // Serial.print(" ");
  // Serial.println(temperature);
  long irValue = PS.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  // Serial.print("  IR=");
  // Serial.print(irValue);
  Serial.print("BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);
  if (irValue < 50000){
    Serial.print(" No finger?");
  }
  Serial.print(", Temp = ");
  Serial.print(Fahrenheit(temperature));
  Serial.println();
  delay(50);

  

}
