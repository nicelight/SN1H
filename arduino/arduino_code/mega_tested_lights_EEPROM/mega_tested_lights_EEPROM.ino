#include <GyverButton.h>
#include <ModbusRTUSlave.h>
#include <EEManager.h>

#define N1_SENS1_DOOR_PIN A0 // первая кнопка у двери детской
#define N2_SENS1_DOOR_PIN A1
#define BUTTON_3_PIN A2
#define BUTTON_4_PIN A3
#define BUTTON_5_PIN A4
#define BUTTON_6_PIN A5
#define BUTTON_7_PIN A6
#define BUTTON_8_PIN A7
#define BUTTON_9_PIN A8
#define BUTTON_10_PIN A9

#define LAMP_1_PIN 3
#define LAMP_2_PIN 5
#define LAMP_3_PIN 7
#define LAMP_4_PIN 9
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
LampPair button3_struct = {OFF, OFF, 0, 0};
LampPair button4_struct = {OFF, OFF, 0, 0};
LampPair button5_struct = {OFF, OFF, 0, 0};
LampPair button6_struct = {OFF, OFF, 0, 0};
LampPair button7_struct = {OFF, OFF, 0, 0};
LampPair button8_struct = {OFF, OFF, 0, 0};
LampPair button9_struct = {OFF, OFF, 0, 0};
LampPair button10_struct = {OFF, OFF, 0, 0};

GButton N1_vhod_but(N1_SENS1_DOOR_PIN);
GButton N2_vhod_and_kabinet_but(N2_SENS1_DOOR_PIN);
GButton button3(BUTTON_3_PIN);
GButton button4(BUTTON_4_PIN);
GButton button5(BUTTON_5_PIN);
GButton button6(BUTTON_6_PIN);
GButton button7(BUTTON_7_PIN);
GButton button8(BUTTON_8_PIN);
GButton button9(BUTTON_9_PIN);
GButton button10(BUTTON_10_PIN);

// создаем в памяти ЕЕПРОМ блоки для запоминания структур света
EEManager EE_N1_spots(N1_spots);
EEManager EE_N2_spots(N2_spots);

ModbusRTUSlave modbus(Serial2);
bool coils[20];
uint16_t holdingRegisters[15];

void setup()
{
  Serial.begin(115200);
  Serial.println("\n\n");
  // заменить на наши пины выходы
  for (int i = 2; i <= 21; i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, 1);
  }

  // ������������� ������
  N1_vhod_but.setDebounce(10);
  N2_vhod_and_kabinet_but.setDebounce(10);
  button3.setDebounce(10);
  button4.setDebounce(10);
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
  EE_N1_spots.begin(16, 'c');
  EE_N2_spots.begin(EE_N1_spots.nextAddr(), 'c'); // следующий блок памяти по адресу за предыдущим
  //   EE_N3_spots.begin(EE_N2_spots.nextAddr(), 'a');//следующий блок памяти по адресу за предыдущим
  updateLamps(); // включим нужные лампы
  logButtonPress(N1_spots);
  logButtonPress(N2_spots);
}

void loop()
{
  // опрос кнопок
  N1_vhod_but.tick();
  N2_vhod_and_kabinet_but.tick();
  button3.tick();
  button4.tick();
  button5.tick();
  button6.tick();
  button7.tick();
  button8.tick();
  button9.tick();
  button10.tick();
  // обновление энергонезависимой памяти
  EE_N1_spots.tick();
  EE_N2_spots.tick();

  handleTwoSpotsButtons(N1_vhod_but, N1_spots);
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

  // отладка
  if (Serial.available())
  {
    String command = Serial.readStringUntil('\n');
    processAdminCommand(command);
  }
}

