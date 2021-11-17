#include "Config.h"

#if DEEP_SLEEP_APP

#include <Arduino.h>

#define SDS_ENABLE_PIN 23U
#define DELAY_1_SECOND 1000U

RTC_DATA_ATTR int bootCount = 0;

void setup()
{
    Serial.begin(115200);
    pinMode(SDS_ENABLE_PIN, OUTPUT);
    delay(DELAY_1_SECOND);


    ++bootCount;
    Serial.println("Boot number: " + String(bootCount));
    Serial.println("Wakeup cause " + String(esp_sleep_get_wakeup_cause()));

    // 5 second wakeup timer
    esp_sleep_enable_timer_wakeup(5 * 1000000);

    digitalWrite(SDS_ENABLE_PIN, HIGH);
    delay(DELAY_1_SECOND);

    /* digitalWrite(SDS_ENABLE_PIN, LOW);
    delay(DELAY_1_SECOND); */

    Serial.println("Going to sleep now");

    gpio_hold_en(GPIO_NUM_23);
    gpio_deep_sleep_hold_en();

    delay(3000U);
    esp_deep_sleep_start();
}

void loop()
{

}

#endif /* DEEP_SLEEP_APP */