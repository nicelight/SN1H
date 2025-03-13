
// обработка нажатий клавиш N3.S1 ( у входа в гостевую, возле планшета)
// функция по тройному нажатию меняет состояния свечений на 1, 2, 1+2
// по одинарному нажатию включает\выключает свет
// по долгому удержанию, выключает свет и посылает сигнал - темноты
void handleN3S() {
  /*
    документация:
    //  if (N3_S1_but.isPress()) Serial.println("Press");         // первое срабатывание. если выключен свет, включим и установим флаг о включении
    //  if (N3_S1_but.isSingle()) Serial.println("Single\n");       // одиночный клик. если флаг о включенном свете возведен, ничего не делаем, иначе убираем флаг и меняем состояние света
    //  if (N3_S1_but.isDouble()) Serial.println("Double\n");       // двойной клик. доп освещение
    //  if (N3_S1_but.isTriple()) Serial.println("Triple\n");       // тройной клик. меняем состояние светильников на 1, 2, 1+2.
    //  if (N3_S1_but.isHolded()) Serial.println("Holded\n");      0
    //struct LampTriple       // структура для работы ламп
    //{
    //  bool lamp1;   // состояние первой лампы
    //  bool lamp2;   // второй лампы
    //  bool state;   // включены или выключены лампы на данный момент
    //  bool rightNowOn;  // маякует о том, что уже было включение ламп, чтобы повторно не включать
    //  uint8_t mode; // режим ( первая лампа включена, вторая включена или обе включены)
    //};
  */
  N3_S1_but.tick();
  EE_N3_spots.tick();

  // первое срабатывание. если выключен свет, включим и установим флаг о включении
  if (N3_S1_but.isPress()) {
    statePIR3 = 0; // обнулим датчик движения
    if (!N3_spots.state) {
      N3_spots.state = 1; // включаем
      N3_spots.rightNowOn = 1;// возводим флаг, что произошло мгновенное включение
      update_N3_Lamps(); // зажжем нужные лампы
      Serial.println("\n N3_S1_but Press");
      // обновить модбас
    }
  }

  // одиночный клик. если флаг о включенном свете возведен, убираем его и ничего не делаем,
  //  иначе убираем флаг и выключаем свет
  if (N3_S1_but.isSingle())
  {
    if (N3_spots.rightNowOn) { // если мгновенно включен свет
      N3_spots.rightNowOn = 0; // убираем флаг мгновенного нажатия
    } else {
      N3_spots.state = 0; // выключаем
      update_N3_Lamps();
      Serial.println("/nN3_S1_but Single");
    }
  }//N3_spots.isSingle

  // двойной клик. on\off альтернатива
  if (N3_S1_but.isDouble()) {
    N3_spots.rightNowOn = 0; // убираем флаг мгновенного нажатия
    if (N3_fan_state) {
      N3_fan_state = 0;
      digitalWrite(N3_FAN, OFF);
    } else {
      N3_fan_state = 1;
      digitalWrite(N3_FAN, ON);
    }
  }
  // тройной клик. меняем состояние светильников на 1, 2, 1+2.
  if (N3_S1_but.isTriple())
  {
    N3_spots.rightNowOn = 0; // убираем флаг мгновенного нажатия
    // циклично меняем режимы работы 1..3
    switch (N3_spots.mode)
    {
      case 0:
        N3_spots.mode = 1;
        N3_spots.lamp1 = OFF;
        N3_spots.lamp2 = ON;
        break;
      case 1:
        N3_spots.mode = 2;
        N3_spots.lamp1 = ON;
        N3_spots.lamp2 = ON;
        break;
      case 2:
        N3_spots.mode = 0;
        N3_spots.lamp1 = ON;
        N3_spots.lamp2 = OFF;
        break;
    }
    update_N3_Lamps();     // включаем в зависимости от прошлого запомненного режима
    Serial.println("\nN3_S1_but Double\n");
    EE_N3_spots.update();                    // стараемся не вызывать часто эти данные
  }

  // удержание. если флаг о включении возведен(т.е. он был выключен) включим весь свет в комнате,
  // иначе(если свет и так включен) выключаем весь свет в комнате, и даже тот за который не отвечаем
  if (N3_S1_but.isHolded()) {
    // СЦЕНА ВХОД, заходим в темную ванну удерживая кнопку, хотим с вытяжкой
    if (N3_spots.rightNowOn) { //если свет включен после тьмы
      N3_spots.rightNowOn = 0;
    } //
    //    // СЦЕНА ВЫХОД выходим из комнаты с включенным светом,
    //    else {
    //      N3_spots.state = 0;
    //      update_N3_Lamps();
    //    Serial.print("\n\n\t\t N3 HOLDED for shutdown \n\n");// TODO отправка режима ночь !!!
    //    }
    // вытяжка
    // вытяжка
    if (!fanN3state) {
      fanN3state = 1; // если вытяжка выключена, включим ее
      ha[N3_FAN] = 1;
    }
    else {
      fanN3state = 6; // иначе выключаем вытяжку
      ha[N3_FAN] = 0;
    }
  }

  if (N3_S1_but.hasClicks())
  {
    N3_spots.rightNowOn = 0; // убираем флаг мгновенного нажатия
    Serial.print("N3_S1_but multi Clicks: ");
    Serial.println(N3_S1_but.getClicks());
    // проверка на наличие нажатий
  }
  // если кнопка была удержана и хотим подсчитать время удержания
  //    if (button.isStep()) {
  //    value++;                                            // увеличивать/уменьшать переменную value с шагом и интервалом
  //    Serial.println(value);                              // для примера выведем в порт
  //  }
  update_N3_modbus();
}//handleN3_s1


