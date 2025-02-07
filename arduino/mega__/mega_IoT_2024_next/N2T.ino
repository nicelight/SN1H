
// обработка нажатий клавиш N2.S1 ( у входа в гостевую, возле планшета)
// функция по тройному нажатию меняет состояния свечений на 1, 2, 1+2
// по одинарному нажатию включает\выключает свет
// по долгому удержанию, выключает свет и посылает сигнал - темноты
void handleN2T() {

  N2_TRACK_but.tick();
  EE_N2_tracks.tick();

  // первое срабатывание. если выключен свет, включим и установим флаг о включении
  if (N2_TRACK_but.isPress()) {
    if (!N2_tracks.state) {
      N2_tracks.state = 1; // включаем
      N2_tracks.rightNowOn = 1;// возводим флаг, что произошло мгновенное включение
      update_N2_Track(); // зажжем нужные лампы
      Serial.println("\n N2_TRACK_but Press");
      // обновить модбас
    }
  }

  // одиночный клик. если флаг о включенном свете возведен, убираем его и ничего не делаем,
  //  иначе убираем флаг и выключаем свет
  if (N2_TRACK_but.isSingle())
  {
    if (N2_tracks.rightNowOn) { // если мгновенно включен свет
      N2_tracks.rightNowOn = 0; // убираем флаг мгновенного нажатия
    } else {
      N2_tracks.state = 0; // выключаем
      update_N2_Track();
      Serial.println("/nN2_TRACK_but Single");
    }
  }//N2_tracks.isSingle

  // тройной клик. меняем состояние светильников SPOTS (не TRACK)  на 1, 2, 1+2.
  if (N2_TRACK_but.isTriple())
  {
    N2_tracks.rightNowOn = 0; // убираем флаг мгновенного нажатия
    // циклично меняем режимы работы 1..3 для спотов, не для ламп
    switch (N2_spots.mode)
    {
      case 0:
        N2_spots.mode = 1;
        N2_spots.lamp1 = OFF;
        N2_spots.lamp2 = ON;
        break;
      case 1:
        N2_spots.mode = 2;
        N2_spots.lamp1 = ON;
        N2_spots.lamp2 = ON;
        break;
      case 2:
        N2_spots.mode = 0;
        N2_spots.lamp1 = ON;
        N2_spots.lamp2 = OFF;
        break;
    }
    update_N2_Lamps();     // включаем в зависимости от прошлого запомненного режима
    Serial.println("\nN2_S1_but Double\n");
    EE_N2_spots.update();                    // стараемся не вызывать часто эти данные
  }

  // удержание. если флаг о включении возведен(т.е. он был выключен) включим весь свет в комнате,
  // иначе(если свет и так включен) выключаем весь свет в комнате, и даже тот за который не отвечаем
  if (N2_TRACK_but.isHolded()) {
    if (N2_tracks.rightNowOn ) {
      N2_tracks.rightNowOn = 0; // убираем флаг мгновенного нажатия
    N2_tracks.state = 1;
    N2_spots.state = 1;
    update_N2_Lamps();
    delay(50);
    update_N2_Track();
    }
    else {
    //      тушим весь свет и отправляем режим ночь
    N2_tracks.state = 0;
    N2_spots.state = 0;
    update_N2_Lamps();
    delay(50);
    update_N2_Track();
    Serial.print("\n\n\t\tN2_TRACK_but  NIGHT MODE ON\n\n");// TODO отправка режима ночь !!!
    }
  }

  if (N2_TRACK_but.hasClicks())
  {
    N2_tracks.rightNowOn = 0; // убираем флаг мгновенного нажатия
    Serial.print("N2_TRACK_but multi Clicks: ");
    Serial.println(N2_TRACK_but.getClicks());
    // проверка на наличие нажатий
  }
  // если кнопка была удержана и хотим подсчитать время удержания
  //    if (button.isStep()) {
  //    value++;                                            // увеличивать/уменьшать переменную value с шагом и интервалом
  //    Serial.println(value);                              // для примера выведем в порт
  //  }
}//handleN2_T

void update_N2_Track() {
  if (N2_tracks.state) {
    digitalWrite(N2_TRACK, N2_tracks.lamp1);
  }
  else
  {
    digitalWrite(N2_TRACK, OFF);
  }
}//update_N2_Track
