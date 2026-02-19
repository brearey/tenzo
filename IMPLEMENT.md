# Ход проекта

## Что дали:

1. Ardiono UNO
2. Тензодатчик мостовой 20 кг (Strain Gauge Load Cell)
3. Модуль HX711
4. Светодиодная лента (не адресная) с тремя RGB на 5 вольт
5. Груз на 100 грамм

## Изучил датчик

Сам датчик на 20 кг стоит 296 рублей [https://iarduino.ru/shop/Sensory-Datchiki/tenzodatchik-dlya-vesov-i-nh711-na-20-kg.html](https://iarduino.ru/shop/Sensory-Datchiki/tenzodatchik-dlya-vesov-i-nh711-na-20-kg.html)

> Тензодатчик 20кг — тензорезистивный датчик, предназначенный для создания на его основе весов, датчиков давления или концевых датчиков.

Справочник и примеры кода работы с тензодатчиком [https://wiki.iarduino.ru/page/hx_711_with_tenzo/?ysclid=mlkqzrz5yf4672388](https://wiki.iarduino.ru/page/hx_711_with_tenzo/?ysclid=mlkqzrz5yf4672388)

## Соединил тензодатчик с ардуино и написал код
0. Соединяю по схеме

| тензодатчик  | HX711 |
|--------------|-------|
|   Красный    |   E+  |
|   Черный     |   E-  |
|   Зеленый    |   A-  |
|   Белый      |   A+  |

| HX711  | Arduino |
|--------|---------|
|  GND   |   GND   |
|  DT    |   A0    |
|  SCK   |   A1    |
|  VCC   |   5+    |

1. Для калибровки загружаем скетч calibrate_tenzo/calibrate_tenzo.ino
2. Смотрим монитор последовательног порта. Когда говорит `You have 10 seconds to set your known load` ставим груз 100 грамм.
3. Получаем `calibration factor`
4. Далее изменяем скетч `output_tenzo/output_tenzo.ino` в строке 8 ставим измеренный `calibration factor`
5. Загружаем скетч `output_tenzo/output_tenzo.ino`
6. В мониторе порта видим вывод датчика в граммах

## Соединил светодиодную ленту с ардуино и написал код

1. Соединяю по схеме

| arduino  | лента |
|----------|-------|
|    R     |   2   |
|    G     |   3   |
|    B     |   4   |
|    5+    |  VCC  |

2. Написал код `rbg/rgb.ino`
3. Лента горит

## Изучаем возможности библиотеки HX711

1. Документация [https://github.com/bogde/HX711](https://github.com/bogde/HX711)
2. Introduction to Load Cells [https://www.aandd.jp/products/weighing/loadcell/introduction/introduction_to_load_cells.html](https://www.aandd.jp/products/weighing/loadcell/introduction/introduction_to_load_cells.html)

## Logic

1. `Калибровка`: 10 секунд собираем базовую линию `baseline` и оцениваем шум, формируем `noiseBandPercent`.
2. Во время `калибровки` светодиоды мигают синим.
3. Используется экспоненциальное сглаживание (EMA) входных данных: `filtered = 0.88 * filtered + 0.12 * raw`.
4. Считаем процент падения: `dropPercent = (baseline - filtered) / baseline * 100`.
5. Состояния системы:
6. `CALIBRATING` — калибровка
7. `NORMAL` — норма, горит зелёный
8. `PARTIAL_BREAK` — частичный обрыв, мигает жёлтый
9. `FULL_BREAK` — полный обрыв, мигает красный
10. Если `dropPercent` в пределах `noiseBandPercent`, базовая линия медленно подстраивается (`BASELINE_ADAPT_ALPHA`) для компенсации дрейфа.
11. Для срабатывания требуется подтверждение по времени: `PARTIAL_CONFIRM_MS` и `FULL_CONFIRM_MS`.
12. После срабатывания аварии состояние фиксируется и не сбрасывается автоматически (нужен ручной сброс/перезапуск).
