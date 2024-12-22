#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char *ssid = "";
const char *password = "";
const char *mqtt_server = "";
const char *mqtt_user = "";
const char *mqtt_password = "";


WiFiClient espClient;
PubSubClient client(espClient);

#define ONE_WIRE_BUS 15              // pin GPIO4 is the default one (change if u want)
OneWire oneWire(ONE_WIRE_BUS);       // OneWire object
DallasTemperature sensors(&oneWire); // DS18B20 object

/*
TODO: move this to main readme file
Pin DQ (Data) -> GPIO4 by default (set in #define ONE_WIRE_BUS).
VCC -> 3.3V.
GND -> GND.

place a pull-up resistor between VCC and DQ (4,7 kΩ or 10 kΩ)
*/

// pin definition for DS18B20

void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.println("connecting with wifi... ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println(".");
  }

  Serial.println("");
  Serial.println("succesfully connected with wifi");
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("connecting with MQTT...");

    if (client.connect("ESP32Client", mqtt_user, mqtt_password))
    {
      Serial.println("succesfully connected with mqtt...");
      client.subscribe("esp32/test");
    }
    else
    {
      Serial.print("Error, rc=");
      Serial.print(client.state());
      Serial.println(" retry in 5 second...");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200); // launch serial port, update config file with 115200 monitor speed

  Serial.println("initialization networking...");
  setup_wifi();
  client.setServer(mqtt_server, 1883);

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
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

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

    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);
    client.publish("esp32/temperature", tempString);
  }

  delay(5000);
}