
// обработка нажатий клавиш N4.S1 ( у входа в гостевую, возле планшета)
// функция по тройному нажатию меняет состояния свечений на 1, 2, 1+2
// по одинарному нажатию включает\выключает свет
// по долгому удержанию, выключает свет и посылает сигнал - темноты
void handleN5S1() {
  /*
    документация:
    //  if (N5_S1_but.isPress()) Serial.println("Press");         // первое срабатывание. если выключен свет, включим и установим флаг о включении
    //  if (N5_S1_but.isSingle()) Serial.println("Single\n");       // одиночный клик. если флаг о включенном свете возведен, ничего не делаем, иначе убираем флаг и меняем состояние света
    //  if (N5_S1_but.isDouble()) Serial.println("Double\n");       // двойной клик. доп освещение
    //  if (N5_S1_but.isTriple()) Serial.println("Triple\n");       // тройной клик. меняем состояние светильников на 1, 2, 1+2.
    //  if (N5_S1_but.isHolded()) Serial.println("Holded\n");      0
    //struct LampTriple       // структура для работы ламп
    //{
    //  bool lamp1;   // состояние первой лампы
    //  bool lamp2;   // второй лампы
    //  bool state;   // включены или выключены лампы на данный момент
    //  bool rightNowOn;  // маякует о том, что уже было включение ламп, чтобы повторно не включать
    //  uint8_t mode; // режим ( первая лампа включена, вторая включена или обе включены)
    //};
  */
  N5_S1_but.tick();
  EE_N5_spots.tick();

  // первое срабатывание. если выключен свет, включим и установим флаг о включении
  if (N5_S1_but.isPress()) {
    if (!N5_spots.state) {
      N5_spots.state = 1; // включаем
      N5_spots.rightNowOn = 1;// возводим флаг, что произошло мгновенное включение
      update_N5_Lamps(); // зажжем нужные лампы
      Serial.println("\n N5_S1_but Press");
      // обновить модбас
    }
  }

  // одиночный клик. если флаг о включенном свете возведен, убираем его и ничего не делаем,
  //  иначе убираем флаг и выключаем свет
  if (N5_S1_but.isSingle())
  {
    if (N5_spots.rightNowOn) { // если мгновенно включен свет
      N5_spots.rightNowOn = 0; // убираем флаг мгновенного нажатия
    } else {
      N5_spots.state = 0; // выключаем
      update_N5_Lamps();
      Serial.println("/nN5_S1_but Single");
    }
  }//N5_spots.isSingle

  //двойной клик включает\выключает кровать
  if (N5_S1_but.isDouble()) {
    N5_spots.rightNowOn = 0; // убираем флаг мгновенного нажатия
    //    N5_spots.lamp3 = !N5_spots.lamp3;
    //    update_N5_Lamps();
  }

  // тройной клик. меняем состояние светильников на 1, 2, 1+2.
  if (N5_S1_but.isTriple())
  {
    N5_spots.rightNowOn = 0; // флаг сбрасываем ( за ним приходится следить из каждого вызова кнопок
    // циклично меняем режимы работы 1..3
    switch (N5_spots.mode)
    {
      case 0:
        N5_spots.mode = 1;
        N5_spots.lamp1 = OFF;
        N5_spots.lamp2 = ON;
        break;
      case 1:
        N5_spots.mode = 2;
        N5_spots.lamp1 = ON;
        N5_spots.lamp2 = ON;
        break;
      case 2:
        N5_spots.mode = 0;
        N5_spots.lamp1 = ON;
        N5_spots.lamp2 = OFF;
        break;
    }
    update_N5_Lamps();     // включаем в зависимости от прошлого запомненного режима
    Serial.println("\nN5_S1_but Triple\n");
    Serial.print("N5_spots.mode = ");
    Serial.print(N5_spots.mode);
    Serial.print("\t N5_spots.lamp2 = ");
    Serial.println(N5_spots.lamp2);

    EE_N5_spots.update();                    // стараемся не вызывать часто эти данные
  }

  // удержание. если флаг о включении возведен(т.е. он был выключен) включим весь свет в комнате,
  // иначе(если свет и так включен) выключаем весь свет в комнате, и даже тот за который не отвечаем
  if (N5_S1_but.isHolded()) {
    // СЦЕНА ВХОД, заходим в темную ванну удерживая кнопку, хотим с вытяжкой
    if (N5_spots.rightNowOn) { //если свет включен после тьмы
      N5_spots.rightNowOn = 0;
      N5_spots.state = 1;
      N5_spots.lamp3 = ON; // кровать включим
      update_N5_Lamps();
    } //
    // СЦЕНА ВЫХОД выходим из комнаты с включенным светом,
    else {
      N5_spots.state = 0;
      N5_spots.lamp3 = OFF; // кровать выключим
      update_N5_Lamps();
      Serial.println("N5 Holded\n");
    }
  }//holded

  if (N5_S1_but.hasClicks())
  {
    N5_spots.rightNowOn = 0; // убираем флаг мгновенного нажатия
    Serial.print("N5_S1_but multi Clicks: ");
    Serial.println(N5_S1_but.getClicks());
    // проверка на наличие нажатий
  }
  // если кнопка была удержана и хотим подсчитать время удержания
  //    if (button.isStep()) {
  //    value++;                                            // увеличивать/уменьшать переменную value с шагом и интервалом
  //    Serial.println(value);                              // для примера выведем в порт
  //  }
}//handleN4_s1

void update_N5_Lamps() {
  //  digitalWrite(N5_BED, N5_spots.lamp3);
  if (N5_spots.state) {
    digitalWrite(N5_SP1, N5_spots.lamp1);
    digitalWrite(N5_SP2, N5_spots.lamp1);
    digitalWrite(N5_SHELF, N5_spots.lamp2);
    digitalWrite(N5_BED, N5_spots.lamp2);
  }
  else
  {
    digitalWrite(N5_SP1, OFF);
    digitalWrite(N5_SP2, OFF);
    digitalWrite(N5_SHELF, OFF);
    digitalWrite(N5_BED, OFF);

  }

}//update_N5_Lamps
