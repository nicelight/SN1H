//#define N6_SP 10
//#define N6_LED 11

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
  // первое срабатывание. если выключен свет, включим и установим флаг о включении
  //-- заменено N4 _S1_but  на N6_S1_but
  if (N6_S1_but.isPress()) {
    //--  заменено N4 _spots на N6_spots
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
      N6_spots.rightNowOn = 0; // ничего не делаем, убираем флаг
    } else {
      N6_spots.state = 0; // выключаем
      update_N6_Lamps();
      Serial.println("N6_S1 Single\n");

    }
  }//N6_spots.isSingle

  // тройной клик. меняем состояние светильников на 1, 2, 1+2.
  if (N6_S1_but.isTriple())
  {
    N6_spots.rightNowOn = 0; // флаг сбрасываем ( за ним приходится следить из каждого вызова кнопок
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
    if (N6_spots.rightNowOn) { // если мгновенно включен свет ( т.е. было темно)
      N6_spots.rightNowOn = 0; // флаг сбрасываем
      //      включим его весь
      N6_spots.lamp1 = ON;
      N6_spots.lamp2 = ON;
    }
    else  // если же было светло
    {
      //      тушим весь свет и отправляем режим ночь
      N6_spots.state = 0;
      Serial.print("\n\n\t\t N6 NIGHT MODE ON\n\n");// TODO отправка режима ночь !!!
    }
    update_N6_Lamps();
    Serial.println("N6_S1 Holded\n");
  }

  if (N6_S1_but.hasClicks())
  {
    Serial.print("N6_S1 multi Clicks: ");
    Serial.println(N6_S1_but.getClicks());
    N6_spots.rightNowOn = 0; // флаг сбрасываем
    // проверка на наличие нажатий
  }
  // если кнопка была удержана и хотим подсчитать время удержания
  //    if (button.isStep()) {
  //    value++;                                            // увеличивать/уменьшать переменную value с шагом и интервалом
  //    Serial.println(value);                              // для примера выведем в порт
  //  }
}//handleN6S1()

void update_N6_Lamps() {
  if (N6_spots.state) {

//#define N6_SENS1 34
//#define N6_SENS_PIR 35
//#define N6_SP 10
//#define N6_LED 11

    digitalWrite(N6_SP, N6_spots.lamp1);
    digitalWrite(N6_LED, N6_spots.lamp2);
  }
  else
  {
    digitalWrite(N6_SP, OFF);
    digitalWrite(N6_LED, OFF);
  }
}//update_N6_Lamps()
