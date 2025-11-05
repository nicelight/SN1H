#define DEBUG

//////////////////////////////////// сенсоры //////////////////////////////////
#define N1_SENS1 A0 // первая кнопка у двери детской,дублируется у кровати

#define N2_SENS_TRACK A5
#define N2_SENS_SPOT A6

#define N3_SENS1 A7
#define N3_SENS_PIR A8

#define N4_SENS_TRACK A15
#define N4_SENS_PLANSHET A10
#define N4_SENS_KITCHEN_TRACK A12 // две кнопки на кухонной зоне, на одном проводе
#define N4_SENS_KITCHEN A13
#define N4_SENS_MAMA A11
#define N4_SENS_WINDOW A14
#define N4_SENS_PIR 32

#define N5_SENS1 A2 //  у двери, дублируется у кровати

#define N6_SENS1 33
//#define N6_SENS_PIR 35 пин свободен. PIR перекинул на зеленый пин 34
#define N6_SENS_PIR 34

#define N7_SENS1 36
#define N7_SENS_PIR 37
#define N7_GATE A1 // на проводе обозначен как DOOR 2



////////////////////////////////// актуаторы //////////////////////////////////
//#define A 9
//#define B 8
//#define C 7
//#define D 6
//#define E 14
//#define F 15
//#define G 22
//#define H 23

#define N1_SP1 41
#define N1_SP2 40

#define N2_SP 39
#define N2_LED 42
#define N2_TRACK 26 // бело-оранжевый

#define N3_SP 43
#define N3_LED 44
#define N3_FAN 6

#define N4_SP1 45
#define N4_SP2 46
#define N4_MEAL 49

//#define N4_TR1 47 // старый на черных твердотелочках, сейчас свободен ( или сгоревший)
//#define N4_TR2 48 // старый на черных твердотелочках, сейчас свободен ( или сгоревший)
#define N4_TR1 27 // оранженвый
#define N4_TR2 28 // бело-зеленый

//#define ЗЕЛЕНЫЙ 29
//#define БЕЛОСИНИЙ 30
//#define СИНИЙ 31

// для modbus номера коилов эквивалентные N1_lamps.state
#define N1_LIGHTS 31 
#define N2_LIGHTS 32
#define N2_TRACK_LIGHTS 14
#define N3_LIGHTS 33 
//#define N3_FAN 6 // уже определена
#define N4_LIGHTS 34 
#define N4_TRACK_LIGHTS 15
#define N4_KITCHEN_LIGHTS 16
#define N4_MUSEUMS_LIGHTS 17
#define N4_MEAL_LIGHTS 18
#define N5_LIGHTS 35 
#define N6_LIGHTS 36 
//#define N6_FAN 7 // уже определена
// !! НАЙТИ пин рекуператора 
#define RECUPERATOR 99 // реле включение рекуператора на максимум 
#define N7_LIGHTS 37



#define N4_KITCH1 50
#define N4_KITCH2 51

#define N4_MUS 53
#define N4_ERM 52

#define N5_SP1 5
#define N5_SP2 4
#define N5_BED 3
//#define N5_SHELF 2 // твердотелка сгорела, вздулась
#define N5_SHELF 38

#define N6_SP 10
#define N6_LED 11
#define N6_FAN 7

#define N7_SP 12
#define N7_LED 24
//#define свободный 25

#define ON 0 // релевключается нулем 
#define OFF 1



#include "timer.h"
#include "led.h"

//https://github.com/yaacov/ArduinoModbusSlave/blob/master/examples/minimum/minimum.ino
#include <ModbusRTUSlave.h>
// MODBUS PORT for RS485 communication
// Serial1 port RX1=19=RO, TX1=18=DI,to use for RS485 communication
//RE_DE = 0 RECIEVE
//RE_DE = 1 TRANSMIT
#define DE_PIN 25 // 16 не работает, 17 работал вроде
#define RS485_SERIAL Serial1
#define MODBUS_BAUD 9600
#define SLAVE_ID 10
// Serial1 port RX1=19=RO, TX1=18=DI,to use for RS485 communication
ModbusRTUSlave modbus(RS485_SERIAL, DE_PIN);  // (Serial1, DEpin)


