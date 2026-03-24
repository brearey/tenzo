#include <VarSpeedServo.h>

// Создаем объекты сервоприводов
VarSpeedServo myservo1;
VarSpeedServo myservo2;
VarSpeedServo myservo3;
VarSpeedServo myservo4;
VarSpeedServo myservo5;
VarSpeedServo myservo6;

// Настройки
#define SPEED 50      // 0=full speed, 1-255 slower to faster
#define IS_WAIT false // false = асинхронно (не блокирует код), true = ждать окончания движения

// Пины сервоприводов согласно wrestling-robot.ino
#define SERVO_1 4
#define SERVO_2 5
#define SERVO_3 6
#define SERVO_4 7
#define SERVO_5 8
#define SERVO_6 9

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
  servoTest(myservo2, 85);
  delay(200);
  servoTest(myservo1, 86);
  delay(200);
  servoTest(myservo5, 100);
  delay(200);
  servoTest(myservo6, 100);
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
  // правая нога вперед
  servoTest(myservo2, 110);
  servoTest(myservo1, 85);
  // левая нога назад
  servoTest(myservo5, 110);
  servoTest(myservo6, 85);
  delay(500);
  toStart();
}

void loop() {
  
}
