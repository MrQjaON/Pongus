//-------------------------Настройки---------------------------------------------------
#define score_limit 3   // Максимальное колличество очков
#define paddle_speed 49  // Скорость ракеткок (чем меньше число, тем быстрее ракетка)
#define ball_speed 7     // Скорость шарика (чем меньше число, тем быстрее шарик)
#define paddle_height 8 // Размер ракеток
//-------------------------------------------------------------------------------------

#include <Adafruit_GFX.h> // Подключаем библиотеку для работы с графическими элементами
#include <Adafruit_SSD1306.h> // Подключаем библиотеку для работы с OLED-дисплеем SSD1306

#define UP_BUTTON 2 // Определяем пин для кнопки управления вверх
#define DOWN_BUTTON 3 // Определяем пин для кнопки управления вниз#include <Wire.h>

const unsigned long PADDLE_RATE = paddle_speed; // Задаем частоту обновления платформы
const unsigned long BALL_RATE = ball_speed; // Задаем частоту обновления мяча
const uint8_t PADDLE_HEIGHT = paddle_height; // Задаем высоту платформы
const uint8_t SCORE_LIMIT = score_limit; // Задаем лимит очков

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire); // Создаем объект для работы с OLED-дисплеем

bool game_over, win; // Флаги окончания игры и победы

uint8_t player_score, bot_score; // Счет игрока и компьютера
uint8_t ball_x = 24, ball_y = 30; // Начальная позиция мяча

int8_t ball_move_x = 1, ball_move_y = 1; // Направление движения мяча

unsigned long ball_update; // Время последнего обновления мяча
unsigned long paddle_update; // Время последнего обновления платформы

const uint8_t bot_x = 12; // Позиция платформы компьютера по оси X
uint8_t bot_y = 16; // Начальная позиция платформы компьютера по оси Y

const uint8_t player_x = 115; // Позиция платформы игрока по оси X
uint8_t player_y = 16; // Начальная позиция платформы игрока по оси Y

void setup() 
{
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Инициализируем OLED-дисплей

    display.display(); // Выводим на дисплей заставку
    unsigned long start = millis(); // Запоминаем время начала работы программы

    pinMode(UP_BUTTON, INPUT_PULLUP); // Устанавливаем пин для кнопки вверх как вход с подтяжкой к питанию
    pinMode(DOWN_BUTTON, INPUT_PULLUP); // Устанавливаем пин для кнопки вниз как вход с подтяжкой к питанию

    display.clearDisplay(); // Очищаем дисплей
    drawCourt(); // Рисуем игровое поле

    while(millis() - start < 1600); // Ждем 1.6 секунды для отображения заставки

    display.display(); // Выводим на дисплей

    ball_update = millis(); // Запоминаем время первого обновления мяча
    paddle_update = ball_update; // Запоминаем время первого обновления платформы
    game_over = false; // Сбрасываем флаг окончания игры
    win = false; // Сбрасываем флаг победы
}