const uint8_t numCoils = 54;
bool ha[numCoils];
//bool prevHa[numCoils];
bool discreteInputs[2];
uint16_t holdingRegisters[2];
uint16_t inputRegisters[2];
////// ModBus


#include <GyverButton.h>
#include <EEManager.h>

//#include "PCF8575.h"  // https://github.com/xreef/PCF8575_library
//// Set i2c address
//PCF8575 pcf8575(0x20);
//

LED builtinLed(13, 3000, 3, 300, 100); //каждые 3000 милисек мигаем 3 раза каждых 300 мс, время горения 100 мсек
// Timer after10sec(10000);



GButton N1_S1_but(N1_SENS1);

GButton N2_TRACK_but(N2_SENS_TRACK);
GButton N2_S1_but(N2_SENS_SPOT);

GButton N3_S1_but(N3_SENS1);

GButton N4_S_TRACK_but(N4_SENS_TRACK);
GButton N4_S1_but(N4_SENS_PLANSHET);
GButton N4_S_KITH_TRACK_but(N4_SENS_KITCHEN_TRACK);
GButton N4_S_KITCHEN_but(N4_SENS_KITCHEN);
GButton N4_S_MAMA_but(N4_SENS_MAMA);
GButton N4_S_WINDOW_but(N4_SENS_WINDOW);

GButton N5_S1_but(N5_SENS1);
GButton N6_S1_but(N6_SENS1);
GButton N7_S_GATE_but(N7_SENS1);


// по одинарному нажатию лампы включаются \ выключаются
// структура для работы ламп
struct LampTriple
{
  bool lamp1;   // состояние первой лампы
  bool lamp2;
  bool lamp3;
  bool state;   // включены или выключены лампы на данный момент
  bool rightNowOn;  // маякует о том, что уже было включение ламп, чтобы повторно не включать
  uint8_t mode; // режим ( первая лампа включена, вторая включена или обе включены)
};
LampTriple N1_spots = {ON, ON, OFF, 0, 0, 0}; // I, II, III лампа, state, rightNowOn, mode
LampTriple N2_spots = {ON, ON, OFF, 0, 0, 0};
LampTriple N2_tracks = {ON, ON, OFF, 0, 0, 0};
LampTriple N3_spots = {ON, ON, OFF, 0, 0, 0};
LampTriple N4_spots = {ON, ON, OFF, 0, 0, 0};
LampTriple N4_museums = {ON, ON, OFF, 0, 0, 0};
LampTriple N4_kitchen = {ON, ON, OFF, 0, 0, 0};
LampTriple N4_tracks = {ON, ON, OFF, 0, 0, 0};
LampTriple N5_spots = {ON, ON, OFF, 0, 0, 0};
LampTriple N6_spots = {ON, ON, OFF, 0, 0, 0};
LampTriple N7_spots = {ON, ON, OFF, 0, 0, 0};
// создаем в памяти ЕЕПРОМ блоки для запоминания структур света
EEManager EE_N1_spots(N1_spots);
EEManager EE_N2_spots(N2_spots);
EEManager EE_N2_tracks(N2_tracks);
EEManager EE_N3_spots(N3_spots);
EEManager EE_N4_spots(N4_spots);
EEManager EE_N4_museums(N4_museums);
EEManager EE_N4_kitchen(N4_kitchen);
EEManager EE_N4_tracks(N4_tracks);
EEManager EE_N5_spots(N5_spots);
EEManager EE_N6_spots(N6_spots);
EEManager EE_N7_spots(N7_spots);


//////////       T E S T S
Timer each100ms(100); // для отладки 100 мс
//bool starttestLightN3 = 0;
//bool starttestLightN4 = 0;
//bool starttestLightN6 = 0;
//byte testPIR3 = 0, prevtestpir3 = 0; //для отладки pir слушаем
//byte testPIR4 = 0, prevtestpir4 = 0; //для отладки pir слушаем
//byte testPIR6 = 0, prevtestpir6 = 0; //для отладки pir слушаем
//


// PIR датчик в корридоре
Timer each100msPirN7(100ul); // опрос датчика в корридоре каждыъ 100 мс
Timer each5MinForN7(300000ul); // по датчику включим свет в корридоре на 5 мин
bool startPirLightN7 = 0;
byte statePIR7 = 0, prevStatepir7 = 0; //для pir в корридоре

