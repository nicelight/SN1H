///*
//  Copyright (c) 2021 Jakub Mandula
//
//  Example of using multiple PZEM modules together on one ModBUS.
//  ================================================================
//
//  First of all, use the PZEMChangeAddress example in order to assign
//  each individual PZEM module a unique custom address. This example
//  requires 2 PZEM modules with addresses 0x10 and 0x11.
//
//
//  Then for each PZEM module create a PZEM004Tv30 instance passing a custom address
//  to the address field.
//
//  The instances can either be stored as individual objects:
//
//  ```c
//  PZEM004Tv30 pzem0(&Serial2, 0x10);
//  PZEM004Tv30 pzem1(&Serial2, 0x11);
//  PZEM004Tv30 pzem2(&Serial2, 0x12);
//
//  pzem0.voltage();
//  pzem1.pf();
//  ```
//
//  Or in an array and addressed using the array index:
//
//  ```c
//  PZEM004Tv30 pzems[] = {
//    PZEM004Tv30(&Serial2, 0x10),
//    PZEM004Tv30(&Serial2, 0x11),
//    PZEM004Tv30(&Serial2, 0x12)};
//
//  pzems[0].voltage();
//  pzems[1].pf();
//  ```
//
//*/
//
//#include <PZEM004Tv30.h>
//
//#define CONSOLE_SERIAL Serial
//
////PZEM004Tv30 pzem(Serial2, PZEM_RX_PIN, PZEM_TX_PIN);
//PZEM004Tv30 pzem(Serial2);
//
//void setup() {
//  Serial.begin(115200);
//  CONSOLE_SERIAL.print("Custom Address:");
//  CONSOLE_SERIAL.println(pzem.readAddress(), HEX);
//
//}//setup
//
//
//void loop() {
//
//  static uint32_t tmr;
//  tmr = millis();
//
//  // считывает данные аж 60 мс
//  float voltage = pzem.voltage();
//  float current = pzem.current();
//  float energy = pzem.energy();
//  //  float power = pzem.power();
//  //  float frequency = pzem.frequency();
//  //  float pf = pzem.pf();
//  Serial.print("\n\n\t\t time for read PZEM:");
//  Serial.println(millis() - tmr);
//  // Check if the data is valid
//  if (isnan(voltage) || isnan(current) || isnan(energy)) {
//    CONSOLE_SERIAL.println("Error reading energy");
//  } else {
//
//    // Print the values to the Serial console
//    CONSOLE_SERIAL.print("Voltage: ");      CONSOLE_SERIAL.print(voltage);      CONSOLE_SERIAL.println("V");
//    CONSOLE_SERIAL.print("Current: ");      CONSOLE_SERIAL.print(current);      CONSOLE_SERIAL.println("A");
//    CONSOLE_SERIAL.print("Energy: ");       CONSOLE_SERIAL.print(energy, 3);     CONSOLE_SERIAL.println("kWh");
//    //    CONSOLE_SERIAL.print("Power: ");        CONSOLE_SERIAL.print(power);        CONSOLE_SERIAL.println("W");
//    //    CONSOLE_SERIAL.print("Frequency: ");    CONSOLE_SERIAL.print(frequency, 1); CONSOLE_SERIAL.println("Hz");
//    //    CONSOLE_SERIAL.print("PF: ");           CONSOLE_SERIAL.println(pf);
//
//  }
//  Serial.print("\n\n\t\t time for draw data from PZEM:");
//  Serial.println(millis() - tmr);
//  Serial.println();
//  delay(2000);
//}