void loop() 
{
    bool update_needed = false; // Флаг необходимости обновления экрана
    unsigned long time = millis(); // Получаем текущее время

    static bool up_state = false; // Состояние кнопки вверх
    static bool down_state = false; // Состояние кнопки вниз
    
    up_state |= (digitalRead(UP_BUTTON) == LOW); // Проверяем нажатие кнопки вверх
    down_state |= (digitalRead(DOWN_BUTTON) == LOW); // Проверяем нажатие кнопки вниз
    // Логика обновления положения мяча//
    if(time > ball_update) // Проверяем, прошло ли достаточно времени для обновления мяча
    {
        uint8_t new_x = ball_x + ball_move_x; // Вычисляем новую позицию мяча по оси X
        uint8_t new_y = ball_y + ball_move_y; // Вычисляем новую позицию мяча по оси Y

        // Проверяем столкновение с вертикальными стенами
        if(new_x == 0 || new_x == 127) 
        {
            ball_move_x = -ball_move_x; // Меняем направление движения мяча по оси X
            new_x += ball_move_x + ball_move_x; // Корректируем позицию мяча

            // Проверяем, кто заработал очко
            if (new_x > 64)
            {
                player_score++; // Увеличиваем счет игрока
            }
            else
            {
                bot_score++; // Увеличиваем счет компьютера
            }

            // Проверяем условие завершения игры
            if (player_score == SCORE_LIMIT || bot_score == SCORE_LIMIT)
            {
                win = player_score > bot_score; // Определяем победителя
                game_over = true; // Устанавливаем флаг окончания игры
            }
        }

        // Проверяем столкновение с горизонтальными стенами
        if(new_y == 0 || new_y == 53) 
        {
            ball_move_y = -ball_move_y; // Меняем направление движения мяча по оси Y
            new_y += ball_move_y + ball_move_y; // Корректируем позицию мяча
        }

        // Проверяем столкновение с платформой компьютера
        if(new_x == bot_x && new_y >= bot_y && new_y <= bot_y + PADDLE_HEIGHT) 
        {
            ball_move_x = -ball_move_x; // Меняем направление движения мяча по оси X
            new_x += ball_move_x + ball_move_x; // Корректируем позицию мяча
        }

        // Проверяем столкновение с платформой игрока
        if(new_x == player_x && new_y >= player_y && new_y <= player_y + PADDLE_HEIGHT)
        {
            ball_move_x = -ball_move_x; // Меняем направление движения мяча по оси X
            new_x += ball_move_x + ball_move_x; // Корректируем позицию мяча
        }

        display.drawPixel(ball_x, ball_y, BLACK); // Стираем предыдущее положение мяча
        display.drawPixel(new_x, new_y, WHITE); // Рисуем новое положение мяча
        ball_x = new_x; // Обновляем координату мяча по оси X
        ball_y = new_y; // Обновляем координату мяча по оси Y

        ball_update += BALL_RATE; // Обновляем время последнего обновления мяча

        update_needed = true; // Устанавливаем флаг необходимости обновления экрана
    }
    // Логика обновления положения платформы игрока и компьютера //
    if(time > paddle_update) // Проверяем, прошло ли достаточно времени для обновления платформы
    {
        paddle_update += PADDLE_RATE; // Обновляем время последнего обновления платформы

        // Платформа компьютера
        display.drawFastVLine(bot_x, bot_y, PADDLE_HEIGHT, BLACK); // Стираем предыдущее положение платформы компьютера
        const uint8_t half_paddle = PADDLE_HEIGHT >> 1; // Вычисляем половину высоты платформы

        // Двигаем платформу компьютера к мячу
        if(bot_y + half_paddle > ball_y)
        {
            bot_y -= 1; // Двигаем платформу компьютера вверх
        }

        if(bot_y + half_paddle < ball_y)
        {
            bot_y += 1; // Двигаем платформу компьютера вниз
        }

        // Проверяем, не вышла ли платформа компьютера за границы экрана
        if(bot_y < 1) 
        {
            bot_y = 1; // Устанавливаем платформу компьютера вверхней границе
        }

        if(bot_y + PADDLE_HEIGHT > 53)
        {
            bot_y = 53 - PADDLE_HEIGHT; // Устанавливаем платформу компьютера нижней границе
        }

        // Платформа игрока
        display.drawFastVLine(bot_x, bot_y, PADDLE_HEIGHT, WHITE); // Рисуем платформу компьютера
        display.drawFastVLine(player_x, player_y, PADDLE_HEIGHT, BLACK); // Стираем предыдущее положение платформы игрока

        // Управление платформой игрока
        if(up_state) 
        {
            player_y -= 1; // Перемещаем платформу игрока вверх
        }

        if(down_state) 
        {
            player_y += 1; // Перемещаем платформу игрока вниз
        }

        // Проверяем, не вышла ли платформа игрока за границы экрана
        if(player_y < 1) 
        {
            player_y = 1; // Устанавливаем платформу игрока вверхней границе
        }

        if(player_y + PADDLE_HEIGHT > 53) 
        {
            player_y = 53 - PADDLE_HEIGHT; // Устанавливаем платформу игрока нижней границе
        }

        up_state = down_state = false; // Сбрасываем состояние кнопок управления

        display.drawFastVLine(player_x, player_y, PADDLE_HEIGHT, WHITE); // Рисуем платформу игрока

        update_needed = true; // Устанавливаем флаг необходимости обновления экрана
    }
   // Логика обновления дисплея и проверка окончания игры //
    if(update_needed) // Если необходимо обновление экрана
    {
        if (game_over) // Если игра окончена
        {
            const char* text = win ? "YOU LOSE!" : "YOU WIN!"; // Устанавливаем текст в зависимости от результата игры
            display.clearDisplay(); // Очищаем дисплей
            display.setCursor(40, 28); // Устанавливаем курсор для текста
            display.print(text); // Выводим текст на дисплей
            display.display(); // Обновляем дисплей

            delay(5000); // Ждем 5 секунд

            // Сбрасываем настройки для новой игры
            display.clearDisplay(); // Очищаем дисплей
            ball_x = 24; // Устанавливаем начальную позицию мяча
            ball_y = 35;
            ball_move_x = 1; // Устанавливаем начальное направление движения мяча
            ball_move_y = 1;
            bot_y = 16; // Устанавливаем начальную позицию платформы компьютера
            player_y = 16; // Устанавливаем начальную позицию платформы игрока
            bot_score = 0; // Сбрасываем счет компьютера
            player_score = 0; // Сбрасываем счет игрока
            game_over = false; // Сбрасываем флаг окончания игры
            drawCourt(); // Рисуем игровое поле
        }

        // Выводим текущий счет на дисплей
        display.setTextColor(WHITE, BLACK); // Устанавливаем цвет текста
        display.setCursor(10, 56); // Устанавливаем позицию для вывода счета компьютера
        display.print(bot_score); // Выводим счет компьютера
        display.setCursor(112, 56); // Устанавливаем позицию для вывода счета игрока
        display.print(player_score); // Выводим счет игрока
        display.display(); // Обновляем дисплей
    }
}

void drawCourt() // Функция для отрисовки игрового поля
{
    display.drawRect(0, 0, 128, 54, WHITE); // Рисуем прямоугольник для игрового поля
}
