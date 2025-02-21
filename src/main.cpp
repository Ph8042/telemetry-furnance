#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <PubSubClient.h>
// TODO: add time stamping
// #include <Time.h>

const char *ssid = "";
const char *password = "";
const char *mqtt_server = "";
const char *mqtt_user = "";
const char *mqtt_password = "";
const char *mqtt_topic = "";
const char *mqtt_publish_topic = "";
const int mqtt_port = 1883;
const char *lwt_topic = "";
const char *lwt_message = "";
const char *mqtt_presence_message = "";  // message when device returns back
const char *mqtt_unique_clientid = "";   // just name it

unsigned long previousMillis = 0;
const long interval = 30000;  // 30sec interval

WiFiClient espClient;
PubSubClient client(espClient);

#define ONE_WIRE_BUS 5                // pin GPIO4 is the default one (change if u want)
OneWire oneWire(ONE_WIRE_BUS);        // OneWire object
DallasTemperature sensors(&oneWire);  // DS18B20 object

/*
TODO: move this to main readme file
Pin DQ (Data) -> GPIO4 by default (set in #define ONE_WIRE_BUS).
VCC -> 3.3V.
GND -> GND.

place a pull-up resistor between VCC and DQ (4,7 kΩ or 10 kΩ)
*/

// pin definition for DS18B20

void setup_wifi() {
  Serial.println("");
  Serial.println("connecting with wifi...");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }

  Serial.println("succesfully connected with Wi-Fi");
}

void setup_mqtt() {
  client.setServer(mqtt_server, mqtt_port);
  client.connect(mqtt_unique_clientid, mqtt_user, mqtt_password, lwt_topic, 0, 0, lwt_message);  // new name!
  Serial.print("MQTT connection status: ");

  if (client.connected()) {
    Serial.println("ON");
  } else {
    Serial.println("OFF");
    Serial.print("Error, rc=");
    Serial.print(client.state());
    client.setServer(mqtt_server, mqtt_port);
    client.connect(mqtt_unique_clientid, mqtt_user, mqtt_password, lwt_topic, 0, 0, lwt_message);  // new name!
    delay(5000);
  }
}

void setup_sensor() {
  Serial.println("initialization DS18B20...");
  sensors.begin();
  if (sensors.getDeviceCount() == 0) {
    Serial.println("sensor not detected");
  } else {
    Serial.println("sensor DS18B20 found");
  }
}

// TODO: add time stamping
// void setup_time()
// String get_timestamp()

void setup() {
  Serial.begin(115200);  // launch serial port, update config file with 115200 monitor speed
  setup_sensor();
  setup_wifi();
  setup_mqtt();
  // setup_time();
  client.publish(mqtt_publish_topic, mqtt_presence_message);
}

void loop() {
  if (!client.connected()) {
    setup_mqtt();
  }
  client.loop();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    sensors.requestTemperatures();
    float temperature = sensors.getTempCByIndex(0);

    if (temperature != DEVICE_DISCONNECTED_C) {
      char tempString[8];
      dtostrf(temperature, 1, 2, tempString);
      client.publish(mqtt_topic, tempString);
    }
  }
}