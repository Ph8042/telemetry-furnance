#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/*
Pin DQ (Data) -> GPIO4 by default (set in #define ONE_WIRE_BUS).
VCC -> 3.3V.
GND -> GND.

place a pull-up resistor between VCC and DQ (4,7 kΩ or 10 kΩ)
*/

// pin definition for DS18B20
#define ONE_WIRE_BUS 15 // pin GPIO4 is the default one (change if u want)

OneWire oneWire(ONE_WIRE_BUS);       // OneWire object
DallasTemperature sensors(&oneWire); // DS18B20 object

void setup()
{
  Serial.begin(115200); // launch serial port, update config file with 115200 monitor speed
  Serial.println("initialization DS18B20...");

  sensors.begin();
  if (sensors.getDeviceCount() == 0)
  {
    Serial.println("sensor not detected");
  }
  else
  {
    Serial.println("sensor DS18B20 found");
  }
}

void loop()
{
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);

  if (temperature == DEVICE_DISCONNECTED_C)
  {
    Serial.println("sensor is not connected");
  }
  else
  {
    Serial.print("temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
  }

  delay(1000);
}