//обновление физ состояний ламп
void update_N3_Lamps() {
  if (N3_spots.state) {
    ha[N3_LIGHTS] = 1;
    digitalWrite(N3_SP, N3_spots.lamp1);
    digitalWrite(N3_LED, N3_spots.lamp2);
  }
  else
  {
    ha[N3_LIGHTS] = 0;
    digitalWrite(N3_SP, OFF);
    digitalWrite(N3_LED, OFF);
  }
}//update_N3_Lamps


// обработка ВХОДЯЩИХ по modbus
//N3_LIGHTS 33 
//N3_FAN 6 
void update_N3_modbus() {
  //свет
  //свет потушен а с ha пришло - включить
  if ((N3_spots.state == 0) && (ha[N3_LIGHTS] == 1)) { 
    N3_spots.state = 1;
    update_N3_Lamps();
  }
  //включенный свет надо потушить
  else if ((N3_spots.state == 1) && (ha[N3_LIGHTS] == 0)) { 
    N3_spots.state = 0;
    update_N3_Lamps();
  }

  // вытяжка
  // если ha команда включить и вытяжка выключена, включим
  if ((ha[N3_FAN] == 1) && !fanN3state) { 
    fanN3state = 1;
    fanN3();
  }
  // иначе выключаем вытяжку
  else if ((ha[N3_FAN] == 0) && fanN3state) {
    fanN3state = 6; 
    ha[N3_FAN] = 0;
    fanN3();
  }
}// update_N3_modbus()


void fanN3() {
  switch (fanN3state) {
    case 0:
      break;
    // запуск на 10 минут
    case 1:
      each10minFanN3.rst();
      digitalWrite(N3_FAN, ON);
      fanN3state = 2;
      break;
    //ожидаем когда время пройдет
    case 2:
      if (each10minFanN3.ready()) {
        fanN3state = 6;
      }
      break;
    // выключаем, уходим на ожидание
    case 6:
      delay(400);
      digitalWrite(N3_FAN, OFF);
      fanN3state = 0;
      break;

  }
}//fanN3()


// автоматическая подсветка от PIR
void pirN3() {
  static byte pirN3state = 0;
  statePIR6 = digitalRead(N3_SENS_PIR);
  if (each100msPirN3.ready()) { // каждых 100 мс
    if (statePIR6 && !N3_spots.state) { // (сработал датчик и свет не горел) или (ha)
      pirN3state = 1; // запуск автомата с 10 минутным таймером
    }

    switch (pirN3state) {
      // ожидание
      case 0:
        break;
      // запуск на 10 минут
      case 1:
        each5MinForN3.rst();
        digitalWrite(N3_LED, ON);
        pirN3state = 2;
        break;
      //ожидаем когда время пройдет
      case 2:
        if (each10minFanN3.ready()) {
          pirN3state = 4;
        }
        break;
      case 4:
        // прошло время служения PIR
        if (each5MinForN3.ready()) {
          if (!N3_spots.state) { // за это время не произошло включения с кнопки
            digitalWrite(N3_LED, OFF);
          }
          pirN3state = 0;
        }
        break;
      // выключаем, уходим на ожидание
      case 6:
        delay(400);
        digitalWrite(N3_LED, OFF);
        pirN3state = 0;
        break;
    }//switch(pirN3state)
  }// each 100 ms
}//pirN3()
