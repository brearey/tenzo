#include <SoftwareSerial.h>
SoftwareSerial mySerial(3, 2); // RX, TX

// servo
#include <Servo.h>
Servo myservo;
#define SERVO_PIN 5
#define SERVO_VCC 12

void print(String msg) {
  mySerial.println(msg);
  Serial.println(msg);
}

void servoTest() {
  myservo.write(0); // degrees
  delay(200);
  myservo.write(180); // degrees
  delay(200);
}

void servoInit() {
  print("Servo on: " + myservo.read());
  pinMode(SERVO_VCC, OUTPUT);
  digitalWrite(SERVO_VCC, HIGH);
  myservo.attach(SERVO_PIN);
  servoTest();
}

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  servoInit();
  Serial.println("Program started");
}

void loop() {
  if (mySerial.available()) { // commands from Android
    int cmd = mySerial.read();
    Serial.write(cmd);

    switch (cmd) {
      case 'f':
          print("Forward");
          myservo.write(0); // degrees
          break;

      case 'b':
          print("Backward");
          myservo.write(180); // degrees
          break;
    }
  }
  if (Serial.available()) { // commands from PC
    mySerial.write(Serial.read());
  }
}
