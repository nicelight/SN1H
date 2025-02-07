#include "pcf8575_pwm.h"

#define PCF_ADDR 0x20
PCF8575_PWM pwm(PCF_ADDR);

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, 0);
  pwm.begin();
  pwm.pinModePWM(0);
  pwm.setPWM(0, 50); // 50% заполнение на пине 0
}

void loop() {
  pwm.tick(); // Обновление PWM (автоматически, пока есть активные каналы)
    static unsigned long lastTime = micros();
    unsigned long now = micros();
    Serial.println(now - lastTime); // Выведет фактический интервал
    lastTime = now;
    
  if ((millis() > 5000) && (millis() < 5200)) {
    pwm.setPWM(0, 200); // 0
  digitalWrite(13, 1);
  }
}
