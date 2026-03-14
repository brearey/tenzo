# Wrestaling robot project by OII

## Bluetooth + ardiuno + android tablet

1. Bluetooth модуль JDY-31 (аналог популярного HC-06) Mac: 00:18:91:D6:89:89
2. Arduino UNO R3
3. Серво Tower pro (micro servo 9g) SG90
4. Джойстик ардуино KY-023 две оси + кнопка

Based on lesson [https://alexgyver.ru/lessons/bt-jdy-31/](https://alexgyver.ru/lessons/bt-jdy-31/)

`SoftwareSerial mySerial(3, 2); // RX, TX`

> Тут важно что 3 это вместо 0 (RX) в ардуино, 2 это вместо 1 (TX)