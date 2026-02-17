#include <HX711.h>

// Пины подключения HX711
#define HX711_DT A0
#define HX711_SCK A1

// Пины светодиодов
#define LED_R 2
#define LED_G 3
#define LED_B 4

// ОПТИМИЗАЦИЯ ПАМЯТИ: Уменьшаем размеры буферов
#define WINDOW_SIZE 20        // Было 50, теперь 20 (экономия ~60 байт)
#define HISTORY_SIZE 15       // Было 30, теперь 15 (экономия ~60 байт)
#define CALIBRATION_SAMPLES 100 // Было 200, теперь 100 (экономия 400 байт!)

// Параметры системы (используем константы вместо #define для экономии?)
const float BREAK_THRESHOLD = 50.0f;
const float PARTIAL_BREAK_MIN = 20.0f;
const float PARTIAL_BREAK_MAX = 40.0f;
const float STABILIZATION_TIME = 3000.0f;
const float ALARM_RESET_TIME = 5000.0f;
const float DRIFT_TRACKING_RATE = 0.001f;

// ОПТИМИЗАЦИЯ: Убрана структура Event и вся работа с EEPROM

// Глобальные переменные (минимизируем количество)
HX711 scale;
float baseline = 0.0f;
float filteredValue = 0.0f;

// ОПТИМИЗАЦИЯ: Используем статические массивы фиксированного размера
float window[WINDOW_SIZE];
int windowIndex = 0;

float history[HISTORY_SIZE];
int historyIndex = 0;

float noiseLevel = 0.0f;
float dynamicNoiseFloor = 0.0f;

// Переменные состояния (объединяем в байты где возможно)
byte alarmState = 0;          // 0-нет, 1-обрыв, 2-частичный (экономия 3 байта вместо int)
byte calibrated = 0;          // 0/1 флаг калибровки

// Переменные времени (используем unsigned long)
unsigned long lastEventTime = 0;
unsigned long alarmStartTime = 0;
unsigned long dropTime = 0;

// Калибровочный буфер (динамически не выделяется, но большой)
float calibrationBuffer[CALIBRATION_SAMPLES];

// Переменные для детекции
float dropDetected = 0.0f;
float longTermAvg = 0.0f;
int longTermCount = 0;

void setup() {
  Serial.begin(9600);
  
  // Настройка пинов
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  setLEDColor(0, 0, 0);
  
  // Инициализация HX711
  scale.begin(HX711_DT, HX711_SCK);
  scale.set_gain(128);
  
  // Короткая задержка для стабилизации
  delay(500);
  
  // ОПТИМИЗАЦИЯ: Используем F() для строк - они хранятся во Flash, не в SRAM
  Serial.println(F("Калибровка: анализ натяжения..."));
  setLEDColor(255, 255, 0); // Желтый - калибровка
  
  // Сбор данных для калибровки
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
    if (scale.wait_ready_timeout(100)) {
      calibrationBuffer[i] = (float)scale.read();
      delay(15);
    }
  }
  
  performAutoCalibration();
  
  // Инициализация буферов
  for (int i = 0; i < WINDOW_SIZE; i++) {
    window[i] = baseline;
  }
  for (int i = 0; i < HISTORY_SIZE; i++) {
    history[i] = baseline;
  }
  
  Serial.println(F("Система готова"));
  setLEDColor(0, 255, 0); // Зеленый на 500мс
  delay(500);
  setLEDColor(0, 0, 0);
}

void loop() {
  if (scale.wait_ready_timeout(100)) {
    long reading = scale.read();
    float currentValue = (float)reading;
    
    // Медианная фильтрация
    currentValue = medianFilter(currentValue);
    
    // Скользящее среднее
    window[windowIndex] = currentValue;
    windowIndex = (windowIndex + 1) % WINDOW_SIZE;
    
    float sum = 0;
    for (int i = 0; i < WINDOW_SIZE; i++) {
      sum += window[i];
    }
    filteredValue = sum / WINDOW_SIZE;
    
    // Обновление истории
    history[historyIndex] = filteredValue;
    historyIndex = (historyIndex + 1) % HISTORY_SIZE;
    
    if (calibrated) {
      trackLongTermDrift(filteredValue);
      detectEvents(filteredValue);
      printData(filteredValue);
    }
    
    updateLEDs();
    delay(30); // Частота ~30 Гц
  }
}

