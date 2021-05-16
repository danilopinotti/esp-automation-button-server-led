#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <Servo.h> // https://github.com/RoboticsBrno/ServoESP32
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>

#define BROKER_ADDRESS "broker.hivemq.com"
#define BROKER_USERNAME ""
#define BROKER_PASSWORD ""
#define BROKER_CLIENT_ID "btkServo1Client"

#define SERVO_PIN 13
#define SERVO_TOPIC "btk/atuadores/idServo1/press"

Servo myServo;

WiFiClient wifiClient;
PubSubClient mqttClient(BROKER_ADDRESS, 1883, wifiClient);

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFiManager wifiManager;

  //  wifiManager.resetSettings();

  bool isConnected = wifiManager.autoConnect("SERVO", "password");

  if (!isConnected) {
    Serial.println("Failed to connect.");
    ESP.restart();
  }
}

void pressServoButton(int pressTime = 500, int interationDelay = 15)
{
  Serial.println("Activating Servo");

  for (int posDegrees = 0; posDegrees <= 90; posDegrees++) {
    myServo.write(posDegrees);
    delay(interationDelay);
  }

  delay(pressTime);

  for (int posDegrees = 90; posDegrees >= 0; posDegrees--) {
    myServo.write(posDegrees);
    delay(interationDelay);
  }
}

void handleReceivedMessage(char* topic, byte* payload, unsigned int length) {
  String message = String((char*)payload).substring(0, length);

  if (String(topic) == String(SERVO_TOPIC)) {
    pressServoButton(300, 10);
  }
}

void setup()
{
  Serial.begin(9600);

  setupWifi();

  myServo.attach(SERVO_PIN);

  if (! mqttClient.connect(BROKER_CLIENT_ID, BROKER_USERNAME, BROKER_PASSWORD)) {
    ESP.restart();
  }

  mqttClient.setCallback(handleReceivedMessage);
  mqttClient.subscribe(SERVO_TOPIC);
}

void loop()
{
  mqttClient.loop();
}
