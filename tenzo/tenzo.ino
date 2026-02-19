#include <HX711.h>
#include <math.h>

// Пины HX711
const uint8_t HX711_DT = A0;
const uint8_t HX711_SCK = A1;

// Пины RGB-ленты (общий анод: HIGH = выкл, LOW = вкл)
const uint8_t LED_R = 2;
const uint8_t LED_G = 3;
const uint8_t LED_B = 4;
const uint8_t LED_VCC = 13;

// Пороговые значения
const float FULL_BREAK_THRESHOLD = 8.5f;
const float PARTIAL_BREAK_MIN = 3.0f;
const float PARTIAL_BREAK_MAX = 7.5f;

// Тайминги
const unsigned long CALIBRATION_MS = 10000UL;
const unsigned long SAMPLE_PERIOD_MS = 30UL;
const unsigned long FULL_CONFIRM_MS = 500UL;
const unsigned long PARTIAL_CONFIRM_MS = 300UL;

// Фильтрация
const float FILTER_ALPHA = 0.12f; // простой НЧ‑фильтр
const float BASELINE_ADAPT_ALPHA = 0.002f; // медленное отслеживание дрейфа

// Состояния
enum State : uint8_t {
  CALIBRATING = 0, // мигает синим
  NORMAL = 1, // горит зеленым
  PARTIAL_BREAK = 2, // мигает желтым
  FULL_BREAK = 3 // мигает красным
};

HX711 scale;

State state = CALIBRATING;
State candidateState = NORMAL;

float baseline = 0.0f;
float filtered = 0.0f;
float noiseBandPercent = 2.0f;

unsigned long calibStartMs = 0;
unsigned long lastSampleMs = 0;
unsigned long candidateStartMs = 0;
unsigned long alarmStartMs = 0;

// Накопители калибровки
float calibSum = 0.0f;
float calibAbsDiffSum = 0.0f;
uint16_t calibCount = 0;
float calibMeanEstimate = 0.0f;

// Управляет цветом RGB-ленты (true = выключить)
void setRGB(bool rOff, bool gOff, bool bOff) {
  digitalWrite(LED_R, rOff ? HIGH : LOW);
  digitalWrite(LED_G, gOff ? HIGH : LOW);
  digitalWrite(LED_B, bOff ? HIGH : LOW);
}

// Меняет состояние системы и фиксирует момент аварии
void setState(State newState) {
  state = newState;
  if (newState == PARTIAL_BREAK || newState == FULL_BREAK) {
    alarmStartMs = millis();
  }
}

// Запускает этап калибровки (сбор базовой линии)
void startCalibration() {
  setState(CALIBRATING);
  calibStartMs = millis();
  calibSum = 0.0f;
  calibAbsDiffSum = 0.0f;
  calibCount = 0;
  calibMeanEstimate = 0.0f;

  Serial.println(F("CAL: start 10s baseline capture"));
}

// Завершает калибровку и вычисляет baseline и шум
void finishCalibration() {
  if (calibCount == 0) {
    // Запасной вариант, если датчик был не готов
    baseline = 1.0f;
    filtered = 1.0f;
    noiseBandPercent = 5.0f;
  } else {
    baseline = calibSum / (float)calibCount;
    filtered = baseline;

    float meanAbsDiff = calibAbsDiffSum / (float)calibCount;
    noiseBandPercent = (baseline != 0.0f) ? fabsf(meanAbsDiff / baseline) * 100.0f * 3.0f : 5.0f;
    if (noiseBandPercent < 2.0f) {
      noiseBandPercent = 2.0f;
    }
    if (noiseBandPercent > 10.0f) {
      noiseBandPercent = 10.0f;
    }
  }

  candidateState = NORMAL;
  candidateStartMs = 0;
  setState(NORMAL);

  Serial.print(F("CAL: baseline="));
  Serial.print(baseline);
  Serial.print(F(", noiseBand="));
  Serial.print(noiseBandPercent);
  Serial.println(F("%"));
}

void updateCalibration(float raw) {
  calibCount++;
  calibSum += raw;

  // Бегущая оценка среднего абсолютного отклонения
  if (calibCount == 1) {
    calibMeanEstimate = raw;
  } else {
    calibMeanEstimate += (raw - calibMeanEstimate) / (float)calibCount;
  }
  calibAbsDiffSum += fabsf(raw - calibMeanEstimate);

  if (millis() - calibStartMs >= CALIBRATION_MS) {
    finishCalibration();
  }
}

