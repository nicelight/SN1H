
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
      N7_spots.rightNowOn = 0; // ничего не делаем, убираем флаг
    } else {
      N7_spots.state = 0; // выключаем
      update_N7_Lamps();
      Serial.println("/nN7_S_GATE_but Single");
    }
  }//N7_spots.isSingle

  // тройной клик. меняем состояние светильников на 1, 2, 1+2.
  if (N7_S_GATE_but.isTriple())
  {
    N7_spots.rightNowOn = 0; // флаг сбрасываем ( за ним приходится следить из каждого вызова кнопок
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
  if (N7_S_GATE_but.isHolded())
  {
    //      тушим весь свет и отправляем режим ночь
    N7_spots.state = 0;
    Serial.print("\n\n\t\tN7_S_GATE_but  NIGHT MODE ON\n\n");// TODO отправка режима ночь !!!
    update_N7_Lamps();
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