// PIR датчик в ванной
Timer each100msPirN6(100ul); // опрос датчика в корридоре каждыъ 100 мс
Timer each5MinForN6(300000ul); // по датчику включим свет в ванной на 5 мин
byte statePIR6 = 0, prevStatepir6 = 0; //для pir в корридоре

// PIR датчик в мастербасрум
Timer each100msPirN3(100ul); // опрос датчика в корридоре каждыъ 100 мс
Timer each5MinForN3(300000ul); // по датчику включим свет в ванной на 5 мин
byte statePIR3 = 0, prevStatepir3 = 0; //для pir в корридоре

Timer each10minFanN3(600000ul); // 10 минут вытяжке
byte fanN3state = 0;  // автомат
bool N3_fan_state = 0; // состояния вытяжек в туалетах

Timer each10minFanN6(600000ul); // 10 минут вытяжке
byte fanN6state = 0; // автомат
bool N6_fan_state = 0; // состояния вытяжек в туалетах

bool weAreAtHome = 1; // флаг о том что кто то есть дома, отрабатывает на корридорном выключателе

uint32_t prevMs = 0;


//
//
//


void updateAllLights() {
  update_N1_Lamps();
  delay(100);
  update_N2_Lamps();
  delay(100);
  update_N2_Track();
  delay(100);
  update_N3_Lamps();
  delay(100);
  update_N2_Track();
  delay(100);
  update_N4_museums();
  delay(100);
  update_N4_Lamps();
  delay(100);
  update_N4_kitchen();
  delay(100);
  update_N4_Tracks();
  delay(100);
  update_N5_Lamps();
  delay(100);
  update_N6_Lamps();
  delay(100);
  update_N7_Lamps();
}
//
//
//

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);
  Serial.println("\n\n");
  modbus.configureDiscreteInputs(discreteInputs, 2);
  modbus.configureHoldingRegisters(holdingRegisters, 2);
  modbus.configureInputRegisters(inputRegisters, 2);
  modbus.configureCoils(ha, numCoils);
  modbus.begin(SLAVE_ID, MODBUS_BAUD);



  for (int i = 2; i <= 21; i++) {
    if (i == DE_PIN) continue; //пропустим пин DE для modbus
    pinMode(i, OUTPUT);
    digitalWrite(i, OFF);
  }
  for (int i = 22; i <= 31; i++) {
    if (i == DE_PIN) continue; //пропустим пин DE для modbus
    pinMode(i, OUTPUT);
    digitalWrite(i, OFF);
  }
  for (int i = 38; i <= 53; i++) {
    if (i == DE_PIN) continue; //пропустим пин DE для modbus
    pinMode(i, OUTPUT);
    digitalWrite(i, OFF);
  }
  for (int i = 32; i < 38; i++) {
    if (i == DE_PIN) continue; //пропустим пин DE для modbus
    pinMode(i, INPUT_PULLUP);
  }
  for (int i = 54; i < 70; i++) pinMode(i, INPUT_PULLUP);

  //pinMode(N6_SENS_PIR, INPUT);
  //pinMode(N6_SENS_PIR, INPUT);

  // кнопки
  N1_S1_but.setDebounce(20);
  N2_TRACK_but.setDebounce(20);
  N2_S1_but.setDebounce(20);

  N3_S1_but.setDebounce(20);
  N4_S_TRACK_but.setDebounce(20);
  N4_S1_but.setDebounce(20);
  N4_S_KITCHEN_but.setDebounce(20);
  N4_S_KITH_TRACK_but.setDebounce(40);
  N4_S_MAMA_but.setDebounce(20);
  N4_S_WINDOW_but.setDebounce(20);
  N5_S1_but.setDebounce(20);
  N6_S1_but.setDebounce(20);
  N7_S_GATE_but.setDebounce(20);

  // EEManager
  EE_N1_spots.begin(16, 'b'); // Spot1, Spot2
  EE_N2_spots.begin(EE_N1_spots.nextAddr(), 'a'); // следующий блок памяти по адресу за предыдущим. Spot, 12v
  EE_N3_spots.begin(EE_N2_spots.nextAddr(), 'a');//следующий блок памяти по адресу за предыдущим. Spot, 12v
  EE_N4_spots.begin(EE_N3_spots.nextAddr(), 'a');//следующий блок памяти по адресу за предыдущим. Spot1, Spot2, Meal
  EE_N4_museums.begin(EE_N4_spots.nextAddr(), 'a');//следующий блок памяти по адресу за предыдущим. Ermitage, Museum
  EE_N4_kitchen.begin(EE_N4_museums.nextAddr(), 'a');//следующий блок памяти по адресу за предыдущим. Kitchen1, Kitchen2
  EE_N4_tracks.begin(EE_N4_kitchen.nextAddr(), 'a');//следующий блок памяти по адресу за предыдущим. Track1, Track2
  EE_N5_spots.begin(EE_N4_tracks.nextAddr(), 'b');//следующий блок памяти по адресу за предыдущим. Spot1, Spot2, bed
  EE_N6_spots.begin(EE_N5_spots.nextAddr(), 'a');//следующий блок памяти по адресу за предыдущим. Spot, 12v
  EE_N7_spots.begin(EE_N6_spots.nextAddr(), 'a');//следующий блок памяти по адресу за предыдущим. Spot, 12v
  EE_N2_tracks.begin(EE_N7_spots.nextAddr(), 'a'); // следующий блок памяти по адресу за предыдущим. Track

  // плавный тестовый запуск всего света при перезагрузке
  //  digitalWrite(40 , 0);// N1 S1
  //  delay(100);
  //  digitalWrite(41 , 0);// N1 S2
  //  delay(100);
  //  digitalWrite(39 , 0);// N2 SP
  //  delay(100);
  //  digitalWrite(43 , 0);// N3 SP
  //  delay(100);
  //
  //  digitalWrite(45, 0);// N4 SP1
  //  delay(100);
  //  digitalWrite(46, 0);// N4 SP2
  //  delay(100);
  //  //  digitalWrite(47, 1); // N4 T1
  //  //  delay(100);
  //  //  digitalWrite(48, 1);// N4 T2
  //  //  delay(100);
  //  //  digitalWrite(38, 1);// N2 T
  //  //  delay(100);
  //  digitalWrite(5 , 0);// N5 S1
  //  delay(100);
  //  digitalWrite(4 , 0);// N5 S2
  //  delay(100);
  //  digitalWrite(10 , 0);// N6 SP
  //  delay(100);
  //  digitalWrite(12 , 0);// N7 SP
  //  delay(300);

  //// тесты PCF8575
  //    for (int i = 0; i < 16; i++) {
  //    pcf8575.digitalWrite(i, ON);
  //    delay(500);
  //  }

  updateAllLights();

}//setup


