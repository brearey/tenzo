#include <VarSpeedServo.h>

// Создаем объекты сервоприводов
VarSpeedServo myservo1;
VarSpeedServo myservo2;
VarSpeedServo myservo3;
VarSpeedServo myservo4;
VarSpeedServo myservo5;
VarSpeedServo myservo6;

// Настройки
#define SPEED 60      // Скорость (1-255, где 0 = макс скорость, 255 = очень медленно)
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
  Serial.begin(9600);
  servoInit();
  print("Program start");
}

void loop() {
  
}
