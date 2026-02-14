#include "HX711.h"                                            // подключаем библиотеку для работы с тензодатчиком

#define DT  A0                                                // Указываем номер вывода, к которому подключен вывод DT  датчика
#define SCK A1                                                // Указываем номер вывода, к которому подключен вывод SCK датчика

HX711 scale;                                                  // создаём объект scale

float calibration_factor = -3.78;
float conversation_rate = 0.035274;
float units;                                                  // задаём переменную для измерений в граммах
float gramms;                                                 // задаём переменную для измерений в унциях

void setup() {
  Serial.begin(9600);                                         // инициируем работу последовательного порта на скорости 9600 бод
  scale.begin(DT, SCK);                                       // инициируем работу с датчиком
  scale.set_scale();                                          // выполняем измерение значения без калибровочного коэффициента
  scale.tare();                                               // сбрасываем значения веса на датчике в 0
  scale.set_scale(calibration_factor);                        // устанавливаем калибровочный коэффициент
}

void loop() {
  Serial.print("Reading: ");                                  // выводим текст в монитор последовательного порта
  units = scale.get_units(10);
  gramms = units * conversation_rate;                                  // переводим вес из унций в граммы
  Serial.print(gramms);
  Serial.println(" grams");
}
