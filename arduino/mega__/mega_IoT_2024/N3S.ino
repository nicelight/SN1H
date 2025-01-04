
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
      N3_spots.rightNowOn = 0; // ничего не делаем, убираем флаг
    } else {
      N3_spots.state = 0; // выключаем
      update_N3_Lamps();
      Serial.println("/nN3_S1_but Single");
    }
  }//N3_spots.isSingle

  // двойной клик. on\off альтернатива
  if (N3_S1_but.isDouble()) {
//    if (N3_museums.state) {
//      N3_museums.state = 0;
//      N3_museums.lamp1 = 0; // не особо нужно в данном случае, разве для модбаса
//      N3_museums.lamp2 = 0; // не особо нужно в данном случае, разве для модбаса
//    } else {
//      N3_museums.state = 1;
//      N3_museums.lamp1 = 1; // не особо нужно в данном случае, разве для модбаса
//      N3_museums.lamp2 = 1; // не особо нужно в данном случае, разве для модбаса
//    }
//    update_N3_museums();
  }
  // тройной клик. меняем состояние светильников на 1, 2, 1+2.
  if (N3_S1_but.isTriple())
  {
    N3_spots.rightNowOn = 0; // флаг сбрасываем ( за ним приходится следить из каждого вызова кнопок
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
  if (N3_S1_but.isHolded())
  {
    //      тушим весь свет и отправляем режим ночь
    N3_spots.state = 0;
    //
    // тут отправляем команду на ночной режим
    //
    Serial.print("\n\n\t\tN3_S1_but  NIGHT MODE ON\n\n");// TODO отправка режима ночь !!!
    update_N3_Lamps();
    //    update_N3_kitchen(); // раскоментить как допишется кухня
  }

  if (N3_S1_but.hasClicks())
  {
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
