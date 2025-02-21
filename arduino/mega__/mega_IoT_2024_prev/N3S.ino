
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
  if (N3_S1_but.isHolded()){
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
    if (!fanN3state) fanN3state = 1; // если вытяжка выключена, включим ее
    else fanN3state = 6; // иначе выключаем вытяжку
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
}//handleN3_s1

void update_N3_Lamps() {
  if (N3_spots.state) {
    digitalWrite(N3_SP, N3_spots.lamp1);
    digitalWrite(N3_LED, N3_spots.lamp2);
  }
  else
  {
    digitalWrite(N3_SP, OFF);
    digitalWrite(N3_LED, OFF);
  }
}//update_N3_Lamps


void fanN3() {
  switch (fanN3state) {
    case 0:
      each10minFanN3.rst();
      break;
    // запуск на 10 минут
    case 1:
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


void pirN3() {
    statePIR3 = digitalRead(N3_SENS_PIR);
  if (each100msPirN3.ready()) { // каждых 100 мс
    if (statePIR3 && !N3_spots.state) { // сработал датчик и свет не горел
      digitalWrite(N3_LED, ON);
      if (!startPirLightN3) {
        startPirLightN3 = 1;
      }
    }
    //    Serial.print("\n\t");
    //    Serial.print(millis() >> 10); // сколько ~секунд прошло
    //    Serial.print("\t\t\t\t pir 7 = ");
    //    Serial.println(statePIR7);

    // прошло время служения PIR N3 и состояния света от кнопки по прежнему выключенные
    // потушим принудительно
    if (each5MinForN3.ready()) {
      if (!N3_spots.state && startPirLightN3) { // вышел таймаут служения светом от сенсора и за это время не произошло включения с кнопки
        digitalWrite(N3_LED, OFF);
      }
      startPirLightN3 = 0;
    }

  }// each 100 ms
}//pirN3()
