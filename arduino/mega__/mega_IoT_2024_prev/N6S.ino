
// обработка нажатий клавиш N6.S1
// функция по тройному нажатию меняет состояния свечений на 1, 2, 1+2
// по одинарному нажатию включает\выключает свет
// по долгому удержанию, выключает свет и посылает сигнал - темноты
void handleN6S1() {
  /*
    документация:
    //  if (N6_S1_but.isPress()) Serial.println("Press");         // первое срабатывание. если выключен свет, включим и установим флаг о включении
    //  if (N6_S1_but.isSingle()) Serial.println("Single\n");       // одиночный клик. если флаг о включенном свете возведен, ничего не делаем, иначе убираем флаг и меняем состояние света
    //  if (N6_S1_but.isDouble()) Serial.println("Double\n");       // двойной клик. доп освещение
    //  if (N6_S1_but.isTriple()) Serial.println("Triple\n");       // тройной клик. меняем состояние светильников на 1, 2, 1+2.
    //  if (N6_S1_but.isHolded()) Serial.println("Holded\n");      0
    //struct LampTriple       // структура для работы ламп
    //{
    //  bool lamp1;   // состояние первой лампы
    //  bool lamp2;   // второй лампы
    //  bool state;   // включены или выключены лампы на данный момент
    //  bool rightNowOn;  // маякует о том, что уже было включение ламп, чтобы повторно не включать
    //  uint8_t mode; // режим ( первая лампа включена, вторая включена или обе включены)
    //};
  */
  N6_S1_but.tick();
  EE_N6_spots.tick();

  // первое срабатывание. если выключен свет, включим и установим флаг о включении
  //-- заменено N4 _S1_but  на N6_S1_but
  if (N6_S1_but.isPress()) {
    statePIR6 = 0; // обнулим датчик движения
    if (!N6_spots.state) {
      N6_spots.state = 1; // включаем
      N6_spots.rightNowOn = 1;// возводим флаг, что произошло мгновенное включение
      //--  замена update _N4_Lamps на update_N6_Lamps
      update_N6_Lamps(); // зажжем нужные лампы
      Serial.println("N6 S1 Press");
      // обновить модбас
    }
  }

  // одиночный клик. если флаг о включенном свете возведен, убираем его и ничего не делаем,
  //  иначе убираем флаг и выключаем свет
  if (N6_S1_but.isSingle())
  {
    if (N6_spots.rightNowOn) { // если мгновенно включен свет
      N6_spots.rightNowOn = 0; // убираем флаг мгновенного нажатия
    } else {
      N6_spots.state = 0; // выключаем
      update_N6_Lamps();
      Serial.println("N6_S1 Single\n");

    }
  }//N6_spots.isSingle


  // двойной клик. on\off альтернатива
  if (N6_S1_but.isDouble()) {
    N6_spots.rightNowOn = 0; // убираем флаг мгновенного нажатия
    //    if(N6_fan_state){
    //      N6_fan_state = 0;
    //      digitalWrite(N6_FAN, OFF);
    //    } else {
    //      N6_fan_state = 1;
    //      digitalWrite(N6_FAN, ON);
    //    }
  }

  // тройной клик. меняем состояние светильников на 1, 2, 1+2.
  if (N6_S1_but.isTriple())
  {
    N6_spots.rightNowOn = 0; // убираем флаг мгновенного нажатия
    // циклично меняем режимы работы 1..3
    switch (N6_spots.mode)
    {
      case 0:
        N6_spots.mode = 1;
        N6_spots.lamp1 = OFF;
        N6_spots.lamp2 = ON;
        break;
      case 1:
        N6_spots.mode = 2;
        N6_spots.lamp1 = ON;
        N6_spots.lamp2 = ON;
        break;
      case 2:
        N6_spots.mode = 0;
        N6_spots.lamp1 = ON;
        N6_spots.lamp2 = OFF;
        break;
    }
    update_N6_Lamps();     // включаем в зависимости от прошлого запомненного режима
    Serial.println("N6_S1 Double\n");
    EE_N6_spots.update();                    // стараемся не вызывать часто эти данные
    // EE_N2_spots.update();
    // EE_N3_spots.update();
  }

  // удержание. если флаг о включении возведен(т.е. он был выключен) включим весь свет в комнате,
  // иначе(если свет и так включен) выключаем весь свет в комнате, и даже тот за который не отвечаем
  if (N6_S1_but.isHolded())
  {
    // СЦЕНА ВХОД, заходим в темную ванну удерживая кнопку, хотим с вытяжкой
    if (N6_spots.rightNowOn) { //если свет включен после тьмы
      N6_spots.rightNowOn = 0;
    } //
    //    // СЦЕНА ВЫХОД выходим из комнаты с включенным светом,
    //    else {
    //      N6_spots.state = 0;
    //      update_N6_Lamps();
    //    Serial.print("\n\n\t\t N6 HOLDED for shutdown \n\n");// TODO отправка режима ночь !!!
    //    }
    // вытяжка
    if (!fanN6state) {
      fanN6state = 1; // если вытяжка выключена, включим ее
      ha[N6_FAN] = 1;
    }
    else {
      fanN6state = 6; // иначе выключаем вытяжку
      ha[N6_FAN] = 0;
    }
  }

  if (N6_S1_but.hasClicks())
  {
    N6_spots.rightNowOn = 0; // убираем флаг мгновенного нажатия
    Serial.print("N6_S1 multi Clicks: ");
    Serial.println(N6_S1_but.getClicks());
    // проверка на наличие нажатий
  }
  // если кнопка была удержана и хотим подсчитать время удержания
  //    if (button.isStep()) {
  //    value++;                                            // увеличивать/уменьшать переменную value с шагом и интервалом
  //    Serial.println(value);                              // для примера выведем в порт
  //  }
  update_N6_modbus();
}//handleN6S1()


