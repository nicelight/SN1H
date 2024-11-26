#include "timer.h"
#include "led.h"

#include <GyverButton.h>
#include <ModbusRTUSlave.h>
#include <EEManager.h>

#define DEBUG

#define N1_SENS1_PIN A0 // первая кнопка у двери детской
#define N2_SENS1_PIN A1
#define N3_SENS1_PIN A2
#define N4_SENS1_PIN A10
#define BUTTON_5_PIN A4
#define BUTTON_6_PIN A5
#define BUTTON_7_PIN A6
#define BUTTON_8_PIN A7
#define BUTTON_9_PIN A8
#define BUTTON_10_PIN A9

#define N1_SPOT_1_PIN 2
#define N1_SPOT_2_PIN 2
#define N4_SPOT_1_PIN 45
#define N4_SPOT_2_PIN 46
#define LAMP_5_PIN 2
#define LAMP_6_PIN 2
#define LAMP_7_PIN 2
#define LAMP_8_PIN 2
#define LAMP_9_PIN 2
#define LAMP_10_PIN 2
#define LAMP_11_PIN 2
#define LAMP_12_PIN 2
#define LAMP_13_PIN 2
#define LAMP_14_PIN 2
#define LAMP_15_PIN 2
#define LAMP_16_PIN 2
#define LAMP_17_PIN 2
#define LAMP_18_PIN 2
#define LAMP_19_PIN 2
#define LAMP_20_PIN 2

#define ON 0 // релевключается нулем 
#define OFF 1

// по одинарному нажатию лампы включаются \ выключаются
// структура для работы ламп
struct LampPair
{
  bool lamp1;   // состояние первой лампы
  bool lamp2;   // второй лампы
  bool state;   // включены или выключены лампы на данный момент
  bool rightNowOn;  // маякует о том, что уже было включение ламп, чтобы повторно не включать
  uint8_t mode; // режим ( первая лампа включена, вторая включена или обе включены)
};

LampPair N1_spots = {OFF, OFF, 0, 0};
LampPair N2_spots = {OFF, OFF, 0, 0};
LampPair N3_spots = {OFF, OFF, 0, 0};
LampPair N4_spots = {OFF, OFF, 0, 0};
LampPair button5_struct = {OFF, OFF, 0, 0};
LampPair button6_struct = {OFF, OFF, 0, 0};
LampPair button7_struct = {OFF, OFF, 0, 0};
LampPair button8_struct = {OFF, OFF, 0, 0};
LampPair button9_struct = {OFF, OFF, 0, 0};
LampPair button10_struct = {OFF, OFF, 0, 0};

GButton N1_S1_but(N1_SENS1_PIN);
GButton N2_S1_but(N2_SENS1_PIN);
GButton N3_S1_but(N3_SENS1_PIN);
GButton N4_S1_but(N4_SENS1_PIN);
GButton button5(BUTTON_5_PIN);
GButton button6(BUTTON_6_PIN);
GButton button7(BUTTON_7_PIN);
GButton button8(BUTTON_8_PIN);
GButton button9(BUTTON_9_PIN);
GButton button10(BUTTON_10_PIN);

// создаем в памяти ЕЕПРОМ блоки для запоминания структур света
EEManager EE_N1_spots(N1_spots);
EEManager EE_N2_spots(N2_spots);
EEManager EE_N3_spots(N3_spots);
EEManager EE_N4_spots(N4_spots);

//ModbusRTUSlave modbus(Serial2); // было Serial2 но выглядит что это ошибка 
ModbusRTUSlave modbus(Serial1);


bool coils[20];
uint16_t holdingRegisters[15];

LED builtinLed(13, 3000, 3, 300, 100); //каждые 3000 милисек мигаем 3 раза каждых 300 мс, время горения 100 мсек
// Timer after10sec(10000);

