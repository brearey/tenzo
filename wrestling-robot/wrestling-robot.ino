
#include <SoftwareSerial.h>
SoftwareSerial mySerial(3, 2); // RX, TX

// servo
#include <VarSpeedServo.h>
VarSpeedServo myservo1;
VarSpeedServo myservo2;
VarSpeedServo myservo3;
VarSpeedServo myservo4;
VarSpeedServo myservo5;
VarSpeedServo myservo6;

// speed 1 - 255, 0 = max speed
#define SPEED 60
#define IS_WAIT false
#define SERVO_1 4
#define SERVO_2 5
#define SERVO_3 6
#define SERVO_4 7
#define SERVO_5 8
#define SERVO_6 9

// min, max pulse duration microseconds of PDM (pulse duration modulation)
#define FEETCH_MIN 544
#define FEETCH_MAX 2400
#define TOWER_MIN 500
#define TOWER_MAX 2500

void print(String msg) {
  mySerial.println(msg);
  Serial.println(msg);
}

void servoInit() {
  myservo1.attach(SERVO_1, TOWER_MIN, TOWER_MAX);
  myservo2.attach(SERVO_2, TOWER_MIN, TOWER_MAX);
  myservo3.attach(SERVO_3, FEETCH_MIN, FEETCH_MAX);
  myservo4.attach(SERVO_4, FEETCH_MIN, FEETCH_MAX);
  myservo5.attach(SERVO_5, FEETCH_MIN, FEETCH_MAX);
  myservo6.attach(SERVO_6, FEETCH_MIN, FEETCH_MAX);
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
          myservo1.write(5, SPEED, IS_WAIT);
          myservo2.write(5, SPEED, IS_WAIT);
          myservo3.write(5, SPEED, IS_WAIT);
          myservo4.write(5, SPEED, IS_WAIT);
          myservo5.write(5, SPEED, IS_WAIT);
          myservo6.write(5, SPEED, IS_WAIT);
          break;

      case 'b':
          print("Backward");
          myservo1.write(175, SPEED, IS_WAIT);
          myservo2.write(175, SPEED, IS_WAIT);
          myservo3.write(175, SPEED, IS_WAIT);
          myservo4.write(175, SPEED, IS_WAIT);
          myservo5.write(175, SPEED, IS_WAIT);
          myservo6.write(175, SPEED, IS_WAIT);
          break;
      case 'd': //default degrees
          myservo1.write(93, SPEED, IS_WAIT);
          myservo2.write(93, SPEED, IS_WAIT);
          myservo3.write(93, SPEED, IS_WAIT);
          myservo4.write(93, SPEED, IS_WAIT);
          myservo5.write(93, SPEED, IS_WAIT);
          myservo6.write(93, SPEED, IS_WAIT);
          break;
      case 's':
          print(String(myservo1.read()));
          print(String(myservo2.read()));
          print(String(myservo3.read()));
          print(String(myservo4.read()));
          print(String(myservo5.read()));
          print(String(myservo6.read()));
          break;
    }
  }
  if (Serial.available()) { // commands from PC
    mySerial.write(Serial.read());
  }
}
