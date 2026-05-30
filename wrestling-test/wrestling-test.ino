#include <VarSpeedServo.h>

// Создаем объекты сервоприводов
VarSpeedServo myservo4;   // правая стопа
VarSpeedServo myservo5;   // правое колено
VarSpeedServo myservo6;   // правое бедро

VarSpeedServo myservo7;   // правая рука
VarSpeedServo myservo8;   // левая рука

VarSpeedServo myservo9;   // левое бедро
VarSpeedServo myservo10;  // левое колено
VarSpeedServo myservo11;  // левая стопа

// Настройки
#define LEG_SPEED 30      // 0=full speed, 1-255 slower to faster
#define HAND_SPEED 80      // 0=full speed, 1-255 slower to faster
#define IS_WAIT false // false = асинхронно (не блокирует код), true = ждать окончания движения

// Пины
#define SERVO_4 4
#define SERVO_5 5
#define SERVO_6 6
#define SERVO_7 7
#define SERVO_8 8
#define SERVO_9 9
#define SERVO_10 10
#define SERVO_11 11

// Стартовые положения
#define S4_START 85
#define S5_START 90
#define S6_START 95
#define S7_START 90
#define S8_START 90
#define S9_START 95
#define S10_START 82
#define S11_START 97

// Диапазоны импульсов
#define FEETCH_MIN 544
#define FEETCH_MAX 2400

#define TOWER_MIN 500
#define TOWER_MAX 2500

#define HC_TRIG 3
#define HC_ECHO 2

float getDist(uint8_t trig, uint8_t echo) {
  // импульс 10 мкс
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  // измеряем время ответного импульса
  uint32_t us = pulseIn(echo, HIGH);

  // считаем расстояние и возвращаем
  return us / 58.2;
}

void print(String msg) {
  Serial.println(msg);
}

void moveLeg(VarSpeedServo &s, int angle) {
  s.write(angle, LEG_SPEED, IS_WAIT);
}

void moveHand(VarSpeedServo &s, int angle) {
  s.write(angle, HAND_SPEED, IS_WAIT);
}

void servoInit() {

  myservo4.attach(SERVO_4, FEETCH_MIN, FEETCH_MAX);
  myservo5.attach(SERVO_5, FEETCH_MIN, FEETCH_MAX);
  myservo6.attach(SERVO_6, FEETCH_MIN, FEETCH_MAX);

  myservo7.attach(SERVO_7, TOWER_MIN, TOWER_MAX);
  myservo8.attach(SERVO_8, TOWER_MIN, TOWER_MAX);

  myservo9.attach(SERVO_9, FEETCH_MIN, FEETCH_MAX);
  myservo10.attach(SERVO_10, FEETCH_MIN, FEETCH_MAX);
  myservo11.attach(SERVO_11, FEETCH_MIN, FEETCH_MAX);
}

void toStartAll() {

  moveLeg(myservo4, S4_START);
  moveLeg(myservo5, S5_START);
  moveLeg(myservo6, S6_START);

  moveHand(myservo7, S7_START);
  moveHand(myservo8, S8_START);

  moveLeg(myservo9, S9_START);
  moveLeg(myservo10, S10_START);
  moveLeg(myservo11, S11_START);

  delay(500);
}

void setup() {
  pinMode(HC_TRIG, OUTPUT); // trig выход
  pinMode(HC_ECHO, INPUT);  // echo вход

  Serial.begin(9600);
  print("Program start");

  servoInit();
  toStartAll();

  int leg_shift = 10;
  int hand_shift = 90;
  int stage_delay = 500;

  // Hello

  moveHand(myservo8, S8_START - hand_shift);
  delay(1000);

  moveHand(myservo8, S8_START);
  delay(stage_delay);

  // Танец руками

  hand_shift = 30;

  for (int i = 0; i < 2; i++) {

    moveHand(myservo7, S7_START + hand_shift);
    moveHand(myservo8, S8_START + hand_shift);

    delay(400);

    moveHand(myservo7, S7_START - hand_shift);
    moveHand(myservo8, S8_START - hand_shift);

    delay(400);
  }

  moveHand(myservo7, S7_START);
  moveHand(myservo8, S8_START);

  delay(stage_delay);

  // Поднять левую ногу

  hand_shift = 90;

  moveHand(myservo8, S8_START - hand_shift);
  moveHand(myservo7, S7_START + hand_shift);

  moveLeg(myservo10, S10_START - leg_shift);
  moveLeg(myservo11, S11_START + leg_shift);

  delay(1000);

  moveHand(myservo8, S8_START);
  moveHand(myservo7, S7_START);

  moveLeg(myservo10, S10_START);
  moveLeg(myservo11, S11_START);

  delay(stage_delay);

  // Поднять правую ногу

  moveHand(myservo8, S8_START - hand_shift);
  moveHand(myservo7, S7_START + hand_shift);

  moveLeg(myservo5, S5_START - leg_shift);
  moveLeg(myservo4, S4_START + leg_shift);

  delay(1000);

  moveHand(myservo8, S8_START);
  moveHand(myservo7, S7_START);

  moveLeg(myservo5, S5_START);
  moveLeg(myservo4, S4_START);

  delay(stage_delay);

  // Bye Bye

  moveHand(myservo8, 0);

  delay(1000);

  for (int i = 0; i < 2; i++) {

    moveHand(myservo8, 10);
    delay(100);

    moveHand(myservo8, 0);
    delay(100);
  }

  delay(1000);

  moveHand(myservo8, S8_START);
}

void loop() {
  float dist = getDist(HC_TRIG, HC_ECHO);   // получаем расстояние
  Serial.println(dist);
  if (dist < 20) {
    // What?
    moveHand(myservo8, 0);
  
    delay(1000);
  
    for (int i = 0; i < 2; i++) {
  
      moveHand(myservo8, 10);
      delay(100);
  
      moveHand(myservo8, 0);
      delay(100);
    }
  
    delay(1000);
  
    moveHand(myservo8, S8_START);
  }
}