/**
 * Автокалибровка с отбрасыванием выбросов
 */
void performAutoCalibration() {
  // Простая сортировка пузырьком (неэффективно, но не критично для калибровки)
  for (int i = 0; i < CALIBRATION_SAMPLES - 1; i++) {
    for (int j = i + 1; j < CALIBRATION_SAMPLES; j++) {
      if (calibrationBuffer[i] > calibrationBuffer[j]) {
        float temp = calibrationBuffer[i];
        calibrationBuffer[i] = calibrationBuffer[j];
        calibrationBuffer[j] = temp;
      }
    }
  }
  
  // Используем процентили для отбрасывания выбросов
  int startIdx = (int)(CALIBRATION_SAMPLES * 0.2);
  int endIdx = (int)(CALIBRATION_SAMPLES * 0.8);
  
  float sum = 0;
  for (int i = startIdx; i < endIdx; i++) {
    sum += calibrationBuffer[i];
  }
  
  baseline = sum / (endIdx - startIdx);
  
  // Расчет стандартного отклонения
  float variance = 0;
  for (int i = startIdx; i < endIdx; i++) {
    float diff = calibrationBuffer[i] - baseline;
    variance += diff * diff;
  }
  float stdDev = sqrt(variance / (endIdx - startIdx));
  
  // Нормировка
  noiseLevel = (baseline > 0) ? (stdDev / baseline) * 100.0f : 1.0f;
  dynamicNoiseFloor = noiseLevel * 2.5f;
  
  calibrated = 1;
  
  Serial.print(F("Базовое натяжение: "));
  Serial.println(baseline);
  Serial.print(F("Уровень шума: "));
  Serial.print(noiseLevel);
  Serial.println(F("%"));
}

/**
 * Медианный фильтр 5-го порядка
 */
float medianFilter(float newValue) {
  static float buffer[5] = {0};
  static int index = 0;
  float sorted[5];
  
  buffer[index] = newValue;
  index = (index + 1) % 5;
  
  // Копируем
  for (int i = 0; i < 5; i++) {
    sorted[i] = buffer[i];
  }
  
  // Сортировка вставками
  for (int i = 1; i < 5; i++) {
    float key = sorted[i];
    int j = i - 1;
    while (j >= 0 && sorted[j] > key) {
      sorted[j + 1] = sorted[j];
      j--;
    }
    sorted[j + 1] = key;
  }
  
  return sorted[2];
}

/**
 * Отслеживание долговременного дрейфа
 */
void trackLongTermDrift(float value) {
  if (longTermCount < 500) {
    longTermAvg = (longTermAvg * longTermCount + value) / (longTermCount + 1);
    longTermCount++;
  } else {
    longTermAvg = longTermAvg * (1 - DRIFT_TRACKING_RATE) + value * DRIFT_TRACKING_RATE;
  }
  
  float driftPercent = ((longTermAvg - baseline) / baseline) * 100.0f;
  
  if (abs(driftPercent) > noiseLevel && abs(driftPercent) < PARTIAL_BREAK_MIN) {
    // Медленная коррекция baseline
    baseline = baseline * 0.999f + longTermAvg * 0.001f;
  }
}

/**
 * Детекция событий
 */
