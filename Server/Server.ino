#include <ESP8266WiFi.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>

#define BROKER_ADDRESS "broker.hivemq.com"
#define BROKER_USERNAME ""
#define BROKER_PASSWORD ""
#define BROKER_CLIENT_ID "btkSrv"

WiFiClient wifiClient;
PubSubClient mqttClient(BROKER_ADDRESS, 1883, wifiClient);

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFiManager wifiManager;

  //wifiManager.resetSettings();

  bool isConnected;
  isConnected = wifiManager.autoConnect("SERVER", "password");

  if (!isConnected) {
    Serial.println("Failed to connect.");
    ESP.restart();
  }
}

void setup()
{
  Serial.begin(9600);

  setupWifi();

  mqttClient.setCallback(handleReceivedMessage);
  if (! mqttClient.connect(BROKER_CLIENT_ID, BROKER_USERNAME, BROKER_PASSWORD)) {
    ESP.restart();
  }

  mqttClient.subscribe("btk/#");
}

void handleReceivedMessage(char* topic, byte* payload, unsigned int length) {
  String channel = String(topic);
  String message = String((char*)payload).substring(0, length);

  if (channel == String("btk/interruptores/idBtn1/status")) {
    mqttClient.publish("btk/iluminacao/idLight1/turn", message.c_str());
  }
}

void loop()
{
  mqttClient.loop();
}
