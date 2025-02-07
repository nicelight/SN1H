#ifndef PCF8575_PWM_H
#define PCF8575_PWM_H

#include <Arduino.h>
#include <Wire.h>
//  https://www.mischianti.org/2019/01/02/pcf8575-i2c-digital-i-o-expander-fast-easy-usage/
#include "PCF8575.h"

#define PWM_RES 256  // Разрешение PWM (0-255)
#define PWM_FREQ 100 // Частота PWM (Гц)

class PCF8575_PWM {
public:
    PCF8575_PWM(uint8_t address) : pcf(address) {
        pwmInterval = 1000000UL / (PWM_FREQ * PWM_RES);
    }

    void begin() {
        pcf.begin();
    }

    void pinModePWM(uint8_t pin) {
        if (pin < 16) {
            pcf.pinMode(pin, OUTPUT); // Устанавливаем как OUTPUT
            pwmEnabled[pin] = true;   // Отмечаем, что пин задействован для PWM
        }
    }

    void setPWM(uint8_t pin, uint8_t duty) {
        if (pin < 16 && pwmEnabled[pin]) {
            pwmDuty[pin] = duty;
            if (duty > 0) activeChannels++;
            else if (activeChannels > 0) activeChannels--;
        }
    }
void tick() {
    if (activeChannels == 0) return;

    unsigned long now = micros();
    if (now - lastUpdate >= pwmInterval) {
        lastUpdate = now;
        pwmCounter = (pwmCounter + 1) % PWM_RES;

        uint16_t outputState = 0; // Храним все 16 бит (пины)

        for (uint8_t i = 0; i < 16; i++) {
            if (pwmEnabled[i] && pwmDuty[i] > pwmCounter) {
                outputState |= (1 << i); // Устанавливаем бит HIGH
            }
        }

        pcf.write(outputState); // Отправляем всё за 1 I²C-команду
    }
}
private:
    PCF8575 pcf;
    uint8_t pwmDuty[16] = {0};
    bool pwmEnabled[16] = {false}; // Какие пины разрешены для PWM
    uint8_t pwmCounter = 0;
    unsigned long lastUpdate = 0;
    unsigned long pwmInterval;
    uint8_t activeChannels = 0;
};

#endif
