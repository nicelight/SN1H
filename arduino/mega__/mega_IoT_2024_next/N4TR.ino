
// обработка нажатий клавиш N4.S1 ( у входа в гостевую, возле планшета)
// функция по тройному нажатию меняет состояния свечений на 1, 2, 1+2
// по одинарному нажатию включает\выключает свет
// по долгому удержанию, выключает свет и посылает сигнал - темноты
void handleN4TR() {
  /*
    документация:
    //  if (N4_S_TRACK_but.isPress()) Serial.println("Press");         // первое срабатывание. если выключен свет, включим и установим флаг о включении
    //  if (N4_S_TRACK_but.isSingle()) Serial.println("Single\n");       // одиночный клик. если флаг о включенном свете возведен, ничего не делаем, иначе убираем флаг и меняем состояние света
    //  if (N4_S_TRACK_but.isDouble()) Serial.println("Double\n");       // двойной клик. доп освещение
    //  if (N4_S_TRACK_but.isTriple()) Serial.println("Triple\n");       // тройной клик. меняем состояние светильников на 1, 2, 1+2.
    //  if (N4_S_TRACK_but.isHolded()) Serial.println("Holded\n");      0
    //struct LampTriple       // структура для работы ламп
    //{
    //  bool lamp1;   // состояние первой лампы
    //  bool lamp2;   // второй лампы
    //  bool state;   // включены или выключены лампы на данный момент
    //  bool rightNowOn;  // маякует о том, что уже было включение ламп, чтобы повторно не включать
    //  uint8_t mode; // режим ( первая лампа включена, вторая включена или обе включены)
    //};
  */
  N4_S_TRACK_but.tick();
  EE_N4_tracks.tick();

  // первое срабатывание. если выключен свет, включим и установим флаг о включении
  if (N4_S_TRACK_but.isPress()) {
    //
    // П Е Р Е Д Е Л А Т Ь
    //
    //вместо N4_spots.state делаем N4_tracks.state
    //  а так же updateLamps
    //  а так же EE_N4_spots на EE_N4_tracks
    if (!N4_spots.state) {
      N4_spots.state = 1; // включаем
      N4_spots.rightNowOn = 1;// возводим флаг, что произошло мгновенное включение
      update_N4_Tracks(); // зажжем нужные лампы
      Serial.println("N4_S1 Press");
      // обновить модбас
    }
  }

  // одиночный клик. если флаг о включенном свете возведен, убираем его и ничего не делаем,
  //  иначе убираем флаг и выключаем свет
  if (N4_S_TRACK_but.isSingle())
  {
    if (N4_spots.rightNowOn) { // если мгновенно включен свет
      N4_spots.rightNowOn = 0; // ничего не делаем, убираем флаг
    } else {
      N4_spots.state = 0; // выключаем
      update_N4_Tracks();
      Serial.println("N4_S1 Single\n");

    }
  }//N4_spots.isSingle

  //  update_N4_Tracks();

  // тройной клик. меняем состояние светильников на 1, 2, 1+2.
  if (N4_S_TRACK_but.isTriple())
  {
    N4_spots.rightNowOn = 0; // флаг сбрасываем ( за ним приходится следить из каждого вызова кнопок
    // циклично меняем режимы работы 1..3
    switch (N4_spots.mode)
    {
      case 0:
        N4_spots.mode = 1;
        N4_spots.lamp1 = OFF;
        N4_spots.lamp2 = ON;
        break;
      case 1:
        N4_spots.mode = 2;
        N4_spots.lamp1 = ON;
        N4_spots.lamp2 = ON;
        break;
      case 2:
        N4_spots.mode = 0;
        N4_spots.lamp1 = ON;
        N4_spots.lamp2 = OFF;
        break;
    }
    update_N4_Tracks();     // включаем в зависимости от прошлого запомненного режима
    Serial.println("N4_track Double\n");
    EE_N4_spots.update();                    // стараемся не вызывать часто эти данные
  }

  // удержание. если флаг о включении возведен(т.е. он был выключен) включим весь свет в комнате,
  // иначе(если свет и так включен) выключаем весь свет в комнате, и даже тот за который не отвечаем
  if (N4_S_TRACK_but.isHolded())
  {
    //      тушим весь свет и отправляем режим ночь
    N4_spots.state = 0;
    Serial.print("\n\n\t\tN4 track  NIGHT MODE ON\n\n");// TODO отправка режима ночь !!!
    update_N4_Tracks();
  }

  if (N4_S_TRACK_but.hasClicks())
  {
    Serial.print("N4_track multi Clicks: ");
    Serial.println(N4_S_TRACK_but.getClicks());
    // проверка на наличие нажатий
  }
  // если кнопка была удержана и хотим подсчитать время удержания
  //    if (button.isStep()) {
  //    value++;                                            // увеличивать/уменьшать переменную value с шагом и интервалом
  //    Serial.println(value);                              // для примера выведем в порт
  //  }
}//handleN4_s1

void update_N4_Tracks() {
  if (N4_spots.state) {
    //
    // И С П Р А В И Т Ь N4_spots на N4_tracks
    //
    digitalWrite(N4_SP1, N4_spots.lamp1);
    digitalWrite(N4_SP2, N4_spots.lamp2);
  }
  else
  {
    digitalWrite(N4_SP1, OFF);
    digitalWrite(N4_SP2, OFF);
  }
}//update_N4_Tracks