void setup()
{

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

  // ������������� ������
  N1_S1_but.setDebounce(10);
  N2_S1_but.setDebounce(10);
  N3_S1_but.setDebounce(10);
  N4_S1_but.setDebounce(10);
  button5.setDebounce(10);
  button6.setDebounce(10);
  button7.setDebounce(10);
  button8.setDebounce(10);
  button9.setDebounce(10);
  button10.setDebounce(10);

  // инициализация Modbus
  modbus.configureCoils(coils, 20);
  modbus.configureHoldingRegisters(holdingRegisters, 15); // unsigned 16 bit integer array of holding register values, number of holding registers
  modbus.begin(99, 9600); // адрес устройства 99

  // EEManager
  EE_N1_spots.begin(16, 'a');
  EE_N2_spots.begin(EE_N1_spots.nextAddr(), 'a'); // следующий блок памяти по адресу за предыдущим
  EE_N3_spots.begin(EE_N2_spots.nextAddr(), 'a');//следующий блок памяти по адресу за предыдущим
  EE_N4_spots.begin(EE_N3_spots.nextAddr(), 'a');//следующий блок памяти по адресу за предыдущим
  updateLamps(); // включим нужные лампы

  // возможно не надо
  logButtonPress(N1_spots);
  logButtonPress(N2_spots);
  logButtonPress(N3_spots);
  logButtonPress(N4_spots);

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
  digitalWrite(39 , 0);// N2 SP
  delay(100);
  digitalWrite(40 , 0);// N1 S1
  delay(100);
  digitalWrite(41 , 0);// N1 S2
  delay(100);
  digitalWrite(5 , 0);// N5 S1
  delay(100);
  digitalWrite(4 , 0);// N5 S2
  delay(100);
}//setup

void loop()
{
  builtinLed.tick();
  //builtinLed.setPeriod(3000, 1, 800, 800);
  // опрос кнопок
  N1_S1_but.tick();
  N2_S1_but.tick();
  N3_S1_but.tick();
  N4_S1_but.tick();
  button5.tick();
  button6.tick();
  button7.tick();
  button8.tick();
  button9.tick();
  button10.tick();
  // обновление энергонезависимой памяти
  EE_N1_spots.tick();
  EE_N2_spots.tick();
  EE_N3_spots.tick();
  EE_N4_spots.tick();

  handleN4S1(); // функция обработчик для кнопки 1 в комнате 4

  //  handleTwoSpotsButtons(N1_vhod_but, N1_spots);
  //  handleButtonPress(N2_vhod_and_kabinet_but, N2_spots);
  // handleButtonPress(button3, button3_struct);
  // handleButtonPress(button4, button4_struct);
  // handleButtonPress(button5, button5_struct);
  // handleButtonPress(button6, button6_struct);
  // handleButtonPress(button7, button7_struct);
  // handleButtonPress(button8, button8_struct);
  // handleButtonPress(button9, button9_struct);
  // handleButtonPress(button10, button10_struct);

  //  Modbus
  modbus.poll();
  makeCOM(); // отладка
}//loop


void updateLamps()
{
  if (N1_spots.state) {
    digitalWrite(N1_SPOT_1_PIN, N1_spots.lamp1);
    digitalWrite(N1_SPOT_2_PIN, N1_spots.lamp2);
  } else
  {
    digitalWrite(N1_SPOT_1_PIN, OFF);
    digitalWrite(N1_SPOT_2_PIN, OFF);
  }

  if (N4_spots.state) {
    digitalWrite(N4_SPOT_1_PIN, N4_spots.lamp1);
    digitalWrite(N4_SPOT_2_PIN, N4_spots.lamp2);
  }
  else
  {
    digitalWrite(N4_SPOT_1_PIN, OFF);
    digitalWrite(N4_SPOT_2_PIN, OFF);
  }
}// updateLamps()


// доработать
void logButtonPress(const LampPair &lampPair)
{
  Serial.print(" L1: ");
  Serial.print(lampPair.lamp1 ? "OFF" : "ON");
  Serial.print(",\t L2: ");
  Serial.print(lampPair.lamp2 ? "OFF" : "ON");
  Serial.print(",\t Mode: ");
  Serial.println(lampPair.mode);
}
