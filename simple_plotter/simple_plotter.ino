#include "HX711.h"

#define DT_PIN A0
#define SCK_PIN A1

HX711 scale;

// Median-5 buffer
float ring5[5] = {0, 0, 0, 0, 0};
uint8_t ringIndex = 0;
bool ringFilled = false;

// медианный фильтр для удаления шумов и сглаживания сигнала (последние 5 записей)
float medianFilter(float x) {
  ring5[ringIndex] = x;
  ringIndex = (ringIndex + 1) % 5;
  if (ringIndex == 0) {
    ringFilled = true;
  }

  float tmp[5];
  uint8_t n = ringFilled ? 5 : ringIndex;
  if (n == 0) {
    return x;
  }

  for (uint8_t i = 0; i < n; i++) {
    tmp[i] = ring5[i];
  }

  // сортировка
  for (uint8_t i = 1; i < n; i++) {
    float key = tmp[i];
    int8_t j = i - 1;
    while (j >= 0 && tmp[j] > key) {
      tmp[j + 1] = tmp[j];
      j--;
    }
    tmp[j + 1] = key;
  }

  return tmp[n / 2];
}

unsigned long atStarted;
#define DURATION (unsigned long)((millis() - atStarted))
#define MAX_DURATION 3000
bool isTimePrinted;

float calibrate() {
  unsigned long startTime = millis();
  float sum = 0;
  float offset = 0;
  int count = 0;
  
  while(millis() - startTime < MAX_DURATION) {
    sum += scale.get_units(1);
    count++;
    delay(10);
  }
  
  if (count > 0) {
    offset = sum / count;
  } else {
    offset = 0;
  }
  
  Serial.println("calibrating end");
  Serial.print("offset = ");
  Serial.println(offset);
  return offset;
}

void setup() {
  // init
  Serial.begin(115200);
  scale.begin(DT_PIN, SCK_PIN);
  scale.set_scale();
  scale.tare();
  atStarted = millis();
  isTimePrinted = false;

  // calibrating
  float offset = calibrate();
  // delay(100);
  // scale.set_offset(offset);

  // main loop
  while(true) {
    Serial.print("raw:");
    Serial.print(medianFilter(scale.get_value(1)), 1);
    Serial.print("\toffsetted:");
    Serial.println(medianFilter(scale.get_value(1) - offset), 1);
  }
}

void loop() {
  // Serial.print("median:");
  // Serial.println(medianFilter(raw), 10);
  // if (raw > 10000 && !isTimePrinted) {
  //   Serial.print("time:");
  //   Serial.println((unsigned long)((millis() - atStarted) / 1000UL));
  //   isTimePrinted = true;
  // }
}