uint32_t prevcheckMs = 0;

void loop() {
  
//  if (millis() - prevcheckMs  > 500) {
//    prevcheckMs = millis();
//    Serial.println();
//    Serial.print(millis() >> 10);
//    Serial.print("\tha[0] = ");
//    Serial.print(ha[0]);
//    Serial.print("\tha[1] = ");
//    Serial.print(ha[1]);
//    Serial.print("\tha[2] = ");
//    Serial.print(ha[2]);
//    Serial.print("\tha[36] = ");
//    Serial.print(ha[36]);
//    Serial.print("\tha[37] = ");
//    Serial.print(ha[37]);
//    Serial.print("\tha[38] = ");
//    Serial.print(ha[38]);
//    Serial.println();
//
//  }
  checkLoopTIme(10);   //если луп  дольше 10 милисек, печатаемся
  builtinLed.setPeriod(3000, 1, 800, 800);
  modbus.poll();

  // опрос кнопок
  handleN1S1(); // обработчики бизнес логики кнопок
  handleN2S1();
  handleN2T();
  handleN3S();
  handleN4S1(); // кнопка 1 в комнате 4
  handleN4K();
  handleN4K2();
  pirN3();
  fanN3();
  handleN4TR();
  handleN5S1();
  handleN6S1();
  pirN6();
  fanN6();
  handleN7S1();
  pirN7();
  recuperation();

  //  makeCOM(); // ручное управление пинами, не надо уже
}//loop
