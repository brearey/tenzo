# Wrestaling robot project by OII

## Bluetooth + ardiuno + android tablet

1. Bluetooth модуль JDY-31 (аналог популярного HC-06) Mac: 00:18:91:D6:89:89
2. Arduino nano
3. Серво Tower pro (micro servo 9g - gram) SG90 `Диапазон ширины принимаемого импульса: 500–2500 мкс` [https://iarduino.ru/shop/Mehanika/privod-postoyannogo-vrascheniya.html?ysclid=mmvg5u90p4161720963](https://iarduino.ru/shop/Mehanika/privod-postoyannogo-vrascheniya.html?ysclid=mmvg5u90p4161720963)
4. Сервопривод Feetech FS90 `Диапазон ширины принимаемого импульса: 544–2400 мкс` [https://wiki.amperka.ru/products:servo-fs90](https://wiki.amperka.ru/products:servo-fs90)
5. Джойстик ардуино KY-023 две оси + кнопка

Based on lesson [https://alexgyver.ru/lessons/bt-jdy-31/](https://alexgyver.ru/lessons/bt-jdy-31/)

`SoftwareSerial mySerial(3, 2); // RX, TX`

> Тут важно что 3 это вместо 0 (RX) в ардуино, 2 это вместо 1 (TX)

## Servo

Based on lesson: [https://alexgyver.ru/lessons/servo/](https://alexgyver.ru/lessons/servo/)

About servo [https://iarduino.ru/shop/Mehanika/privod-postoyannogo-vrascheniya.html](https://iarduino.ru/shop/Mehanika/privod-postoyannogo-vrascheniya.html)

> Сервопривод потребляет довольно приличный ток (стартовый до 1А), поэтому рекомендуется питать его от внешнего источника. При питании от компьютера напряжение может просесть и МК перезагрузится, при высокой нагрузке (с 2 и больше приводов) может выгореть защитный диод на плате Arduino!

При первом подключении без команд серво ставит себе угол 93 градуса. Если питание слабое то ардуино уходит на перезагрузку.

## Tests

Подключил три серво через бутерброд из слоев: ардуино уно + амперка мотор шилд (с питанием 7.4 В) + тройка шилд. При одновременной работе всех трех серво при достижении 700-800 мА ардуино перезагружается.

Переделал подключение:

Внешний источник питания LiPo аккумулятор 45C 1800 mAh на 7.4V
Сервоприводы SG90 шесть штук

Arduino uno | Аккумулятор
---------------------
GND | GND
VIN | VCC

Все сервоприводы подключены коричневым проводом к GND ардуино uno. Красный провод подключен напрямую к VCC аккумулятора. Сигнальные провода подключены к пинам 4,5,6,7,8,9. Параллельно к VCC и GND всех сервоприводов подключен один конденсатор 25В 470 мкФ.

## Измерения

Сила тока на 6 серво во время одновременной работы достигает максимум 1300 мА. Напряжение в диапазоне 7.54-7.64 вольт

## Servo library with slow motion

> attach(pin, min, max  ) - Attaches to a pin setting min and max values in microseconds
> default min is 544, max is 2400

[https://github.com/netlabtoolkit/VarSpeedServo](https://github.com/netlabtoolkit/VarSpeedServo)

## Презентация 1 июня

Проект wrestling: должны быть два робота с дист управлением через android + гироскоп. ДО 1 июня