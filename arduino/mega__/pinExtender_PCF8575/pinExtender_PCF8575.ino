/*
   PCF8575 GPIO Port Expand
   http://nopnop2002.webcrow.jp/WeMos/WeMos-25.html

   PCF8575    ----- WeMos
   A0         ----- GRD
   A1         ----- GRD
   A2         ----- GRD
   VSS        ----- GRD
   VDD        ----- 5V/3.3V
   SDA        ----- GPIO_4(PullUp)
   SCL        ----- GPIO_5(PullUp)

   P0     ----------------- LED0
   P1     ----------------- LED1
   P2     ----------------- LED2
   P3     ----------------- LED3
   P4     ----------------- LED4
   P5     ----------------- LED5
   P6     ----------------- LED6
   P7     ----------------- LED7
   P8     ----------------- LED8
   P9     ----------------- LED9
   P10     ----------------- LED10
   P11     ----------------- LED11
   P12     ----------------- LED12
   P13     ----------------- LED13
   P14     ----------------- LED14
   P15     ----------------- LED15

*/

#include "Arduino.h"
#include "PCF8575.h"  // https://github.com/xreef/PCF8575_library

// Set i2c address
PCF8575 pcf8575(0x20);

void setup()
{
  Serial.begin(9600);

  // Set pinMode to OUTPUT
  for (int i = 0; i < 16; i++) {
    pcf8575.pinMode(i, OUTPUT);
  }
  pcf8575.begin();
pinMode(13, OUTPUT);
}

void loop()
{
  for (int i = 0; i < 16; i++) {
    pcf8575.digitalWrite(i, HIGH);
    digitalWrite(13, 1);
  }
    delay(500);
  for (int i = 0; i < 16; i++) {
    pcf8575.digitalWrite(i, LOW);
    digitalWrite(13, 0);
  }
    delay(500);

}
