
// обработка нажатий клавиш N4.S1 ( у входа в гостевую, возле планшета)
// функция по двойному нажатию меняет состояния свечений на 1, 2, 1+2
// по одинарному нажатию включает\выключает свет
// по долгому удержанию, выключает свет и посылает сигнал - темноты
void handleN4S1() {
  /*
    документация:
    //  if (N4_S1_but.isPress()) Serial.println("Press");         // первое срабатывание. если выключен свет, включим и установим флаг о включении
    //  if (N4_S1_but.isSingle()) Serial.println("Single\n");       // одиночный клик. если флаг о включенном свете возведен, ничего не делаем, иначе убираем флаг и меняем состояние света
    //  if (N4_S1_but.isDouble()) Serial.println("Double\n");       // двойной клик. меняем состояние светильников на 1, 2, 1+2.
    //  if (N4_S1_but.isHolded()) Serial.println("Holded\n");      0
    //struct LampPair       // структура для работы ламп
    //{
    //  bool lamp1;   // состояние первой лампы
    //  bool lamp2;   // второй лампы
    //  bool state;   // включены или выключены лампы на данный момент
    //  bool rightNowOn;  // маякует о том, что уже было включение ламп, чтобы повторно не включать
    //  uint8_t mode; // режим ( первая лампа включена, вторая включена или обе включены)
    //};
  */
  // первое срабатывание. если выключен свет, включим и установим флаг о включении
  if (N4_S1_but.isPress()) {
    if (!N4_spots.state) {
      N4_spots.state = 1; // включаем
      N4_spots.rightNowOn = 1;// возводим флаг, что произошло мгновенное включение
      updateLamps(); // зажжем нужные лампы
      Serial.println("Press");
      logButtonPress(N4_spots);
      // обновить модбас
    }
  }

  // одиночный клик. если флаг о включенном свете возведен, убираем его и ничего не делаем,
  //  иначе убираем флаг и выключаем свет
  if (N4_S1_but.isSingle())
  {
    if (N4_spots.rightNowOn) { // если мгновенно включен свет
      N4_spots.rightNowOn = 0; // ничего не делаем, убираем флаг
    } else {
      N4_spots.state = 0; // выключаем
      updateLamps();
      Serial.println("Single\n");
      logButtonPress(N4_spots);

    }
  }//N4_spots.isSingle

  //  updateLamps();
  //  logButtonPress(N4_spots);

  // двойной клик. меняем состояние светильников на 1, 2, 1+2.
  if (N4_S1_but.isDouble())
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
    updateLamps();     // включаем в зависимости от прошлого запомненного режима
    Serial.println("Double\n");
    logButtonPress(N4_spots);
    EE_N4_spots.update();                    // стараемся не вызывать часто эти данные
    // EE_N2_spots.update();
    // EE_N3_spots.update();
  }

  // удержание. если флаг о включении возведен(т.е. он был выключен) включим весь свет в комнате,
  // иначе(если свет и так включен) выключаем весь свет в комнате, и даже тот за который не отвечаем
  if (N4_S1_but.isHolded())
  {
    if (N4_spots.rightNowOn) { // если мгновенно включен свет ( т.е. было темно)
      N4_spots.rightNowOn = 0; // флаг сбрасываем
      //      включим его весь
      N4_spots.lamp1 = ON;
      N4_spots.lamp2 = ON;
    }
    else  // если же было светло
    {
      //      тушим весь свет и отправляем режим ночь
      N4_spots.state = 0;
      Serial.print("\n\n\t\t NIGHT MODE ON\n\n");// TODO отправка режима ночь !!!
    }
    updateLamps();
    Serial.println("Holded\n");
    logButtonPress(N4_spots);
  }

  if (N4_S1_but.hasClicks())
  {
    Serial.print("multi Clicks: ");
    Serial.println(N4_S1_but.getClicks());
    N4_spots.rightNowOn = 0; // флаг сбрасываем
    // проверка на наличие нажатий
  }
  // если кнопка была удержана и хотим подсчитать время удержания
  //    if (button.isStep()) {
  //    value++;                                            // увеличивать/уменьшать переменную value с шагом и интервалом
  //    Serial.println(value);                              // для примера выведем в порт
  //  }
}//handleN4_s1
