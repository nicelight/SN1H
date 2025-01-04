


//////// ModBus
//#include <ModbusRTUSlave.h>
//#define RS485_DE_PIN 21
//ModbusRTUSlave modbus(Serial1, RS485_DE_PIN);  // (Serial1, DEpin) ; TX1 = 18; RX1 = 19;
//bool coils[2];
//bool discreteInputs[2];
//uint16_t holdingRegisters[2];
//uint16_t inputRegisters[2];
//////// ModBus
//


////  в цикле loop

//  static uint32_t prevMs = 0;
//  if ((millis() - prevMs) > 1000ul) {
//    prevMs = millis();
//    byte reg0 = (millis() / 1000) % 60;
//    byte reg1 = reg0 + 1;
//
//    inputRegisters[0] = reg0;
//    inputRegisters[1] = reg1;
//    coils[0] = !coils[0];
//    Serial.print("\tcoil_0 "); //
//    Serial.print(coils[0]);
//    Serial.print("\tcoil_1 "); //
//    Serial.print(coils[1]);
//    Serial.print("\tInpReg0(RO)  "); //
//    Serial.print(reg0);
//    Serial.print("\tInpReg1(RO) ");
//    Serial.print(reg1);
//    Serial.print("\tholdReg0(RW) ");
//    Serial.print(holdingRegisters[0]);
//    Serial.print("\tholdReg1(RW) ");
//    Serial.print(holdingRegisters[0]);
//    Serial.println();
//
//    //read_pzem(); // считываем pzem
//  }//each 1 sec

//  modbus.poll();
//  digitalWrite(LED, coils[1]);


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

  //как считывать флоат из регистров
  union Float {
  float    m_float;
  uint16_t  m_sh[sizeof(float)/2];
  } t,s;
  t.m_sh[0]=modbusregisters[CURRENT_TEMP_REGISTER_LOW];
  t.m_sh[1]=modbusregisters[CURRENT_TEMP_REGISTER_HIGH];
  s.m_sh[0]=modbusregisters[TARGET_TEMP_REGISTER_LOW];
  s.m_sh[1]=modbusregisters[TARGET_TEMP_REGISTER_HIGH];
*/
