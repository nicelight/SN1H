
// обработка нажатий клавиш N2.S1 ( у входа в гостевую, возле планшета)
// функция по тройному нажатию меняет состояния свечений на 1, 2, 1+2
// по одинарному нажатию включает\выключает свет
// по долгому удержанию, выключает свет и посылает сигнал - темноты
void handleN2S1() {
  /*
    документация:
    //  if (N2_S1_but.isPress()) Serial.println("Preuss");         // первое срабатывание. если выключен свет, включим и установим флаг о включении
    //  if (N2_S1_but.isSingle()) Serial.println("Single\n");       // одиночный клик. если флаг о включенном свете возведен, ничего не делаем, иначе убираем флаг и меняем состояние света
    //  if (N2_S1_but.isDouble()) Serial.println("Double\n");       // двойной клик. доп освещение
    //  if (N2_S1_but.isTriple()) Serial.println("Triple\n");       // тройной клик. меняем состояние светильников на 1, 2, 1+2.
    //  if (N2_S1_but.isHolded()) Serial.println("Holded\n");      0
    //struct LampTriple       // структура для работы ламп
    //{
    //  bool lamp1;   // состояние первой лампы
    //  bool lamp2;   // второй лампы
    //  bool state;   // включены или выключены лампы на данный момент
    //  bool rightNowOn;  // маякует о том, что уже было включение ламп, чтобы повторно не включать
    //  uint8_t mode; // режим ( первая лампа включена, вторая включена или обе включены)
    //};
  */
  N2_S1_but.tick();
  EE_N2_spots.tick();

  // первое срабатывание. если выключен свет, включим и установим флаг о включении
  if (N2_S1_but.isPress()) {
    if (!N2_spots.state) {
      N2_spots.state = 1; // включаем
      N2_spots.rightNowOn = 1;// возводим флаг, что произошло мгновенное включение
      update_N2_Lamps(); // зажжем нужные лампы
      Serial.println("\n N2_S1_but Press");
      // обновить модбас
    }
  }

  // одиночный клик. если флаг о включенном свете возведен, убираем его и ничего не делаем,
  //  иначе убираем флаг и выключаем свет
  if (N2_S1_but.isSingle())
  {
    if (N2_spots.rightNowOn) { // если мгновенно включен свет
      N2_spots.rightNowOn = 0; // ничего не делаем, убираем флаг
    } else {
      N2_spots.state = 0; // выключаем
      update_N2_Lamps();
      Serial.println("/nN2_S1_but Single");
    }
  }//N2_spots.isSingle

  // тройной клик. меняем состояние светильников на 1, 2, 1+2.
  if (N2_S1_but.isTriple())
  {
    N2_spots.rightNowOn = 0; // флаг сбрасываем ( за ним приходится следить из каждого вызова кнопок
    // циклично меняем режимы работы 1..3
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
  if (N2_S1_but.isHolded()) {
    //      тушим весь свет и отправляем режим ночь
    N2_spots.state = 0;
  Serial.print("\n\n\t\tN2_S1_but  NIGHT MODE ON\n\n");// TODO отправка режима ночь !!!
  update_N2_Lamps();
}

if (N2_S1_but.hasClicks())
{
  Serial.print("N2_S1_but multi Clicks: ");
  Serial.println(N2_S1_but.getClicks());
  // проверка на наличие нажатий
}
// если кнопка была удержана и хотим подсчитать время удержания
//    if (button.isStep()) {
//    value++;                                            // увеличивать/уменьшать переменную value с шагом и интервалом
//    Serial.println(value);                              // для примера выведем в порт
//  }
}//handleN4_s1

void update_N2_Lamps() {
  if (N2_spots.state) {
    digitalWrite(N2_SP, N2_spots.lamp1);
//    digitalWrite(N2_LED, N2_spots.lamp2);
  }
  else
  {
    digitalWrite(N2_SP, OFF);
//    digitalWrite(N2_LED, OFF);

  }
}//update_N2_Lamps
