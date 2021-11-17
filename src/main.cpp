#include "Config.h"
#if MAIN_APP

/* Includes */
#include "WiFiConfig.h"
#include <Adafruit_ADS1X15.h>
#include <Arduino.h>
#include <HTTPClient.h>
#include <SDS011.h>
#include <SPI.h>
#include <WiFi.h>

/* Global objects */
Adafruit_ADS1115 ads;
HardwareSerial port(2);
HTTPClient http;
SDS011 sds;

/* Global variables */
String serverName = "http://localhost:8086/write?db=test_db?";

/* Defines and macros */

/* Functions prototypes */
void InitWiFi(void);
void ReadAndPrintADSMeasurements(void);
void ReadAndPrintSDSMeasurements(void);
void SendToDatabase(void);

void setup(void)
{
    Serial.begin(115200);

    InitWiFi();

    if (!ads.begin())
    {
        Serial.println("Failed to initialize ADS.");
        while (1);
    }

    sds.begin(&port);

}

void loop(void)
{
    ReadAndPrintADSMeasurements();
    delay(2000);
    ReadAndPrintSDSMeasurements();
    delay(5000);
    SendToDatabase();
    delay(2000);
}

void InitWiFi(void)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(1000);
    }
    Serial.println(WiFi.localIP());
}

void ReadAndPrintADSMeasurements(void)
{
    int16_t adc0, adc1, adc2, adc3;
    float volts0, volts1, volts2, volts3;

    adc0 = ads.readADC_SingleEnded(0);
    adc1 = ads.readADC_SingleEnded(1);
    adc2 = ads.readADC_SingleEnded(2);
    adc3 = ads.readADC_SingleEnded(3);

    volts0 = ads.computeVolts(adc0);
    volts1 = ads.computeVolts(adc1);
    volts2 = ads.computeVolts(adc2);
    volts3 = ads.computeVolts(adc3);

    Serial.println("-----------------------------------------------------------");
    Serial.print("AIN0: "); Serial.print(adc0); Serial.print("  "); Serial.print(volts0); Serial.println("V");
    Serial.print("AIN1: "); Serial.print(adc1); Serial.print("  "); Serial.print(volts1); Serial.println("V");
    Serial.print("AIN2: "); Serial.print(adc2); Serial.print("  "); Serial.print(volts2); Serial.println("V");
    Serial.print("AIN3: "); Serial.print(adc3); Serial.print("  "); Serial.print(volts3); Serial.println("V");
}

void ReadAndPrintSDSMeasurements(void)
{
    float p10, p25;
    int err;

    sds.wakeup();
    Serial.println("SDS wakeup");
    delay(5000);

    err = sds.read(&p25, &p10);

    if (!err)
    {
        Serial.println("P2.5: " + String(p25));
        Serial.println("P10:  " + String(p10));
    }
    else
    {
        Serial.println("SDS read error");
    }

    sds.sleep();
    Serial.println("SDS sleep");
}

void SendToDatabase(void)
{
    if(WiFi.status()== WL_CONNECTED)
    {
        HTTPClient http;

        // Your Domain name with URL path or IP address with path
        http.begin(serverName.c_str());

        // Send HTTP GET request
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0)
        {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            String payload = http.getString();
            Serial.println(payload);
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

#endif /* MAIN_APP */