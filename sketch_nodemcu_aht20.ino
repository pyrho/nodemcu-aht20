#include <Wire.h>
#include "AHT20.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "secrets.h"

// Constants
#define ONE_MINUTE 60 * 1000
#define TWO_SECONDS 1000 * 2
#define ONE_HOUR ONE_MINUTE * 60
#define ONE_MINUTE_IN_MICROSECONDS ONE_MINUTE * 1000
#define FIVE_MINUTES_IN_MICROSECONDS ONE_MINUTE_IN_MICROSECONDS * 5

// Replace with your MQTT broker details
#define MQTT_SERVER_ADDR "mosquitto.lan"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "ESP8266-FILAMENTBOX1"

WiFiClient espClient;
PubSubClient client(espClient);

// Temp sensor
AHT20 aht20;

void setup_wifi() {

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(WiFi.status());
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Wi-Fi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup_sensor() {
  Wire.begin();  // Start the I2C Controller bus on the NodeMCU

  if (!aht20.begin()) {  // Check that the sensor is online
    Serial.println("Could not find AHT20 sensor, resetting");
    while (1)
      ;  // This will cause a crash and a reset (apparently)
  }
  Serial.println("AHT20 sensor online");
}

void setup_mqtt() {
  // Connect to MQTT
  client.setServer(MQTT_SERVER_ADDR, MQTT_PORT);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect(MQTT_CLIENT_ID)) {
      Serial.println("Connected to MQTT!");
    } else {
      Serial.print("Failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  delay(5);
  Serial.begin(74880);
  Serial.print("\n\n");  // Flush the output
  Serial.println("Hi!");
  setup_sensor();
  setup_wifi();
  setup_mqtt();
}

void reconnect_mqtt() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() {


  float humidity = aht20.getHumidity();
  float temperature = aht20.getTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from AHT20 sensor!");
    return;
  }


  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");


  // Check WiFi connection and reconnect if needed
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi connection lost");
  } else {
    if (client.connected()) {
      client.publish("home/sensors/filament-box-1/temperature", String(temperature).c_str());
      client.publish("home/sensors/filament-box-1/humidity", String(humidity).c_str());
    } else {
      Serial.println("Lost connection to MQTT, reconnecting...");
      reconnect_mqtt();
    }
  }

  // delay(TWO_SECONDS);
  Serial.println("Going to sleep now, see you later!");
  ESP.deepSleep(FIVE_MINUTES_IN_MICROSECONDS);
}
