// ESP32 DEVKITV1
// SDS RX -> TX2
// SDS TX -> RX2

#include "Config.h"

#if SDS_TEST_APP

#include <Arduino.h>
#include <SDS011.h>

float p10, p25;
int err;

SDS011 my_sds;
HardwareSerial port(2);

void setup()
{
  my_sds.begin(&port);
  Serial.begin(115200);
}

void loop()
{
  my_sds.wakeup();
  Serial.println("Wakeup");
  delay(5000);

  err = my_sds.read(&p25, &p10);

  if (!err)
  {
    Serial.println("P2.5: " + String(p25));
    Serial.println("P10:  " + String(p10));
  }
  else
  {
    Serial.println("SDS read error");
  }

  my_sds.sleep();
  Serial.println("Sleep");
  delay(5000);
}

#endif /* SDS_TEST_APP */