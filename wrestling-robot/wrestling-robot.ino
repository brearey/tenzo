#include <SoftwareSerial.h>
// Используем (3, 2) как более надежный вариант для JDY-31/HC-06 на Uno.
SoftwareSerial mySerial(3, 2); 

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

// Буфер для приема команд
const int BUFFER_SIZE = 20;
char inputBuffer[BUFFER_SIZE];
int bufferIndex = 0;

void print(String msg) {
  mySerial.println(msg);
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
  
  // Начальная позиция (центр), чтобы робот не дергался при старте
  myservo1.write(93, SPEED, IS_WAIT);
  myservo2.write(93, SPEED, IS_WAIT);
  myservo3.write(93, SPEED, IS_WAIT);
  myservo4.write(93, SPEED, IS_WAIT);
  myservo5.write(93, SPEED, IS_WAIT);
  myservo6.write(93, SPEED, IS_WAIT);
}

void processCommand(char* cmd) {
  // Ожидаем формат "ID:Угол", например "1:180"
  char* separator = strchr(cmd, ':');
  
  if (separator == NULL) {
    return;
  }

  *separator = 0; // Разделяем строку
  int servoId = atoi(cmd);        // Номер серво (до ':')
  int angle = atoi(separator + 1); // Угол (после ':')

  // Проверка корректности угла
  if (angle < 0 || angle > 180) {
    print("Err: Angle 0-180");
    return;
  }

  // Выполняем движение
  switch (servoId) {
    case 1: myservo1.write(angle, SPEED, IS_WAIT); break;
    case 2: myservo2.write(angle, SPEED, IS_WAIT); break;
    case 3: myservo3.write(angle, SPEED, IS_WAIT); break;
    case 4: myservo4.write(angle, SPEED, IS_WAIT); break;
    case 5: myservo5.write(angle, SPEED, IS_WAIT); break;
    case 6: myservo6.write(angle, SPEED, IS_WAIT); break;
    default:
      print("Err: ID 1-6");
      break;
  }
  
  // Для отладки можно раскомментировать:
  print("S" + String(servoId) + "->" + String(angle));
}

void printCurrentAngle() {
  print(String(myservo1.read()));
  print(String(myservo2.read()));
  print(String(myservo3.read()));
  print(String(myservo4.read()));
  print(String(myservo5.read()));
  print(String(myservo6.read()));
}

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  servoInit();
  print("Ready. Send 'ID:Angle' (e.g. 1:180)");
}

void loop() {
  // 1. Обработка входящих данных (Bluetooth и USB)
  // Читаем из Bluetooth
  if (mySerial.available()) {
    char inChar = mySerial.read();
    if (inChar == '\n' || inChar == '\r') {
      if (bufferIndex > 0) {
        inputBuffer[bufferIndex] = '\0';
        processCommand(inputBuffer);
        bufferIndex = 0;
      }
    } else {
      if (bufferIndex < BUFFER_SIZE - 1) {
        inputBuffer[bufferIndex++] = inChar;
      } else {
        bufferIndex = 0; // Переполнение буфера
      }
    }
  }

  // Читаем из USB (для тестов через монитор порта ПК)
  if (Serial.available()) {
    char inChar = Serial.read();
    if (inChar == '\n' || inChar == '\r') {
      if (bufferIndex > 0) {
        inputBuffer[bufferIndex] = '\0';
        processCommand(inputBuffer);
        bufferIndex = 0;
      }
    } else {
      if (bufferIndex < BUFFER_SIZE - 1) {
        inputBuffer[bufferIndex++] = inChar;
      }
    }
  }
}
