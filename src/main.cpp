#include "Config.h"
#if MAIN_APP

/* Includes */
#include "Secrets.h"
#include <Adafruit_ADS1X15.h>
#include <Arduino.h>
#include <HTTPClient.h>
#include <SDS011.h>
#include <SPI.h>
#include <WiFi.h>

/* Global objects */
Adafruit_ADS1115 ads;
HardwareSerial port(2);
SDS011 sds;

/* Global variables */

/* Variables resistant to software reboots */
RTC_NOINIT_ATTR uint8_t wifiConnectionAttempts;

/* Defines and macros */
#define ADS_CHANNEL_0  0U
#define ADS_CHANNEL_1  1U
#define ADS_CHANNEL_2  2U
#define ADS_CHANNEL_3  3U
#define SDS_ENABLE_PIN 23U
/* ToDo change to 30000 in production*/
#define SDS_READ_DELAY 10000U

/* Functions prototypes */
void PrepareResistantVariables(void);
void InitWiFi(void);
void ADSMeasurement(void);
void SDSMeasurement(void);
void SendToDatabase(void);
void DeepSleep(void);

/* Init function */
void setup(void)
{
    Serial.begin(115200);
    pinMode(SDS_ENABLE_PIN, OUTPUT);
    PrepareResistantVariables();
    InitWiFi();

    if (!ads.begin())
    {
        Serial.println("Failed to initialize ADS.");
    }

    sds.begin(&port);

}

/* Main loop function */
void loop(void)
{
    ADSMeasurement();
    SDSMeasurement();
    SendToDatabase();
}

/* Prepare variables that are resistatnt after reboot
 * Initialize only if it is first power up
 */
void PrepareResistantVariables(void)
{
    Serial.println("Wakeup cause " + String(esp_sleep_get_wakeup_cause()));
    Serial.println("Reset cause " + String(esp_reset_reason()));

    if (esp_reset_reason() == ESP_RST_POWERON)
    {
        wifiConnectionAttempts = 0U;
    }
}

/* Function to connect to WIFI */
void InitWiFi(void)
{
    /* Time in seconds before reboot */
    uint8_t connectionTimeout = 0U;

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    ++wifiConnectionAttempts;

    Serial.print("Connecting to WiFi attempt = " + String(wifiConnectionAttempts));

    /* Wait until wifi is connected */
    while (WiFi.status() != WL_CONNECTED)
    {
        ++connectionTimeout;
        Serial.println("Attemp=" + String(connectionTimeout));
        delay(1000);

        /* Try to connect for WIFI_TIMEOUT seconds */
        if (connectionTimeout >= WIFI_TIMEOUT)
        {
            Serial.println("Unable to connect to wifi");

            /* Reboot ESP and try again, after WIFI_CONNECTION_ATTEMPTS go to deep sleep */
            if (wifiConnectionAttempts < WIFI_CONNECTION_ATTEMPTS)
            {
                ESP.restart();
            }
            else
            {
                DeepSleep();
            }
        }
    }

    Serial.println(WiFi.localIP());
    wifiConnectionAttempts = 0U;
}

/* Function for reading ADS sensor values */
void ADSMeasurement(void)
{
    /* ADS reads in integer */
    int16_t adcCh0Reading = 0U;
    int16_t adcCh1Reading = 0U;
    int16_t adcCh2Reading = 0U;
    int16_t adcCh3Reading = 0U;
    /* ADS reads in volts */
    float voltsCh0 = 0.0;
    float voltsCh1 = 0.0;
    float voltsCh2 = 0.0;
    float voltsCh3 = 0.0;

    /* ADS read */
    adcCh0Reading = ads.readADC_SingleEnded(ADS_CHANNEL_0);
    adcCh1Reading = ads.readADC_SingleEnded(ADS_CHANNEL_1);
    adcCh2Reading = ads.readADC_SingleEnded(ADS_CHANNEL_2);
    adcCh3Reading = ads.readADC_SingleEnded(ADS_CHANNEL_3);

    /* Convert ADS reads to volts */
    voltsCh0 = ads.computeVolts(adcCh0Reading);
    voltsCh1 = ads.computeVolts(adcCh1Reading);
    voltsCh2 = ads.computeVolts(adcCh2Reading);
    voltsCh3 = ads.computeVolts(adcCh3Reading);

    /* Print all measured values */
    Serial.println("-----------------------------------------------------------");
    Serial.print("AIN0: "); Serial.print(adcCh0Reading); Serial.print("  "); Serial.print(voltsCh0); Serial.println("V");
    Serial.print("AIN1: "); Serial.print(adcCh1Reading); Serial.print("  "); Serial.print(voltsCh1); Serial.println("V");
    Serial.print("AIN2: "); Serial.print(adcCh2Reading); Serial.print("  "); Serial.print(voltsCh2); Serial.println("V");
    Serial.print("AIN3: "); Serial.print(adcCh3Reading); Serial.print("  "); Serial.print(voltsCh3); Serial.println("V");
}

/* Function for reading SDS sensor values */
void SDSMeasurement(void)
{
    /* Variables to store sensor readings and read status*/
    float p10 = 0.0;
    float p25 = 0.0;
    int sdsReadStatus = 0;

    /* Wakeup sensor and provide power to it */
    digitalWrite(SDS_ENABLE_PIN, HIGH);
    sds.wakeup();
    Serial.println("SDS wakeup");
    delay(SDS_READ_DELAY);

    /* Read sensor values */
    sdsReadStatus = sds.read(&p25, &p10);

    /* Check for error */
    if (!sdsReadStatus)
    {
        /* Print values */
        Serial.println("P2.5: " + String(p25));
        Serial.println("P10:  " + String(p10));
    }
    else
    {
        Serial.println("SDS read error");
    }

    /* Go to sleep, disable power */
    sds.sleep();
    digitalWrite(SDS_ENABLE_PIN, LOW);
    Serial.println("SDS sleep");
}

/* Function to send sensor values to database */
void SendToDatabase(void)
{
    if(WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        /* Start client with server address */
        http.begin(serverName.c_str());
        /* Add header for raw data */
        http.addHeader("Content-Type", "application/raw");
        /* ToDo test data for now */
        String httpRequestData = "measurements69,value=1 value_1=" + String(random(500));

        // Send HTTP GET request
        int httpResponseCode = http.POST(httpRequestData);

        if (httpResponseCode > 0)
        {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
        }
        else
        {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }

        // Free resources
        http.end();
    }
}

/* Prepare and go to deep sleep */
void DeepSleep(void)
{
    /* ToDo only print and infinite loop for now */
    Serial.println("DEEP SLEEP for 30 minutes");
    while (1);
}

#endif /* MAIN_APP */