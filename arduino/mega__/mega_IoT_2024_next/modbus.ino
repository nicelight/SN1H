// делаем коилсы такими же как и пины чтобы не путаться где то. 
//а на свободные номера коилсов повесим клапана теплонасоса и что нить еще 
//modbus coils 
////##################  ЗАРЕЗЕРВИРОВАНЫ пинами  ##################//#define N5_BED 3
//#define N5_SP2 4
//#define N5_SP1 5
//#define N3_FAN 6
//#define N6_FAN 7
//
//#define N6_SP 10
//#define N6_LED 11
//#define N7_SP 12
//
//#define N7_LED 24
//#define N2_TRACK 26 // бело-оранжевый
//#define N4_TR1 27 // оранженвый
//#define N4_TR2 28 // бело-зеленый
//
////#define ЗЕЛЕНЫЙ 29
////#define БЕЛОСИНИЙ 30
////#define СИНИЙ 31

// для N1_lamps.state
//#define N1_LIGHTS 31 
//#define N2_LIGHTS 32
//#define N2_TRACK_LIGHTS 14
//#define N3_LIGHTS 33 
//#define N3_FAN 6 // уже определена

//#define N4_LIGHTS 34 
//#define N4_TRACK_LIGHTS 15
//#define N4_KITCHEN_LIGHTS 16
//#define N4_MUSEUMS_LIGHTS 17
//#define N4_MEAL_LIGHTS 18
//#define N5_LIGHTS 35 
//#define N6_LIGHTS 36 
//#define N6_FAN 7
//#define N7_LIGHTS 37

//#define N5_SHELF 38
//#define N2_SP 39
//#define N1_SP2 40
//#define N1_SP1 41
//#define N2_LED 42
//#define N3_SP 43
//#define N3_LED 44
//#define N4_SP1 45
//#define N4_SP2 46
//#define N4_MEAL 49
//#define N4_KITCH1 50
//#define N4_KITCH2 51
//#define N4_ERM 52
//#define N4_MUS 53
////##################  ЗАРЕЗЕРВИРОВАНЫ пинами  ##################//




//############  Тестовый скетч ################### //

/*
  ModbusRTUSlaveExample
  This example demonstrates how to setup and use the ModbusRTUSlave library (https://github.com/CMB27/ModbusRTUSlave).
  It is intended to be used with a second board running ModbusRTUMasterExample from the ModbusRTUMaster library (https://github.com/CMB27/ModbusRTUMaster).

*/
//#include <ModbusRTUSlave.h>
//
//// Serial1 port RX1=19=RO, TX1=18=DI,to use for RS485 communication
////RE_DE = 0 RECIEVE
////RE_DE = 1 TRANSMIT
//
//const byte dePin = 17; // 16 не работает
//
////#define USE_SOFTWARE_SERIAL
//#ifdef USE_SOFTWARE_SERIAL
//const byte rxPin = 11;
//const byte txPin = 10;
//#include <SoftwareSerial.h>
//SoftwareSerial mySerial(rxPin, txPin);
//ModbusRTUSlave modbus(mySerial, dePin);  // serial port, driver enable pin for rs-485
//#else
//ModbusRTUSlave modbus(Serial1, dePin);  // serial port, driver enable pin for rs-485
//#endif
//
//bool coils[2];
//bool discreteInputs[2];
//uint16_t holdingRegisters[2];
//uint16_t inputRegisters[2];
//
//void setup() {
//#if defined ESP32
//  analogReadResolution(10);
//#endif
//  //
//  //  pinMode(potPins[0], INPUT);
//  //  pinMode(potPins[1], INPUT);
//  //  pinMode(buttonPins[0], INPUT_PULLUP);
//  //  pinMode(buttonPins[1], INPUT_PULLUP);
//  pinMode(13, OUTPUT);
//  pinMode(30, INPUT_PULLUP);
//  pinMode(31, OUTPUT);
//  digitalWrite(31, 0);
//  //  pinMode(ledPins[1], OUTPUT);
//  //  pinMode(ledPins[2], OUTPUT);
//  //  pinMode(ledPins[3], OUTPUT);
//
//  modbus.configureCoils(coils, 2);                       // bool array of coil values, number of coils
//  modbus.configureDiscreteInputs(discreteInputs, 2);     // bool array of discrete input values, number of discrete inputs
//  modbus.configureHoldingRegisters(holdingRegisters, 2); // unsigned 16 bit integer array of holding register values, number of holding registers
//  modbus.configureInputRegisters(inputRegisters, 2);     // unsigned 16 bit integer array of input register values, number of input registers
//
//  modbus.begin(10, 9600); //sleve ID 10
//}
//
//void loop() {
//  //  inputRegisters[0] = map(analogRead(potPins[0]), 0, 1023, 0, 255);
//  //  inputRegisters[1] = map(analogRead(potPins[1]), 0, 1023, 0, 255);
//  //  discreteInputs[0] = !digitalRead(buttonPins[0]);
//  //  discreteInputs[1] = !digitalRead(buttonPins[1]);
//
//  modbus.poll();
//
//  //  analogWrite(ledPins[0], holdingRegisters[0]);
//  //  analogWrite(ledPins[1], holdingRegisters[1]);
//  //  digitalWrite(ledPins[2], coils[0]);
//  digitalWrite(13, coils[1]); // отсчет с нулевой катушки.
//  if (!digitalRead(30)) {
//    delay(20);
//    if (!digitalRead(30)) {
//      while (!digitalRead(30)) delay(40);
//      coils[1] = !coils[1];
//    }
//  }
//}//loop



//################## MODBUS float passthrough ##################//
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
