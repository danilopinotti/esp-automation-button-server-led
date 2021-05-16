#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>

#define BROKER_ADDRESS "broker.hivemq.com"
#define BROKER_USERNAME ""
#define BROKER_PASSWORD ""
#define BROKER_CLIENT_ID "btkBtn1Client"

#define BUTTON_PIN 5 // D1 no esp8266
#define STATUS_LED_PIN LED_BUILTIN

#define BUTTON_STATUS_TOPIC "btk/interruptores/idBtn1/status"

boolean isButtonOn = false;

WiFiClient wifiClient;
PubSubClient mqttClient(BROKER_ADDRESS, 1883, wifiClient);

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFiManager wifiManager;

  //wifiManager.resetSettings();

  bool isConnected;
  isConnected = wifiManager.autoConnect("BUTTON_SWITCH", "password");

  if (!isConnected) {
    Serial.println("Failed to connect.");
    ESP.restart();
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);

  setupWifi();

  mqttClient.setCallback(handleReceivedMessage);
  if (! mqttClient.connect(BROKER_CLIENT_ID, BROKER_USERNAME, BROKER_PASSWORD)) {
    ESP.restart();
  }

  propagateButtonStatus();
}

void handleReceivedMessage(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

void handleButtonPress() {
  if (digitalRead(BUTTON_PIN) == true) {
    delay(100);
    if (digitalRead(BUTTON_PIN) == true) {
      Serial.println("Button Pressed");
      
      isButtonOn = !isButtonOn;
      while (digitalRead(BUTTON_PIN) == true) {
        delay(50);
      }
      propagateButtonStatus();
    }
  }
}

void propagateButtonStatus() {
  if (isButtonOn == true) {
    mqttClient.publish(BUTTON_STATUS_TOPIC, "on");
  } else {
    mqttClient.publish(BUTTON_STATUS_TOPIC, "off");
  }
}

void loop()
{
  handleButtonPress();

  digitalWrite(STATUS_LED_PIN, isButtonOn);

  mqttClient.loop();
}
