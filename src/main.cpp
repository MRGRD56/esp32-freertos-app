#include <Arduino.h>
#include <EEPROM.h>
#include "../lib/button.h"
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, U8X8_PIN_NONE, SCL, SDA);

button ledButton = button(4);

volatile ushort value = 0;
volatile int hall = 0;

TaskHandle_t updateScreenTaskHandle;
TaskHandle_t buttonTaskHandle;

void updateScreen() {
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_10x20_t_cyrillic);
        u8g2.setCursor(0, 16);
        u8g2.printf("Val: %d", value);
        u8g2.setCursor(0, 38);
        u8g2.printf("Hall: %d", hall);
    } while (u8g2.nextPage());
}

void updateScreenTask(void* pvParam) {
    while (true) {
        if (xTaskNotifyWait(0, 0, nullptr, portMAX_DELAY) == pdTRUE) {
            updateScreen();
        }
    }
}

void updateScreenAsync() {
    xTaskNotify(updateScreenTaskHandle, 1, eSetValueWithOverwrite);
}

void buttonTask(void* pvParam) {
    while (true) {
        if (ledButton.isPressing()) {
            value++;

            EEPROM.writeUShort(0, value);
            EEPROM.commit();

            updateScreenAsync();
        }
    }
}

void updateHallTask(void* pvParam) {
    while (true) {
        hall = hallRead();
        updateScreenAsync();
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

void setup() {
    ledButton.initialize();

    u8g2.begin();

    EEPROM.begin(sizeof(value));

    value = EEPROM.readUShort(0);

    xTaskCreate(buttonTask, "button", CONFIG_ESP_MAIN_TASK_STACK_SIZE, nullptr, 1, &buttonTaskHandle);
    xTaskCreate(updateScreenTask, "update-screen", CONFIG_ESP_MAIN_TASK_STACK_SIZE, nullptr, 1, &updateScreenTaskHandle);
    xTaskCreate(updateHallTask, "update-hall", CONFIG_ESP_MAIN_TASK_STACK_SIZE, nullptr, 1, nullptr);

    updateScreenAsync();
}

void loop() { }