// обработка ВХОДЯЩИХ по modbus
void update_N6_modbus() {
  //свет
  if ((N6_spots.state == 0) && (ha[N6_LIGHTS] == 1)) { //свет потушен а с ha пришло - включить
    N6_spots.state = 1;
    update_N6_Lamps();
  }
  else if ((N6_spots.state == 1) && (ha[N6_LIGHTS] == 0)) { //включенный свет надо потушить
    N6_spots.state = 0;
    update_N6_Lamps();
  }

  // вытяжка
  if ((ha[N6_FAN] == 1) && !fanN6state) { // если ha команда включить и вытяжка выключена, включим
    fanN6state = 1;
    fanN6();
  }
  else if ((ha[N6_FAN] == 0) && fanN6state) {
    fanN6state = 6; // иначе выключаем вытяжку
    ha[N6_FAN] = 0;
    fanN6();
  }
}// update_N6_modbus()


//обновление физ состояний ламп
void update_N6_Lamps() {
  if (N6_spots.state) {

    //#define N6_SENS1 34
    //#define N6_SENS_PIR 35
    //#define N6_SP 10
    //#define N6_LED 11

    ha[N6_LIGHTS] = 1;
    digitalWrite(N6_SP, N6_spots.lamp1);
    digitalWrite(N6_LED, N6_spots.lamp2);
  }
  else
  {
    ha[N6_LIGHTS] = 0;
    digitalWrite(N6_SP, OFF);
    digitalWrite(N6_LED, OFF);
  }
}//update_N6_Lamps()


void fanN6() {
  switch (fanN6state) {
    case 0:
      break;
    // запуск на 10 минут
    case 1:
      each10minFanN6.rst();
      digitalWrite(N6_FAN, ON);
      fanN6state = 2;
      break;
    //ожидаем когда время пройдет
    case 2:
      if (each10minFanN6.ready()) {
        fanN6state = 6;
      }
      break;
    // выключаем, уходим на ожидание
    case 6:
      delay(400);
      digitalWrite(N6_FAN, OFF);
      fanN6state = 0;
      break;
  }
}//fanN6()


// автоматическая подсветка от PIR
void pirN6() {
  static byte pirN6state = 0; // автомат отработки
  statePIR6 = digitalRead(N6_SENS_PIR);
  if (each100msPirN6.ready()) { // каждых 100 мс
    if (statePIR6 && !N6_spots.state) { // (сработал датчик и свет не горел) или (ha)
      pirN6state = 1; // запуск автомата с 10 минутным таймером
    }

    switch (pirN6state) {
      // ожидание
      case 0:
        break;
      // запуск на 10 минут
      case 1:
        each5MinForN6.rst();
        digitalWrite(N6_LED, ON);
        pirN6state = 2;
        break;
      //ожидаем когда время пройдет
      case 2:
        if (each10minFanN6.ready()) {
          pirN6state = 4;
        }
        break;
      case 4:
        // прошло время служения PIR
        if (each5MinForN6.ready()) {
          if (!N6_spots.state) { // за это время не произошло включения с кнопки
            digitalWrite(N6_LED, OFF);
          }
          pirN6state = 0;
        }
        break;
      // выключаем, уходим на ожидание
      case 6:
        delay(400);
        digitalWrite(N6_LED, OFF);
        pirN6state = 0;
        break;
    }//switch(pirN6state)
  }// each 100 ms
}//pirN6()