void detectEvents(float currentValue) {
  static float dropDetected = 0.0f;
  static unsigned long dropTime = 0;
  
  float historyAvg = 0;
  for (int i = 0; i < HISTORY_SIZE; i++) {
    historyAvg += history[i];
  }
  historyAvg /= HISTORY_SIZE;
  
  // Поиск резкого падения
  if (historyAvg > baseline * 0.7f) {
    float instantDrop = ((historyAvg - currentValue) / historyAvg) * 100.0f;
    
    if (instantDrop > dynamicNoiseFloor && instantDrop > 5.0f) {
      dropDetected = instantDrop;
      dropTime = millis();
    }
  }
  
  // Анализ после стабилизации
  if (dropDetected > 0 && (millis() - dropTime) >= STABILIZATION_TIME) {
    // Стабильное значение после падения
    float stableSum = 0;
    for (int i = 0; i < HISTORY_SIZE/2; i++) {
      int idx = (historyIndex - 1 - i + HISTORY_SIZE) % HISTORY_SIZE;
      stableSum += history[idx];
    }
    float newStable = stableSum / (HISTORY_SIZE/2);
    
    float totalDrop = ((baseline - newStable) / baseline) * 100.0f;
    
    if (totalDrop > BREAK_THRESHOLD) {
      triggerAlarm(1, totalDrop);
      baseline = newStable; // Обновляем baseline
    }
    else if (totalDrop >= PARTIAL_BREAK_MIN && totalDrop <= PARTIAL_BREAK_MAX) {
      triggerAlarm(2, totalDrop);
      baseline = newStable; // Обновляем baseline
    }
    
    dropDetected = 0;
  }
}

/**
 * Активация тревоги
 */
void triggerAlarm(byte type, float dropPercent) {
  if (millis() - lastEventTime < ALARM_RESET_TIME) {
    return;
  }
  
  alarmState = type;
  alarmStartTime = millis();
  lastEventTime = millis();
  
  Serial.print(F("ТРЕВОГА "));
  if (type == 1) {
    Serial.print(F("ОБРЫВ"));
  } else {
    Serial.print(F("ЧАСТИЧНЫЙ"));
  }
  Serial.print(F(": падение "));
  Serial.print(dropPercent);
  Serial.println(F("%"));
}

/**
 * Вывод в Serial Plotter
 */
void printData(float value) {
  float percent = ((value - baseline) / baseline) * 100.0f;
  
  Serial.print("S:");
  Serial.print(percent);
  Serial.print(",N:");
  Serial.print(-dynamicNoiseFloor);
  Serial.print(",B:");
  Serial.print(-BREAK_THRESHOLD);
  Serial.print(",P:");
  Serial.print(-PARTIAL_BREAK_MIN);
  Serial.println();
}

/**
 * Управление светодиодом
 */
void setLEDColor(byte r, byte g, byte b) {
  analogWrite(LED_R, r);
  analogWrite(LED_G, g);
  analogWrite(LED_B, b);
}

/**
 * Обновление индикации
 */
void updateLEDs() {
  static unsigned long lastBlink = 0;
  static byte blinkState = 0;
  
  if (!calibrated) {
    setLEDColor(255, 255, 0);
    return;
  }
  
  unsigned long now = millis();
  
  if (alarmState == 1) {
    // Красный мигающий
    if (now - lastBlink > 250) {
      lastBlink = now;
      blinkState = !blinkState;
      setLEDColor(blinkState ? 255 : 0, 0, 0);
    }
    if (now - alarmStartTime > ALARM_RESET_TIME) {
      alarmState = 0;
      setLEDColor(0, 0, 0);
    }
  } 
  else if (alarmState == 2) {
    // Желтый мигающий
    if (now - lastBlink > 300) {
      lastBlink = now;
      blinkState = !blinkState;
      setLEDColor(blinkState ? 255 : 0, blinkState ? 255 : 0, 0);
    }
    if (now - alarmStartTime > ALARM_RESET_TIME) {
      alarmState = 0;
      setLEDColor(0, 0, 0);
    }
  } else {
    // Индикатор работы (короткая вспышка зеленым)
    if (now % 5000 < 50) {
      setLEDColor(0, 50, 0); // Уменьшил яркость для экономии?
    } else {
      setLEDColor(0, 0, 0);
    }
  }
}