// Печатает сообщение о срабатывании аварии
void emitAlarm(State alarmType, float dropPercent) {
  if (alarmType == FULL_BREAK) {
    Serial.print(F("ALARM FULL_BREAK drop="));
  } else {
    Serial.print(F("ALARM PARTIAL_BREAK drop="));
  }
  Serial.print(dropPercent);
  Serial.println(F("%"));
}

// Детектирует обрыв по проценту падения относительно baseline
void detectBreak(float value) {
  if (baseline == 0.0f) {
    return;
  }

  float dropPercent = ((baseline - value) / baseline) * 100.0f;

  State target = NORMAL;
  if (dropPercent >= FULL_BREAK_THRESHOLD) {
    target = FULL_BREAK;
  } else if (dropPercent >= PARTIAL_BREAK_MIN && dropPercent <= PARTIAL_BREAK_MAX) {
    target = PARTIAL_BREAK;
  }

  if (target == NORMAL) {
    candidateState = NORMAL;
    candidateStartMs = 0;

    // Медленная компенсация дрейфа только в зоне нормы
    if (fabsf(dropPercent) <= noiseBandPercent) {
      baseline = baseline * (1.0f - BASELINE_ADAPT_ALPHA) + value * BASELINE_ADAPT_ALPHA;
    }
    return;
  }

  if (candidateState != target) {
    candidateState = target;
    candidateStartMs = millis();
    return;
  }

  if (candidateStartMs == 0) {
    candidateStartMs = millis();
    return;
  }

  unsigned long needed = (target == FULL_BREAK) ? FULL_CONFIRM_MS : PARTIAL_CONFIRM_MS;
  if (millis() - candidateStartMs >= needed) {
    if (state != target) {
      emitAlarm(target, dropPercent);
      setState(target);
    }
  }
}

// Обновляет индикацию (калибровка, норма, аварии)
void updateLED() {
  static unsigned long blinkTick = 0;
  static bool blinkOn = false;
  unsigned long now = millis();

  if (state == CALIBRATING) {
    if (now - blinkTick > 250UL) {
      blinkTick = now;
      blinkOn = !blinkOn;
    }
    // Синий мигает во время калибровки
    setRGB(true, true, blinkOn ? false : true);
    return;
  }

  if (state == FULL_BREAK) {
    if (now - blinkTick > 180UL) {
      blinkTick = now;
      blinkOn = !blinkOn;
    }
    setRGB(blinkOn ? false : true, true, true); // красный мигает
    return;
  }

  if (state == PARTIAL_BREAK) {
    if (now - blinkTick > 250UL) {
      blinkTick = now;
      blinkOn = !blinkOn;
    }
    setRGB(blinkOn ? false : true, blinkOn ? false : true, true); // желтый мигает
    return;
  }

  // NORMAL: постоянный зеленый
  setRGB(true, false, true);
}

// Выводит данные для Serial Plotter
void printPlot(float value) {
  if (baseline == 0.0f) {
    return;
  }

  float dropPercent = ((baseline - value) / baseline) * 100.0f;

  // Формат для Serial Plotter
  Serial.print(F("drop:"));
  Serial.print(dropPercent);
  Serial.print(F(",full:"));
  Serial.print(FULL_BREAK_THRESHOLD);
  Serial.print(F(",partialMin:"));
  Serial.print(PARTIAL_BREAK_MIN);
  Serial.print(F(",partialMax:"));
  Serial.println(PARTIAL_BREAK_MAX);
}

void setup() {
  Serial.begin(9600);

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_VCC, OUTPUT);
  digitalWrite(LED_VCC, HIGH);
  setRGB(true, true, true);

  scale.begin(HX711_DT, HX711_SCK);
  scale.set_gain(128);

  delay(300);
  startCalibration();
}

void loop() {
  updateLED();

  unsigned long now = millis();
  if (now - lastSampleMs < SAMPLE_PERIOD_MS) {
    return;
  }
  lastSampleMs = now;

  if (!scale.wait_ready_timeout(60)) {
    return;
  }

  float raw = (float)scale.read();

  // Инициализация фильтра
  if (filtered == 0.0f) {
    filtered = raw;
  } else {
    filtered = filtered * (1.0f - FILTER_ALPHA) + raw * FILTER_ALPHA;
  }

  if (state == CALIBRATING) {
    updateCalibration(filtered);
    return;
  }

  detectBreak(filtered);
  printPlot(filtered);
}
