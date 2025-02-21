/*
  ModbusRTUSlaveExample
  This example demonstrates how to setup and use the ModbusRTUSlave library (https://github.com/CMB27/ModbusRTUSlave).
  It is intended to be used with a second board running ModbusRTUMasterExample from the ModbusRTUMaster library (https://github.com/CMB27/ModbusRTUMaster).

*/



#include <ModbusRTUSlave.h>

// Serial1 port RX1=19=RO, TX1=18=DI,to use for RS485 communication
//RE_DE = 0 RECIEVE
//RE_DE = 1 TRANSMIT

const byte dePin = 25; // 16 не работает, 17 работал вроде
//#define USE_SOFTWARE_SERIAL
#ifdef USE_SOFTWARE_SERIAL
const byte rxPin = 11;
const byte txPin = 10;
#include <SoftwareSerial.h>
SoftwareSerial mySerial(rxPin, txPin);
ModbusRTUSlave modbus(mySerial, dePin);  // serial port, driver enable pin for rs-485
#else
ModbusRTUSlave modbus(Serial1, dePin);  // serial port, driver enable pin for rs-485
#endif

bool ha[55];
bool discreteInputs[2];
uint16_t holdingRegisters[2];
uint16_t inputRegisters[2];

void setup() {
  Serial.begin(115200);
  //
  //  pinMode(potPins[0], INPUT);
  //  pinMode(potPins[1], INPUT);
  //  pinMode(buttonPins[0], INPUT_PULLUP);
  //  pinMode(buttonPins[1], INPUT_PULLUP);
  pinMode(13, OUTPUT);
//  pinMode(30, INPUT_PULLUP);
//  pinMode(31, OUTPUT);
//  digitalWrite(31, 0);
  //  pinMode(ledPins[1], OUTPUT);
  //  pinMode(ledPins[2], OUTPUT);
  //  pinMode(ledPins[3], OUTPUT);

  modbus.configureCoils(ha, 55);                       // bool array of coil values, number of coils
  modbus.configureDiscreteInputs(discreteInputs, 2);     // bool array of discrete input values, number of discrete inputs
  modbus.configureHoldingRegisters(holdingRegisters, 2); // unsigned 16 bit integer array of holding register values, number of holding registers
  modbus.configureInputRegisters(inputRegisters, 2);     // unsigned 16 bit integer array of input register values, number of input registers

  modbus.begin(10, 9600); //sleve ID 10
}

void loop() {
  //  inputRegisters[0] = map(analogRead(potPins[0]), 0, 1023, 0, 255);
  //  inputRegisters[1] = map(analogRead(potPins[1]), 0, 1023, 0, 255);
  //  discreteInputs[0] = !digitalRead(buttonPins[0]);
  //  discreteInputs[1] = !digitalRead(buttonPins[1]);
static uint32_t prevcheckMs = 0;
  if(millis() - prevcheckMs  > 500){
    prevcheckMs = millis();
    Serial.println();
    Serial.print(millis() >> 10);
    Serial.print("\tha[0] = ");
    Serial.print(ha[0]);
    Serial.print("\tha[1] = ");
    Serial.print(ha[1]);
    Serial.print("\tha[2] = ");
    Serial.print(ha[2]);
    Serial.print("\tha[36] = ");
    Serial.print(ha[36]);
    Serial.print("\tha[37] = ");
    Serial.print(ha[37]);
    Serial.print("\tha[38] = ");
    Serial.print(ha[38]);
    Serial.println();
    
  }
  modbus.poll();

  
  //  analogWrite(ledPins[0], holdingRegisters[0]);
  //  analogWrite(ledPins[1], holdingRegisters[1]);
  //  digitalWrite(ledPins[2], coils[0]);
//  digitalWrite(13, coils[1]); // отсчет с нулевой катушки.
//  if (!digitalRead(30)) {
//    delay(20);
//    if (!digitalRead(30)) {
//      while (!digitalRead(30)) delay(40);
//      coils[1] = !coils[1];
//    }
//  }
}//loop