// обработка нажатий клавиш
// функция по двойному нажатию меняет состояния свечений на 1, 2, 1+2
// по одинарному нажатию включает\выключает свет
// по долгому удержанию, выключает свет и посылает сигнал - темноты
void handleTwoSpotsButtons(GButton &button, LampPair &lampPair)
{
  //  if (button.isPress()) Serial.println("Press");         // первое срабатывание. если выключен свет, включим и установим флаг о включении
  //  if (button.isSingle()) Serial.println("Single\n");       // одиночный клик. если флаг о включенном свете возведен, ничего не делаем, иначе убираем флаг и меняем состояние света
  //  if (button.isDouble()) Serial.println("Double\n");       // двойной клик. меняем состояние светильников на 1, 2, 1+2.
  //  if (button.isHolded()) Serial.println("Holded\n");      0

  //struct LampPair {
  //  bool lamp1;   // состояние первой лампы
  //  bool lamp2;   // второй лампы
  //  bool state;   // включены или выключены лампы на данный момент
  //  bool rightNowOn;  // маякует о том, что уже было включение ламп, чтобы повторно не включать
  //  uint8_t mode; // режим ( первая лампа включена, вторая включена или обе включены)
  //};

  // первое срабатывание. если выключен свет, включим и установим флаг о включении
  if (button.isPress()) {
    if (!lampPair.state) {
      lampPair.state = 1; // включаем
      lampPair.rightNowOn = 1;// возводим флаг, что произошло мгновенное включение
      updateLamps(); // зажжем нужные лампы
      Serial.println("Press");
      logButtonPress(lampPair);
    }
  }

  // одиночный клик. если флаг о включенном свете возведен, убираем его и ничего не делаем,
  //  иначе убираем флаг и выключаем свет
  if (button.isSingle())
  {
    if (lampPair.rightNowOn) { // если мгновенно включен свет
      lampPair.rightNowOn = 0; // ничего не делаем, убираем флаг
    } else {
      lampPair.state = 0; // выключаем
      updateLamps();
      Serial.println("Single\n");
      logButtonPress(lampPair);

    }
  }//button.isSingle

  //  updateLamps();
  //  logButtonPress(lampPair);

  // двойной клик. меняем состояние светильников на 1, 2, 1+2.
  if (button.isDouble())
  {
    lampPair.rightNowOn = 0; // флаг сбрасываем ( за ним приходится следить из каждого вызова кнопок
    // циклично меняем режимы работы 1..3
    switch (lampPair.mode)
    {
      case 0:
        lampPair.mode = 1;
        lampPair.lamp1 = OFF;
        lampPair.lamp2 = ON;
        break;
      case 1:
        lampPair.mode = 2;
        lampPair.lamp1 = ON;
        lampPair.lamp2 = ON;
        break;
      case 2:
        lampPair.mode = 0;
        lampPair.lamp1 = ON;
        lampPair.lamp2 = OFF;
        break;
    }
    updateLamps();     // включаем в зависимости от прошлого запомненного режима
    Serial.println("Double\n");
    logButtonPress(lampPair);
    EE_N1_spots.update();                    // стараемся не вызывать часто эти данные
    // EE_N2_spots.update();
    // EE_N3_spots.update();
  }

  // удержание. если флаг о включении возведен(т.е. он был выключен) включим весь свет в комнате,
  // иначе(если свет и так включен) выключаем весь свет в комнате, и даже тот за который не отвечаем
  if (button.isHolded())
  {
    if (lampPair.rightNowOn) { // если мгновенно включен свет ( т.е. было темно)
      lampPair.rightNowOn = 0; // флаг сбрасываем
      //      включим его весь
      lampPair.lamp1 = ON;
      lampPair.lamp2 = ON;
    }
    else  // если же было светло
    {
      //      тушим весь свет и отправляем режим ночь
      lampPair.state = 0;
      Serial.print("\n\n\t\t NIGHT MODE ON\n\n");// TODO отправка режима ночь !!!
    }
    updateLamps();
    Serial.println("Holded\n");
    logButtonPress(lampPair);
  }

  if (button.hasClicks())
  {
    Serial.print("multi Clicks: ");
    Serial.println(button.getClicks());
    lampPair.rightNowOn = 0; // флаг сбрасываем
    // проверка на наличие нажатий
  }
  // если кнопка была удержана и хотим подсчитать время удержания
  //    if (button.isStep()) {
  //    value++;                                            // увеличивать/уменьшать переменную value с шагом и интервалом
  //    Serial.println(value);                              // для примера выведем в порт
  //  }
}//handleTwoSpotButtons

void updateLamps()
{
  if (N1_spots.state) {
    digitalWrite(LAMP_1_PIN, N1_spots.lamp1);
    digitalWrite(LAMP_2_PIN, N1_spots.lamp2);
  } else
  {
    digitalWrite(LAMP_1_PIN, OFF);
    digitalWrite(LAMP_2_PIN, OFF);
  }

  if (N2_spots.state) {
    digitalWrite(LAMP_3_PIN, N2_spots.lamp1);
    digitalWrite(LAMP_4_PIN, N2_spots.lamp2);
  }
  else
  {
    digitalWrite(LAMP_3_PIN, OFF);
    digitalWrite(LAMP_4_PIN, OFF);
  }
  // ���������� ��� ���� ��������...
  // ...
  digitalWrite(LAMP_19_PIN, button10_struct.lamp1);
  digitalWrite(LAMP_20_PIN, button10_struct.lamp2);
}

void logButtonPress(const LampPair &lampPair)
{
  Serial.print(" L1: ");
  Serial.print(lampPair.lamp1 ? "OFF" : "ON");
  Serial.print(",\t L2: ");
  Serial.print(lampPair.lamp2 ? "OFF" : "ON");
  Serial.print(",\t Mode: ");
  Serial.println(lampPair.mode);
}

void processAdminCommand(const String &command)
{
  if (command.startsWith("BUTTON_"))
  {
    int buttonNum = command.charAt(7) - '0'; // �������� ����� ������
    if (command.endsWith("=1"))
    {
    }
    else if (command.endsWith("=2"))
    {
    }
  }
}
