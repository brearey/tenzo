#include <VarSpeedServo.h>

// Создаем объекты сервоприводов
VarSpeedServo myservo1;
VarSpeedServo myservo2;
VarSpeedServo myservo3;
VarSpeedServo myservo4;
VarSpeedServo myservo5;
VarSpeedServo myservo6;

// Настройки
#define LEG_SPEED 30      // 0=full speed, 1-255 slower to faster
#define HAND_SPEED 80      // 0=full speed, 1-255 slower to faster
#define IS_WAIT false // false = асинхронно (не блокирует код), true = ждать окончания движения

// Пины сервоприводов согласно wrestling-robot.ino
#define SERVO_1 4
#define SERVO_2 5
#define SERVO_3 6
#define SERVO_4 7
#define SERVO_5 8
#define SERVO_6 9

#define S1_START 86
#define S2_START 98
#define S3_START 93
#define S4_START 93
#define S5_START 80
#define S6_START 100

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
  moveLeg(myservo1, S1_START);
  moveLeg(myservo2, S2_START);
  moveLeg(myservo3, S3_START);
  moveLeg(myservo4, S4_START);
  moveLeg(myservo5, S5_START);
  moveLeg(myservo6, S6_START);
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
