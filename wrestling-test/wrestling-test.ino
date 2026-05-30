#include <VarSpeedServo.h>

// Создаем объекты сервоприводов
VarSpeedServo myservo4; // правая стопа
VarSpeedServo myservo5; // правое колено
VarSpeedServo myservo6; // правое бедро
VarSpeedServo myservo7; // правая рука

VarSpeedServo myservo8; // левая рука
VarSpeedServo myservo9; // левое бедро
VarSpeedServo myservo10; // левое колено
VarSpeedServo myservo11; // левая стопа

// Настройки
#define LEG_SPEED 30      // 0=full speed, 1-255 slower to faster
#define HAND_SPEED 80      // 0=full speed, 1-255 slower to faster
#define IS_WAIT false // false = асинхронно (не блокирует код), true = ждать окончания движения

// Пины сервоприводов согласно wrestling-robot.ino
#define SERVO_4 4
#define SERVO_5 5
#define SERVO_6 6
#define SERVO_7 7
#define SERVO_8 8
#define SERVO_9 9
#define SERVO_10 10
#define SERVO_11 11

#define S4_START 90
#define S5_START 90
#define S6_START 90
#define S7_START 90
#define S8_START 90
#define S9_START 90
#define S10_START 90
#define S11_START 90

// Диапазоны импульсов (мкс) согласно вашему предыдущему коду
#define FEETCH_MIN 544
#define FEETCH_MAX 2400
#define TOWER_MIN 500
#define TOWER_MAX 2500

void print(String msg) {
  Serial.println(msg);
}

void toStartAll() {
  // обе ноги на исходную
  moveLeg(myservo4, S4_START);
  moveLeg(myservo5, S5_START);
  moveLeg(myservo6, S6_START);
  moveLeg(myservo7, S7_START);
  moveLeg(myservo8, S8_START);
  moveLeg(myservo9, S9_START);
  moveLeg(myservo10, S10_START);
  moveLeg(myservo11, S11_START);
  delay(500);
}

void servoInit() {
  // Инициализация с индивидуальными диапазонами
  myservo1.attach(SERVO_1, TOWER_MIN, TOWER_MAX);
  myservo2.attach(SERVO_2, TOWER_MIN, TOWER_MAX);
  myservo3.attach(SERVO_3, FEETCH_MIN, FEETCH_MAX);
  myservo4.attach(SERVO_4, TOWER_MIN, TOWER_MAX);
  myservo5.attach(SERVO_5, FEETCH_MIN, FEETCH_MAX);
  myservo6.attach(SERVO_6, FEETCH_MIN, FEETCH_MAX);
}

void moveLeg(VarSpeedServo s, int angle) {
  s.write(angle, LEG_SPEED, IS_WAIT);
}

void moveHand(VarSpeedServo s, int angle) {
  s.write(angle, HAND_SPEED, IS_WAIT);
}

void setup() {
  print("Program start");
  Serial.begin(9600);
  servoInit();
  toStartAll();

  int leg_shift = 15;
  int hand_shift = 90;
  int stage_delay = 500;

  // Hello
  moveHand(myservo4, S4_START - hand_shift); // left hand up
  delay(1000);
  moveHand(myservo4, S4_START); // left hand to start
  delay(stage_delay);

  // Move hands
  hand_shift = 30;
  for (int i = 0; i < 2; i++) {
    moveHand(myservo3, S3_START + hand_shift); // right hand UP
    moveHand(myservo4, S4_START + hand_shift); // left hand DOWN
    delay(400);
    moveHand(myservo3, S3_START - hand_shift); // right hand DOWN
    moveHand(myservo4, S4_START - hand_shift); // left hand UP
    delay(400);
  }
  moveHand(myservo3, S3_START); // right hand to start
  moveHand(myservo4, S4_START); // left hand to start
  delay(stage_delay);

  // Move left leg
  hand_shift = 90;
  moveHand(myservo4, S4_START - hand_shift); // left hand up
  moveHand(myservo3, S3_START + hand_shift); // right hand up
  moveLeg(myservo5, S5_START - leg_shift);
  moveLeg(myservo6, S6_START + leg_shift);
  delay(1000);
  moveHand(myservo4, S4_START);
  moveHand(myservo3, S3_START);
  moveLeg(myservo5, S5_START);
  moveLeg(myservo6, S6_START);
  delay(stage_delay);

  // Move right leg
  moveHand(myservo4, S4_START - hand_shift); // left hand up
  moveHand(myservo3, S3_START + hand_shift); // right hand up
  moveLeg(myservo1, S1_START - leg_shift);
  moveLeg(myservo2, S2_START + leg_shift);
  delay(1000);
  moveHand(myservo4, S4_START);
  moveHand(myservo3, S3_START);
  moveLeg(myservo1, S1_START);
  moveLeg(myservo2, S2_START);
  delay(stage_delay);

  // Bye bye
  moveHand(myservo4, 0); // left hand up
  delay(1000);
  for (int i = 0; i < 2; i++) {
    moveHand(myservo4, 10); // left hand DOWN
    delay(100);
    moveHand(myservo4, 0); // left hand UP
    delay(100);
  }
  
  delay(1000);
  moveHand(myservo4, S4_START);
}

void loop() {
  
}
