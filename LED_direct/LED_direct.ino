#include <ESP8266WiFi.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>

#define BROKER_ADDRESS "broker.hivemq.com"
#define BROKER_USERNAME ""
#define BROKER_PASSWORD ""
#define BROKER_CLIENT_ID "btkLight1Client"

#define LED_PIN LED_BUILTIN

#define TURN_LIGHT_TOPIC "btk/interruptores/idBtn1/status"

WiFiClient wifiClient;
PubSubClient mqttClient(BROKER_ADDRESS, 1883, wifiClient);

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFiManager wifiManager;

  //wifiManager.resetSettings();

  bool isConnected;
  isConnected = wifiManager.autoConnect("LED_DIRECT", "password");

  if (!isConnected) {
    Serial.println("Failed to connect.");
    ESP.restart();
  }
}

void setup()
{
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  setupWifi();

  mqttClient.setCallback(handleReceivedMessage);
  if (! mqttClient.connect(BROKER_CLIENT_ID, BROKER_USERNAME, BROKER_PASSWORD)) {
    ESP.restart();
  }

  mqttClient.subscribe(TURN_LIGHT_TOPIC);
}

void handleReceivedMessage(char* topic, byte* payload, unsigned int length) {
  String message = String((char*)payload).substring(0, length);

  if (String(topic) == String(TURN_LIGHT_TOPIC)) {
    if (message == String("on")) {
      digitalWrite(LED_PIN, HIGH);
    } else {
      digitalWrite(LED_PIN, LOW);
    }
  }
}

void loop()
{
  mqttClient.loop();
}
