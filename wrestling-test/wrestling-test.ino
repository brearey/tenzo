#include <VarSpeedServo.h>

// Создаем объекты сервоприводов
VarSpeedServo myservo1;
VarSpeedServo myservo2;
VarSpeedServo myservo3;
VarSpeedServo myservo4;
VarSpeedServo myservo5;
VarSpeedServo myservo6;

// Настройки
#define SPEED 40      // 0=full speed, 1-255 slower to faster
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

void toStart() {
  // обе ноги на исходную
  servoTest(myservo1, S1_START);
  servoTest(myservo2, S2_START);
  servoTest(myservo3, S3_START);
  servoTest(myservo4, S4_START);
  servoTest(myservo5, S5_START);
  servoTest(myservo6, S6_START);
  delay(500);
}

void servoInit() {
  // Инициализация с индивидуальными диапазонами
  myservo1.attach(SERVO_1, TOWER_MIN, TOWER_MAX);
  myservo2.attach(SERVO_2, TOWER_MIN, TOWER_MAX);
  myservo3.attach(SERVO_3, FEETCH_MIN, FEETCH_MAX);
  myservo4.attach(SERVO_4, FEETCH_MIN, FEETCH_MAX);
  myservo5.attach(SERVO_5, FEETCH_MIN, FEETCH_MAX);
  myservo6.attach(SERVO_6, FEETCH_MIN, FEETCH_MAX);
}

void servoTest(VarSpeedServo s, int angle) {
  s.write(angle, SPEED, IS_WAIT);
}

void setup() {
  print("Program start");
  Serial.begin(9600);
  servoInit();
  toStart();
}

void loop() {
  int shift = 25;
  // правая нога вперед
  servoTest(myservo2, S2_START + shift);
  servoTest(myservo1, S1_START - shift);
  // левая нога назад
  servoTest(myservo5, S5_START + shift);
  servoTest(myservo6, S6_START - shift);
  
  // на исходную
  delay(500);
  toStart();

  // правая нога назад
  servoTest(myservo2, S2_START - shift);
  servoTest(myservo1, S1_START + shift);
  // левая нога впереж
  servoTest(myservo5, S5_START - shift);
  servoTest(myservo6, S6_START + shift);

  // на исходную
  delay(500);
  toStart();
}
