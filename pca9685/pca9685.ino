/**
 * PCA9685 - Управление одним сервоприводом
 * 
 * Плата: Arduino Uno/Nano
 * Драйвер: PCA9685 16-Channel 12-bit PWM
 * Серво: Tower Pro SG90 или аналог
 * 
 * Схема подключения:
 * PCA9685 -> Arduino:
 *   VCC  -> 5V (логика)
 *   GND  -> GND
 *   SDA  -> A4 (Uno) / D4 (Nano)
 *   SCL  -> A5 (Uno) / D5 (Nano)
 * 
 * PCA9685 -> Серво:
 *   V+   -> 5V (питание серво)
 *   GND  -> GND (общий)
 *   CH0  -> Сигнал серво (желтый/оранжевый провод)
 * 
 * Питание серво:
 *   При одном SG90 можно питать от Arduino 5V
 *   При нескольких серво - внешний источник 5V 2A+
 */

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Создаем объект драйвера PCA9685
// Адрес по умолчанию: 0x40
Adafruit_PWMServoDriver servoDriver = Adafruit_PWMServoDriver(0x40);

// Настройки серво
#define SERVO_CHANNEL 0      // Канал PCA9685 (0-15)
#define SERVO_MIN_US 500     // Минимальная длительность импульса (мкс)
#define SERVO_MAX_US 2500    // Максимальная длительность импульса (мкс)
#define SERVO_MIN_DEG 20      // Минимальный угол (градусы)
#define SERVO_MAX_DEG 160    // Максимальный угол (градусы)

// Частота PWM для серво (стандарт 50Hz)
#define PWM_FREQUENCY 50

// Параметры плавного движения
#define MOVE_STEP 1          // Шаг изменения угла (градусы)
#define MOVE_DELAY 15        // Задержка между шагами (мс)

void setup() {
  Serial.begin(9600);
  Serial.println(F("PCA9685 Servo Control"));
  Serial.println(F("Введите угол: 0-180"));

  // Инициализация драйвера
  servoDriver.begin();
  servoDriver.setPWMFreq(PWM_FREQUENCY);
  
  Serial.println(F("PCA9685 инициализирован"));
  
  // Установка начальной позиции (90 градусов - центр)
  moveToAngle(90);
  delay(500);
  
  Serial.println(F("Готово к работе"));
}

void loop() {
  // Чтение команды из Serial
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    int angle = input.toInt();
    
    // Проверка диапазона
    if (angle >= SERVO_MIN_DEG && angle <= SERVO_MAX_DEG) {
      Serial.print(F("Угол: "));
      Serial.println(angle);
      moveToAngle(angle);
    } else {
      Serial.print(F("Ошибка! Введите число от "));
      Serial.print(SERVO_MIN_DEG);
      Serial.print(F(" до "));
      Serial.println(SERVO_MAX_DEG);
    }
  }
}

/**
 * Преобразование угла в тики PWM
 * PCA9685 имеет 12-bit разрешение (0-4095)
 * Период при 50Hz = 20ms = 20000мкс
 * 1 тик = 20000 / 4096 ≈ 4.88 мкс
 */
int angleToPulse(int angle) {
  // Маппинг угла на длительность импульса
  long pulseWidth = map(angle, SERVO_MIN_DEG, SERVO_MAX_DEG, 
                        SERVO_MIN_US, SERVO_MAX_US);
  
  // Преобразование мкс в тики PCA9685
  int pulse = (pulseWidth * 4096) / 20000;
  
  return pulse;
}

/**
 * Установка серво на нужный угол
 */
void setServoAngle(int angle) {
  int pulse = angleToPulse(angle);
  servoDriver.setPWM(SERVO_CHANNEL, 0, pulse);
}

/**
 * Плавное перемещение к углу
 */
void moveToAngle(int targetAngle) {
  int currentAngle = 90; // TODO: хранить текущий угол в переменной
  
  // Определение направления движения
  int step = (targetAngle > currentAngle) ? MOVE_STEP : -MOVE_STEP;
  
  // Плавное перемещение
  for (int angle = currentAngle; 
       (step > 0 && angle <= targetAngle) || (step < 0 && angle >= targetAngle); 
       angle += step) {
    setServoAngle(angle);
    delay(MOVE_DELAY);
  }
  
  // Финальная установка
  setServoAngle(targetAngle);
}
