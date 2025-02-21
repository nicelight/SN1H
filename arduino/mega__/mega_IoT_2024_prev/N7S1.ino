
// обработка нажатий клавиш N7.S1 ( у входа в гостевую, возле планшета)
// функция по тройному нажатию меняет состояния свечений на 1, 2, 1+2
// по одинарному нажатию включает\выключает свет
// по долгому удержанию, выключает свет и посылает сигнал - темноты
void handleN7S1() {
  /*
    документация:
    //  if (N7_S_GATE_but.isPress()) Serial.println("Press");         // первое срабатывание. если выключен свет, включим и установим флаг о включении
    //  if (N7_S_GATE_but.isSingle()) Serial.println("Single\n");       // одиночный клик. если флаг о включенном свете возведен, ничего не делаем, иначе убираем флаг и меняем состояние света
    //  if (N7_S_GATE_but.isDouble()) Serial.println("Double\n");       // двойной клик. доп освещение
    //  if (N7_S_GATE_but.isTriple()) Serial.println("Triple\n");       // тройной клик. меняем состояние светильников на 1, 2, 1+2.
    //  if (N7_S_GATE_but.isHolded()) Serial.println("Holded\n");      0
    //struct LampTriple       // структура для работы ламп
    //{
    //  bool lamp1;   // состояние первой лампы
    //  bool lamp2;   // второй лампы
    //  bool state;   // включены или выключены лампы на данный момент
    //  bool rightNowOn;  // маякует о том, что уже было включение ламп, чтобы повторно не включать
    //  uint8_t mode; // режим ( первая лампа включена, вторая включена или обе включены)
    //};
  */
  N7_S_GATE_but.tick();
  EE_N7_spots.tick();


  // первое срабатывание. если выключен свет, включим и установим флаг о включении
  if (N7_S_GATE_but.isPress()) {
    if (!N7_spots.state) {
      N7_spots.state = 1; // включаем
      N7_spots.rightNowOn = 1;// возводим флаг, что произошло мгновенное включение
      update_N7_Lamps(); // зажжем нужные лампы
      Serial.println("\n N7_S_GATE_but Press");
      // обновить модбас
    }
  }

  // одиночный клик. если флаг о включенном свете возведен, убираем его и ничего не делаем,
  //  иначе убираем флаг и выключаем свет
  if (N7_S_GATE_but.isSingle())
  {
    if (N7_spots.rightNowOn) { // если мгновенно включен свет
      N7_spots.rightNowOn = 0; // убираем флаг мгновенного нажатия
    } else {
      N7_spots.state = 0; // выключаем
      update_N7_Lamps();
      Serial.println("/nN7_S_GATE_but Single");
    }
  }//N7_spots.isSingle

  // тройной клик. меняем состояние светильников на 1, 2, 1+2.
  if (N7_S_GATE_but.isTriple())
  {
    N7_spots.rightNowOn = 0; // убираем флаг мгновенного нажатия
    // циклично меняем режимы работы 1..3
    switch (N7_spots.mode)
    {
      case 0:
        N7_spots.mode = 1;
        N7_spots.lamp1 = OFF;
        N7_spots.lamp2 = ON;
        break;
      case 1:
        N7_spots.mode = 2;
        N7_spots.lamp1 = ON;
        N7_spots.lamp2 = ON;
        break;
      case 2:
        N7_spots.mode = 0;
        N7_spots.lamp1 = ON;
        N7_spots.lamp2 = OFF;
        break;
    }
    update_N7_Lamps();     // включаем в зависимости от прошлого запомненного режима
    Serial.println("\nN7_S_GATE_but Double\n");
    EE_N7_spots.update();                    // стараемся не вызывать часто эти данные
  }

  // удержание. если флаг о включении возведен(т.е. он был выключен) включим весь свет в комнате,
  // иначе(если свет и так включен) выключаем весь свет в комнате, и даже тот за который не отвечаем
  if (N7_S_GATE_but.isHolded()) {
    N7_spots.rightNowOn = 0; // убираем флаг мгновенного нажатия
    if (!weAreAtHome) { // если никого не было дома и пришли
      // СЦЕНА - Пришли домой. включаем свет во всей квартире кроме туалетов
      weAreAtHome = 1;
      N4_tracks.state = 1;
      N4_spots.state = 1;
      N4_museums.state = 1;
      N7_spots.state = 1;
      update_N4_museums();
      delay(50);
      update_N4_Lamps();
      delay(50);
      update_N4_Tracks();
      delay(50);
      update_N7_Lamps();
      Serial.print("\n\n\t\tN7_S_GATE_but Come home ON\n\n");// TODO отправка режима ночь !!!
    }
    else {
      // СЦЕНА - уходим из дому, тушим все.
      weAreAtHome = 0;
      N1_spots.state = 0;
      N2_spots.state = 0;
      N2_tracks.state = 0;
      N3_spots.state = 0;
      N4_spots.state = 0;
      N4_tracks.state = 0;
      N4_museums.state = 0;
      N5_spots.state = 0;
      N6_spots.state = 0;
      N7_spots.state = 0;
      update_N1_Lamps();
      delay(50);
      update_N2_Lamps();
      delay(50);
      update_N2_Track();
      delay(50);
      update_N3_Lamps();
      delay(50);
      update_N4_museums();
      delay(50);
      update_N4_Lamps();
      delay(50);
      update_N4_Tracks();
      delay(50);
      update_N5_Lamps();
      delay(50);
      update_N6_Lamps();
      delay(50);
      update_N7_Lamps();
      Serial.print("\n\n\t\tN7_S_GATE_but Go out. Bye... \n\n");// TODO отправка режима ночь !!!
    }
  }

  if (N7_S_GATE_but.hasClicks())
  {
    Serial.print("N7_S_GATE_but multi Clicks: ");
    Serial.println(N7_S_GATE_but.getClicks());
    // проверка на наличие нажатий
  }
  // если кнопка была удержана и хотим подсчитать время удержания
  //    if (button.isStep()) {
  //    value++;                                            // увеличивать/уменьшать переменную value с шагом и интервалом
  //    Serial.println(value);                              // для примера выведем в порт
  //  }
}//handleN7_s1


void update_N7_Lamps() {
  if (N7_spots.state) {
    digitalWrite(N7_SP, N7_spots.lamp1);
    digitalWrite(N7_LED, N7_spots.lamp2);
  }
  else
  {
    digitalWrite(N7_SP, OFF);
    digitalWrite(N7_LED, OFF);
  }
}//update_N7_Lamps
//
//
//


void pirN7() {
  statePIR7 = digitalRead(N7_SENS_PIR);
  if (each100msPirN7.ready()) { // каждых 100 мс
    if (statePIR7 && !N7_spots.state) { // сработал датчик и свет не горел
      digitalWrite(N7_SP, ON);
      if (!startPirLightN7) {
        startPirLightN7 = 1;
      }
    }
    //    Serial.print("\n\t");
    //    Serial.print(millis() >> 10); // сколько ~секунд прошло
    //    Serial.print("\t\t\t\t pir 7 = ");
    //    Serial.println(statePIR7);

    // прошло время служения PIR N7 и состояния света от кнопки по прежнему выключенные
    // потушим принудительно
    if (each5MinForN7.ready()) {
      if (!N7_spots.state && startPirLightN7) { // вышел таймаут служения светом от сенсора и за это время не произошло включения с кнопки
        digitalWrite(N7_SP, OFF);
      }
      startPirLightN7 = 0;
    }

  }// each 100 ms
}//pirN7()
