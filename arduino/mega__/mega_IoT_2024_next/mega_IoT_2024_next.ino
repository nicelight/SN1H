#include "timer.h"
#include "led.h"

#include <GyverButton.h>
#include <ModbusRTUSlave.h>
#include <EEManager.h>

#define DEBUG

// сенсоры
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
//                                   35 пин свободен
//#define N6_SENS_PIR 35 перекинул на другой зеленый пин.
#define N6_SENS_PIR 34

#define N7_SENS1 36
#define N7_SENS_PIR 37
#define N7_GATE A1 // на проводе обозначен как DOOR 2



//// экзекьюторы
#define N1_SP1 41
#define N1_SP2 40

#define N2_SP 39
#define N2_TRACK

#define N3_SP 43
#define N3_LED 44

#define N4_SP1 45
#define N4_SP2 46
#define N4_MEAL 49

#define N4_TR1 47
#define N4_TR2 48

#define N4_KITCH1 50
#define N4_KITCH2 51

#define N4_MUS 53
#define N4_ERM 52

#define N5_SP1 5
#define N5_SP2 4
#define N5_BED 3
#define N5_SHELF 2

#define N6_SP 10
#define N6_LED 11

#define N7_SP 12
#define N7_LED 20
#define ON 0 // релевключается нулем 
#define OFF 1

GButton N1_S1_but(N1_SENS1);

GButton N2_TRACK_but(N2_SENS_TRACK);
GButton N2_S1_but(N2_SENS_SPOT);

GButton N3_S1_but(N3_SENS1);

GButton N4_S_TRACK_but(N4_SENS_TRACK);
GButton N4_S1_but(N4_SENS_PLANSHET);
GButton N4_S_KITCHEN_but(N4_SENS_KITCHEN);
GButton N4_S_KITH_TRACK_but(N4_SENS_KITCHEN_TRACK);
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
LampTriple N1_spots = {ON, ON, OFF, 0, 0}; // I, II, III лампа, state, mode
LampTriple N2_spots = {ON, ON, OFF, 0, 0};
LampTriple N3_spots = {ON, ON, OFF, 0, 0};
LampTriple N4_spots = {ON, ON, OFF, 0, 0};
LampTriple N4_museums = {ON, ON, OFF, 0, 0};
LampTriple N4_kitchen = {ON, ON, OFF, 0, 0};
LampTriple N4_tracks = {ON, ON, OFF, 0, 0};
LampTriple N5_spots = {ON, ON, OFF, 0, 0};
LampTriple N6_spots = {ON, ON, OFF, 0, 0};
LampTriple N7_spots = {ON, ON, OFF, 0, 0};
// создаем в памяти ЕЕПРОМ блоки для запоминания структур света
EEManager EE_N1_spots(N1_spots);
EEManager EE_N2_spots(N2_spots);
EEManager EE_N3_spots(N3_spots);
EEManager EE_N4_spots(N4_spots);
EEManager EE_N4_museums(N4_museums);
EEManager EE_N4_kitchen(N4_kitchen);
EEManager EE_N4_tracks(N4_tracks);
EEManager EE_N5_spots(N5_spots);
EEManager EE_N6_spots(N6_spots);
EEManager EE_N7_spots(N7_spots);

//ModbusRTUSlave modbus(Serial2); // было Serial2 но выглядит что это ошибка
ModbusRTUSlave modbus(Serial1);
bool coils[20];
uint16_t holdingRegisters[15];

LED builtinLed(13, 3000, 3, 300, 100); //каждые 3000 милисек мигаем 3 раза каждых 300 мс, время горения 100 мсек
// Timer after10sec(10000);


//////////       T E S T S
Timer each100ms(100); // для отладки 100 мс
Timer each5secForN3(5000); // отладка включим свет на 5 сек
Timer each5secForN4(5000); // отладка включим свет на 5 сек
Timer each5secForN6(5000); // отладка включим свет на 5 сек
Timer each5secForN7(5000); // отладка включим свет на 5 сек
bool starttestLightN3 = 0;
bool starttestLightN4 = 0;
bool starttestLightN6 = 0;
byte testPIR3 = 0, prevtestpir3 = 0; //для отладки pir слушаем
byte testPIR4 = 0, prevtestpir4 = 0; //для отладки pir слушаем
byte testPIR6 = 0, prevtestpir6 = 0; //для отладки pir слушаем


