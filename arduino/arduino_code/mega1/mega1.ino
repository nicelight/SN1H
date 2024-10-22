#define LED 13
#define ON 0
#define OFF 1

#include "GyverButton.h"
GButton butt1(2, HIGH_PULL, NORM_OPEN);


/*
  ModbusRTUSlaveExample

  This example demonstrates how to setup and use the ModbusRTUSlave library (https://github.com/CMB27/ModbusRTUSlave).
  - RX pin to RO pin TTL-RS485
  - TX pin to DI pin TTL-RS485
  - GND to GND of the master/client board
*/

////// ModBus
#include <ModbusRTUSlave.h>
#define RS485_DE_PIN 21
ModbusRTUSlave modbus(Serial1, RS485_DE_PIN);  // (Serial1, DEpin) ; TX1 = 18; RX1 = 19;
bool coils[2];
bool discreteInputs[2];
uint16_t holdingRegisters[2];
uint16_t inputRegisters[2];
////// ModBus


///// энерджиметр
#include <PZEM004Tv30.h>
#if !defined(PZEM_RX_PIN) && !defined(PZEM_TX_PIN)
#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17
#endif
#define PZEM_SERIAL Serial2
PZEM004Tv30 pzem;
///// энерджиметр



void led_blink() // мигнем светодиодом
{
  digitalWrite(LED, 1);
  delay (50);
  digitalWrite(LED, 0);
  delay (20);
}

void read_pzem()
{
  Serial.print("PZEM addr:");
  Serial.println(pzem.getAddress(), HEX);
  Serial.println("===================");
  // Read the data from the sensor
  float voltage = pzem.voltage();
  float current = pzem.current();
  float power = pzem.power();
  float energy = pzem.energy();
  float frequency = pzem.frequency();
  float pf = pzem.pf();
  // Print the values to the Serial console
  Serial.print("Voltage: ");      Serial.print(voltage);      Serial.println("V");
  Serial.print("Current: ");      Serial.print(current);      Serial.println("A");
  Serial.print("Power: ");        Serial.print(power);        Serial.println("W");
  Serial.print("Energy: ");       Serial.print(energy, 3);     Serial.println("kWh");
  Serial.print("Frequency: ");    Serial.print(frequency, 1); Serial.println("Hz");
  Serial.print("PF: ");           Serial.println(pf);
  Serial.println("-------------------");
  Serial.println();
}//read_pzem()


void statesAll(bool state)  // установить всем пинам состояние
{
  for (int i = 38; i < 54; i++) {
    digitalWrite (i, state);
    delay(50);
  }
  for (int i = 2; i < 16; i++) {
    digitalWrite (i, state);
    delay(50);
  }
  digitalWrite (22, state);
  delay(50);
  digitalWrite (23, state);
  delay(50);
}


void initPins() {
  // INPUTS
  for (int i = 32; i < 38; i++) { // 32-37 входы
    pinMode(i, INPUT_PULLUP);
  }
  for (int i = 54; i < 70; i++) { // 54-69 входы (А0-А15)
    pinMode(i, INPUT_PULLUP);
  }
  // OUTPUTS
  for (int i = 2; i < 16; i++) { // 2-15 выходы
    pinMode (i, OUTPUT);
    digitalWrite(i, OFF);
  }
  pinMode (22, OUTPUT); // 22, 23 выходы
  pinMode (23, OUTPUT);
  for (int i = 38; i < 54; i++) { //38-53 выходы
    pinMode (i, OUTPUT);
    digitalWrite(i, OFF);
  }

  statesAll(ON);  // помигаем всеми пинами
  statesAll(OFF);
  pinMode(LED, OUTPUT);
  pinMode(2, INPUT_PULLUP);  // 2, 3 кнопка
  pinMode(3, OUTPUT);
  digitalWrite(3, 0);

  butt1.setDebounce(30);        // настройка антидребезга (по умолчанию 80 мс)
  butt1.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
}



