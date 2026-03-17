#include <SoftwareSerial.h>
SoftwareSerial mySerial(8, 7); // RX, TX

// servo
#include <Servo.h>
Servo myservo1;
Servo myservo2;
Servo myservo3;
Servo myservo4;
Servo myservo5;
Servo myservo6;

#define SERVO_1 3
#define SERVO_2 5
#define SERVO_3 6
#define SERVO_4 9
#define SERVO_5 10
#define SERVO_6 11

void print(String msg) {
  mySerial.println(msg);
  Serial.println(msg);
}

void servoInit() {
  myservo1.attach(SERVO_1);
  myservo2.attach(SERVO_2);
  myservo3.attach(SERVO_3);
  // myservo4.attach(SERVO_4);
  // myservo5.attach(SERVO_5);
  // myservo6.attach(SERVO_6);
  // servoTest();
}

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  servoInit();
  print("Program started");
}

void loop() {
  if (mySerial.available()) { // commands from Android
    int cmd = mySerial.read();
    Serial.write(cmd);

    switch (cmd) {
      case 'f':
          print("Forward");
          myservo1.write(0); // degrees
          myservo2.write(0); // degrees
          myservo3.write(0); // degrees
          // myservo4.write(0); // degrees
          // myservo5.write(0); // degrees
          // myservo6.write(0); // degrees
          print(String(myservo1.read()));
          print(String(myservo2.read()));
          print(String(myservo3.read()));
          // print(String(myservo4.read()));
          // print(String(myservo5.read()));
          // print(String(myservo6.read()));
          break;

      case 'b':
          print("Backward");
          myservo1.write(180); // degrees
          myservo2.write(180); // degrees
          myservo3.write(180); // degrees
          // myservo4.write(180); // degrees
          // myservo5.write(180); // degrees
          // myservo6.write(180); // degrees
          print(String(myservo1.read()));
          print(String(myservo2.read()));
          print(String(myservo3.read()));
          // print(String(myservo4.read()));
          // print(String(myservo5.read()));
          // print(String(myservo6.read()));
          break;
      case 's':
          print(String(myservo1.read()));
          print(String(myservo2.read()));
          print(String(myservo3.read()));
          // print(String(myservo4.read()));
          // print(String(myservo5.read()));
          // print(String(myservo6.read()));
          break;
    }
  }
  if (Serial.available()) { // commands from PC
    mySerial.write(Serial.read());
  }
}
