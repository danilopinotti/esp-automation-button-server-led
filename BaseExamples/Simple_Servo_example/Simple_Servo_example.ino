#include <Servo.h> // https://github.com/RoboticsBrno/ServoESP32

#define SERVO_PIN 13

Servo myServo;

void pressButton(int pressTime = 100, int interationDelay = 15)
{
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

void setup() {
  Serial.begin(9600);
  myServo.attach(SERVO_PIN);
}

void loop() {
  pressButton(500, 10);
}
