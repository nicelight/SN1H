
//    Modbus slave example.
//    https://github.com/yaacov/ArduinoModbusSlave
//тут пример использования модбас с homeassistant 
//https://www.instructables.com/Arduino-for-a-Wired-Home-Assistant-Network/

#include <ModbusSlave.h>

#define SLAVE_ID 10           // The Modbus slave ID, change to the ID you want to use.
#define RS485_CTRL_PIN 16     // Change to the pin the RE/DE pin of the RS485 controller is connected to.
#define SERIAL_BAUDRATE 9600 // Change to the baudrate you want to use for Modbus communication.
#define SERIAL_PORT Serial1   // Serial1 port RX1=19=RO, TX1=18=DI,to use for RS485 communication

// Modbus object declaration
Modbus slave(SERIAL_PORT, SLAVE_ID, RS485_CTRL_PIN);
bool coils[2];
bool discreteInputs[2];
uint16_t holdingRegisters[2];
uint16_t inputRegisters[2];

// Modbus handler functions
// The handler functions must return an uint8_t and take the following parameters:
//     uint8_t  fc - function code
//     uint16_t address - first register/coil address
//     uint16_t length/status - length of data / coil status

// Handle the function codes Read Input Status (FC=01/02) and write boolack the values from the digital pins (input status).
// coils, DiscreteInputs
uint8_t readDigitalReg(uint8_t fc, uint16_t address, uint16_t length)
{
  // Read the digital inputs.
  for (int i = 0; i < length; i++)
  {
    // Write the state of the digital pin to the response buffer.
    // slave.writeCoilToBuffer(i, digitalRead(digital_pins[address + i]));
    Serial.print("give: =");
    Serial.print(coils[address + i]);
    Serial.print("= of Coil/DiscreteInp #");
    Serial.print(address + i);
//    slave.writeCoilToBuffer(i, coils[address + i]);
  }

  return STATUS_OK;
}

// Handle the function code Read Holding Reg (FC=03) and write back the values from the EEPROM (holding registers).
uint8_t readHoldingReg(uint8_t fc, uint16_t address, uint16_t length)
{
  // Read the requested EEPROM registers.
  for (int i = 0; i < length; i++)
  {
    //    uint8_t value;
    // Read a value from the EEPROM.
    //          EEPROM.get((address + i), value);
    //          slave.writeRegisterToBuffer(i, value);
    byte rnd = random(0, 255);
    Serial.print("give: =");
    Serial.print(rnd);
    Serial.print("= of HoldingReg#");
    Serial.print(address + i);
    // Write the state of the digital pin to the response buffer.
    slave.writeRegisterToBuffer(i, rnd); // отдаем на чтение мастеру
  }

  return STATUS_OK;
}

// Handle the function code Read Input Reg (FC=04) and write back the values from the analog input pins (input registers).
uint8_t readInputReg(uint8_t fc, uint16_t address, uint16_t length)
{
  // Read the analog inputs
  for (int i = 0; i < length; i++)
  {
    // Write the state of the analog pin to the response buffer.
    // slave.writeRegisterToBuffer(i, analogRead(analog_pins[address + i]));
    byte rnd = random(0, 255);
    Serial.print("give: =");
    Serial.print(rnd);
    Serial.print("= of InputReg#");
    Serial.print(address + i);
    slave.writeRegisterToBuffer(i, rnd); // отдаем на чтение мастеру
  }

  return STATUS_OK;
}

// Handle the function codes Force Single Coil (FC=05) and Force Multiple Coils (FC=15) and set the digital output pins (coils).
uint8_t writeCoils(uint8_t fc, uint16_t address, uint16_t length)
{

  // Set the output pins to the given state.
  for (int i = 0; i < length; i++)
  {
    // Write the value in the input buffer to the digital pin.
    //        digitalWrite(digital_pins[address + i], slave.readCoilFromBuffer(i));
    Serial.print("recieved to coil #");
    Serial.print(address + i);
    Serial.print(" data: ");
    coils[i] = slave.readCoilFromBuffer(i);
//    FOR TEST
    Serial.print(coils[i]);
    digitalWrite(13, coils[i]);
  }
  Serial.println();
  return STATUS_OK;
}

// Handle the function codes Write Holding Register(s) (FC=06, FC=16) and write data to the eeprom.
uint8_t writeHoldingReg(uint8_t fc, uint16_t address, uint16_t length)
{
  // Write the received data to EEPROM.
  for (int i = 0; i < length; i++)
  {

    uint8_t value = slave.readRegisterFromBuffer(i);
    //      EEPROM.put(address + i, value);
    //      pinMode(digital_pins[address + i], value);
    Serial.print("recieved to HoldingReg #");
    Serial.print(address + i);
    Serial.print(" data:");
    Serial.print(value);
  }
  return STATUS_OK;
}

void setup()
{
Serial.begin(115200);
  // Register functions to call when a certain function code is received.
  slave.cbVector[CB_READ_COILS] = readDigitalReg;
  slave.cbVector[CB_READ_DISCRETE_INPUTS] = readDigitalReg;
  slave.cbVector[CB_WRITE_COILS] = writeCoils;
  slave.cbVector[CB_READ_INPUT_REGISTERS] = readInputReg;
  slave.cbVector[CB_READ_HOLDING_REGISTERS] = readHoldingReg;
  slave.cbVector[CB_WRITE_HOLDING_REGISTERS] = writeHoldingReg;

  // Set the serial port and slave to the given baudrate.
  SERIAL_PORT.begin(SERIAL_BAUDRATE);
  slave.begin(SERIAL_BAUDRATE);
  pinMode(13, OUTPUT);
}

void loop()
{
  // Listen for modbus requests on the serial port.
  // When a request is received it's going to get validated.
  // And if there is a function registered to the received function code, this function will be executed.
  slave.poll();


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

  }//each 1 sec
}


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
