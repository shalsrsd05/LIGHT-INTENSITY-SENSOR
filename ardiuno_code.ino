#include <Wire.h>
#include <BH1750.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";

// MQTT broker configuration
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_topic = "esp8266/gy30";

// Objects
WiFiClient espClient;
PubSubClient client(espClient);
BH1750 lightMeter;

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266_GY30")) {
      client.subscribe(mqtt_topic);
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Wire.begin(D2, D1); // SDA, SCL
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  lightMeter.begin();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float lux = lightMeter.readLightLevel();

  StaticJsonDocument<100> doc;
  doc["lux"] = lux;

  char buffer[100];
  serializeJson(doc, buffer);

  client.publish(mqtt_topic, buffer);
  Serial.print("Light Intensity: ");
  Serial.print(lux);
  Serial.println(" lux");

  delay(3000);
}