Timer each100msPirN7(100); // опрос датчика в корридоре каждыъ 100 мс
Timer each5MinForN7(300000); // по датчику включим свет в корридоре на 5 мин
bool startPirLightN7 = 0;
byte statePIR7 = 0, prevStatepir7 = 0; //для pir в корридоре
uint32_t prevMs = 0;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);
  Serial.println("\n\n");
  builtinLed.setPeriod(3000, 1, 800, 800);
  //  builtinLed.setPeriod(3000, 3, 300, 100);
  for (int i = 2; i <= 21; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, OFF);
  }
  for (int i = 22; i <= 31; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, OFF);
  }
  for (int i = 38; i <= 53; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, OFF);
  }
  for (int i = 32; i < 38; i++) pinMode(i, INPUT_PULLUP);
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
  N4_S_KITH_TRACK_but.setDebounce(20);
  N4_S_MAMA_but.setDebounce(20);
  N4_S_WINDOW_but.setDebounce(20);
  N5_S1_but.setDebounce(20);
  N6_S1_but.setDebounce(20);
  N7_S_GATE_but.setDebounce(20);

  // инициализация Modbus
  modbus.configureCoils(coils, 20);
  modbus.configureHoldingRegisters(holdingRegisters, 15); // unsigned 16 bit integer array of holding register values, number of holding registers
  modbus.begin(99, 9600); // адрес устройства 99

  // EEManager
  EE_N1_spots.begin(16, 'a'); // Spot1, Spot2
  EE_N2_spots.begin(EE_N1_spots.nextAddr(), 'a'); // следующий блок памяти по адресу за предыдущим. Spot, 12v, track
  EE_N3_spots.begin(EE_N2_spots.nextAddr(), 'a');//следующий блок памяти по адресу за предыдущим. Spot, 12v
  EE_N4_spots.begin(EE_N3_spots.nextAddr(), 'a');//следующий блок памяти по адресу за предыдущим. Spot1, Spot2, Meal
  EE_N4_museums.begin(EE_N4_spots.nextAddr(), 'a');//следующий блок памяти по адресу за предыдущим. Ermitage, Museum
  EE_N4_kitchen.begin(EE_N4_museums.nextAddr(), 'a');//следующий блок памяти по адресу за предыдущим. Kitchen1, Kitchen2
  EE_N4_tracks.begin(EE_N4_kitchen.nextAddr(), 'a');//следующий блок памяти по адресу за предыдущим. Track1, Track2
  EE_N5_spots.begin(EE_N4_tracks.nextAddr(), 'a');//следующий блок памяти по адресу за предыдущим. Spot1, Spot2, bed
  EE_N6_spots.begin(EE_N5_spots.nextAddr(), 'a');//следующий блок памяти по адресу за предыдущим. Spot, 12v
  EE_N7_spots.begin(EE_N6_spots.nextAddr(), 'a');//следующий блок памяти по адресу за предыдущим. Spot, 12v

  // плавный тестовый запуск всего света при перезагрузке
  digitalWrite(40 , 0);// N1 S1
  delay(100);
  digitalWrite(41 , 0);// N1 S2
  delay(100);
  digitalWrite(39 , 0);// N2 SP
  delay(100);
  digitalWrite(43 , 0);// N3 SP
  delay(100);

  digitalWrite(45, 0);// N4 SP1
  delay(100);
  digitalWrite(46, 0);// N4 SP2
  delay(100);
  //  digitalWrite(47, 1); // N4 T1
  //  delay(100);
  //  digitalWrite(48, 1);// N4 T2
  //  delay(100);
  //  digitalWrite(38, 1);// N2 T
  //  delay(100);
  digitalWrite(5 , 0);// N5 S1
  delay(100);
  digitalWrite(4 , 0);// N5 S2
  delay(100);
  digitalWrite(10 , 0);// N6 SP
  delay(100);
  digitalWrite(12 , 0);// N7 SP
  delay(300);
  updateAllLights();
}//setup




void loop()
{
  builtinLed.tick();
  //builtinLed.setPeriod(3000, 1, 800, 800);
  // опрос кнопок
  N4_S_KITCHEN_but.tick();
  N4_S_KITH_TRACK_but.tick();
  N4_S_MAMA_but.tick();
  N4_S_WINDOW_but.tick();

  // обновление энергонезависимой памяти
  EE_N3_spots.tick();

  handleN1S1();
  handleN2S1();
  handleN2T();
  handleN3S();
  handleN4S1(); // функция обработчик для кнопки 1 в комнате 4
  handleN4TR();
  handleN5S1();
  handleN6S1();
  handleN7S1();
  pirN7();
  //  Modbus
  //    modbus.poll();

  //  makeCOM(); // ручное управление пинами, не надо уже
  //  testPirs();
  checkLoopTIme(10);   //если луп  дольше 10 милисек, печатаемся
}//loop

// пример вызова функции со структурой в аргументе
//  logButtonPress(N4_spots);
//// пример передачи в функцию переменной по адресу из структуры
//void logButtonPress(const LampTriple &lamps)
//{
//  Serial.print(" L1: ");
//  Serial.print(lamps.lamp1 ? "OFF" : "ON");
//  Serial.print(",\t L2: ");
//  Serial.print(lamps.lamp2 ? "OFF" : "ON");
//  Serial.print(",\t Mode: ");
//  Serial.println(lamps.mode);
//}
