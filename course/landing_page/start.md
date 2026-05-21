# Общение по проекту с AI

## Gemini:

Прочитай все документы в папке materials/markdown и медиа в папках images, videos. Не смотри папку docx. Сделай:

1. Структура сайта landing page в отдельном файле landing.md. Сайт для продажи курса + набора и защиты как проекта перед комиссией для получения гранта
2. Промпт генерации картинок. Промпт должен быть понятным для любой AI генерации картинок. Картинки по курсу и по образовательному набору. Это для сайта. Промпты в отдельном файле generate_images.md
3. Список вопросов для формы регистрации. Форма должна быть на сайте. Список в вопросов сохрани в register.md

Информация: Сам курс будет на платфорем coreapp.ai доступ к которому будем давать по почте из ответов на форму регистрации. Не создавай сайт или картинку. Только планируем.

## Gemini after generated images

1. Посмотри папку materials/generated_images. Не смотри папку trash. Обнови landing.md с учетом сгенерированных картинок для landing page.
2. Добавь в landing.md про стек: HTML, CSS, JS. Про использование CSS анимаций. Про адаптивный экран.

## Add teachers

Исправь Блок 8: Команда проекта в файле landing.md с учетом того что я добавил фотографии учителей в папке images/teachers

## Start dev LP

1. в папке site создай все необходимые html, css, js файлы
2. скопируй в папку site/img все изображения из materials/images
2. скопируй в папку site/video все видео из materials/videos
3. прочитай файл landing.md
4. сделай landing page по структуре landing.md

## Claude AI

lorriant@genesys:~/Arduino/course/landing_page/site$ ls -l
итого 8
drwxrwxr-x 4 lorriant lorriant 4096 мая 21 14:47 img
-rw-rw-r-- 1 lorriant lorriant    0 мая 21 14:52 index.html
-rw-rw-r-- 1 lorriant lorriant    0 мая 21 14:52 script.js
-rw-rw-r-- 1 lorriant lorriant    0 мая 21 14:53 style.css
drwxrwxr-x 2 lorriant lorriant 4096 мая 21 14:47 video

lorriant@genesys:~/Arduino/course/landing_page/site$ ls -l img/
итого 8
drwxrwxr-x 2 lorriant lorriant 4096 мая 21 14:47 generated_images
drwxrwxr-x 2 lorriant lorriant 4096 мая 21 14:47 teachers

lorriant@genesys:~/Arduino/course/landing_page/site$ ls -l img/generated_images/
итого 12880
-rw-rw-r-- 1 lorriant lorriant 2103415 мая 21 14:47 "A close-up shot of teenagers' hands assembling a blocky yellow robot.png"
-rw-rw-r-- 1 lorriant lorriant  278695 мая 21 14:47 'A dynamic action shot.png'
-rw-rw-r-- 1 lorriant lorriant 2050198 мая 21 14:47 'A flat lay of a robotics kit neatly arranged.png'
-rw-rw-r-- 1 lorriant lorriant  185577 мая 21 14:47 'A friendly, cartoonish, blocky humanoid.png'
-rw-rw-r-- 1 lorriant lorriant 2083631 мая 21 14:47 'A teenager is focused on a computer screen.png'
-rw-rw-r-- 1 lorriant lorriant  280236 мая 21 14:47 'humanoid hands up.png'
-rw-rw-r-- 1 lorriant lorriant 2017562 мая 21 14:47 'Over-the-shoulder shot of a student writing code on a laptop.png'
-rw-rw-r-- 1 lorriant lorriant 1858476 мая 21 14:47 'Two friendly, blocky, humanoid robots wrestling.png'
-rw-rw-r-- 1 lorriant lorriant 2312452 мая 21 14:47 'Yakut teenagers.png'

lorriant@genesys:~/Arduino/course/landing_page/site$ ls -l img/teachers/
итого 1936
-rwxr-xr-x 1 lorriant lorriant 1144876 мая 21 14:47 'Евгений Семенович программирование.jpg'
-rwxr-xr-x 1 lorriant lorriant   36427 мая 21 14:47 'Ирина Ивановна схемотехника.jpeg'
-rwxr-xr-x 1 lorriant lorriant  797523 мая 21 14:47 'Сергей Александрович 3Д моделирование.jpg

lorriant@genesys:~/Arduino/course/landing_page/site$ ls -l video/
итого 6240
-rw-rw-r-- 1 lorriant lorriant 4272394 мая 21 14:47 'концепт борьбы роботов хапсагай.mp4'
-rw-rw-r-- 1 lorriant lorriant 2109981 мая 21 14:47 'концепт робота хапсагай.mp4

1. посмотри эту структуру с сделай landing page по структуре landing.md
2. мне нужны только содержимое файлов: index.html, style.css, script.js