# Wrestaling robot project by OII

## Bluetooth + ardiuno + android tablet

1. Bluetooth модуль JDY-31 (аналог популярного HC-06) Mac: 00:18:91:D6:89:89
2. Arduino UNO R3
3. Серво Tower pro (micro servo 9g - gram) SG90
4. Джойстик ардуино KY-023 две оси + кнопка

Based on lesson [https://alexgyver.ru/lessons/bt-jdy-31/](https://alexgyver.ru/lessons/bt-jdy-31/)

`SoftwareSerial mySerial(3, 2); // RX, TX`

> Тут важно что 3 это вместо 0 (RX) в ардуино, 2 это вместо 1 (TX)

## Servo

Based on lesson: [https://alexgyver.ru/lessons/servo/](https://alexgyver.ru/lessons/servo/)

About servo [https://iarduino.ru/shop/Mehanika/privod-postoyannogo-vrascheniya.html](https://iarduino.ru/shop/Mehanika/privod-postoyannogo-vrascheniya.html)

> Сервопривод потребляет довольно приличный ток (стартовый до 1А), поэтому рекомендуется питать его от внешнего источника. При питании от компьютера напряжение может просесть и МК перезагрузится, при высокой нагрузке (с 2 и больше приводов) может выгореть защитный диод на плате Arduino!

При первом подключении без команд серво ставит себе угол 93 градуса. Если питание слабое то ардуино уходит на перезагрузку.