void setup() {
  // put your setup code here, to run once:
  initPins();
  Serial.begin(116200);
  led_blink();  led_blink();  led_blink();


  modbus.configureCoils(coils, 2);                       // bool array of coil values, number of coils
  modbus.configureDiscreteInputs(discreteInputs, 2);     // bool array of discrete input values, number of discrete inputs
  modbus.configureHoldingRegisters(holdingRegisters, 2); // unsigned 16 bit integer array of holding register values, number of holding registers
  modbus.configureInputRegisters(inputRegisters, 2);     // unsigned 16 bit integer array of input register values, number of input registers
  modbus.begin(1, 9600); // (slave.ID, Serial.speed);

  pzem = PZEM004Tv30(PZEM_SERIAL, 0x10); // энерджиметр
  //  delay(5000);
}//setup

void loop() {
  butt1.tick();  // обязательная функция отработки. Должна постоянно опрашиваться
  if (butt1.isClick()) Serial.println("Click");         // проверка на один клик
  if (butt1.isSingle()) Serial.println("Single");       // проверка на один клик
  if (butt1.isDouble()) Serial.println("Double");       // проверка на двойной клик
  if (butt1.isTriple()) Serial.println("Triple");       // проверка на тройной клик
  if (butt1.isHolded()) Serial.println("Holded");       // проверка на удержание

  //  if (butt1.isPress()) Serial.println("Press");         // нажатие на кнопку (+ дебаунс)
  //  if (butt1.isRelease()) Serial.println("Release");     // отпускание кнопки (+ дебаунс)
  //  if (butt1.isHold()) Serial.println("Holding");        // проверка на удержание
  //if (butt1.state()) Serial.println("Hold");          // возвращает состояние кнопки    // проверка на тройной клик

  static uint32_t prevMs = 0;
  if ((millis() - prevMs) > 1000ul) {
    prevMs = millis();
    byte reg0 = (millis() / 1000) % 60;
    byte reg1 = reg0 + 1;

    inputRegisters[0] = reg0;
    inputRegisters[1] = reg1;
    Serial.print("\tInpReg0(RO)  "); //
    Serial.print(reg0);
    Serial.print("\tInpReg1(RO) ");
    Serial.print(reg1);
    Serial.print("\tholdReg0(RW) ");
    Serial.print(holdingRegisters[0]);
    Serial.print("\tholdReg1(RW) ");
    Serial.print(holdingRegisters[0]);
    Serial.println();

    //read_pzem(); // считываем pzem
  }//each 1 sec

  modbus.poll();
  digitalWrite(LED, coils[1]);

  /////////// MODBUS
  //  inputRegisters[0] = map(analogRead(potPins[0]), 0, 1023, 0, 255);
  //  inputRegisters[1] = map(analogRead(potPins[1]), 0, 1023, 0, 255);
  //  discreteInputs[0] = !digitalRead(buttonPins[0]);
  //  discreteInputs[1] = !digitalRead(buttonPins[1]);
  //  modbus.poll();
  //  analogWrite(ledPins[0], holdingRegisters[0]);
  //  analogWrite(ledPins[1], holdingRegisters[1]);
  //  digitalWrite(ledPins[3], coils[1]);
  /////////// MODBUS


}// loop


///// MODBUS примеры 

/*
    как записывать флоат в 2 регистра

  union Float {
    float    m_float;
    uint16_t  m_sh[sizeof(float)/2];
  } target_temperature;
  target_temperature.m_float=20.0;
  modbusregisters[TARGET_TEMP_REGISTER_HIGH]=target_temperature.m_sh[1];
  modbusregisters[TARGET_TEMP_REGISTER_LOW]=target_temperature.m_sh[0];
*/




/*
   как считывать флоат из регистров

  union Float {
  float    m_float;
  uint16_t  m_sh[sizeof(float)/2];
  } t,s;
  t.m_sh[0]=modbusregisters[CURRENT_TEMP_REGISTER_LOW];
  t.m_sh[1]=modbusregisters[CURRENT_TEMP_REGISTER_HIGH];
  s.m_sh[0]=modbusregisters[TARGET_TEMP_REGISTER_LOW];
  s.m_sh[1]=modbusregisters[TARGET_TEMP_REGISTER_HIGH];


*/
