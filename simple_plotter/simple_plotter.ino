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

void setup() {
  Serial.begin(115200);
  scale.begin(DT_PIN, SCK_PIN);
  scale.set_scale();
  scale.tare();
}

void loop() {
  float raw = scale.get_units(1);
  Serial.print("median:");
  Serial.println(medianFilter(raw), 10);
}
