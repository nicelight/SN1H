
// обработка нажатий клавиш N4.S1 ( у входа в гостевую, возле планшета)
// функция по тройному нажатию меняет состояния свечений на 1, 2, 1+2
// по одинарному нажатию включает\выключает свет
// по долгому удержанию, выключает свет и посылает сигнал - темноты
void handleN4S1() {
  /*
    документация:
    //  if (N4_S1_but.isPress()) Serial.println("Press");         // первое срабатывание. если выключен свет, включим и установим флаг о включении
    //  if (N4_S1_but.isSingle()) Serial.println("Single\n");       // одиночный клик. если флаг о включенном свете возведен, ничего не делаем, иначе убираем флаг и меняем состояние света
    //  if (N4_S1_but.isDouble()) Serial.println("Double\n");       // двойной клик. доп освещение
    //  if (N4_S1_but.isTriple()) Serial.println("Triple\n");       // тройной клик. меняем состояние светильников на 1, 2, 1+2.
    //  if (N4_S1_but.isHolded()) Serial.println("Holded\n");      0
    //struct LampTriple       // структура для работы ламп
    //{
    //  bool lamp1;   // состояние первой лампы
    //  bool lamp2;   // второй лампы
    //  bool state;   // включены или выключены лампы на данный момент
    //  bool rightNowOn;  // маякует о том, что уже было включение ламп, чтобы повторно не включать
    //  uint8_t mode; // режим ( первая лампа включена, вторая включена или обе включены)
    //};
  */
  N4_S1_but.tick();
  EE_N4_spots.tick();

  // первое срабатывание. если выключен свет, включим и установим флаг о включении
  if (N4_S1_but.isPress()) {
    if (!N4_spots.state) {
      N4_spots.state = 1; // включаем
      N4_spots.rightNowOn = 1;// возводим флаг, что произошло мгновенное включение
      update_N4_Lamps(); // зажжем нужные лампы
      Serial.println("\n N4_S1_but Press");
      // обновить модбас
    }
  }

  // одиночный клик. если флаг о включенном свете возведен, убираем его и ничего не делаем,
  //  иначе убираем флаг и выключаем свет
  if (N4_S1_but.isSingle())
  {
    if (N4_spots.rightNowOn) { // если мгновенно включен свет
      N4_spots.rightNowOn = 0; // убираем флаг мгновенного нажатия
    } else {
      N4_spots.state = 0; // выключаем
      update_N4_Lamps();
      Serial.println("/nN4_S1_but Single");
    }
  }//N4_spots.isSingle

  // двойной клик. on\off museum + ermitage.
  if (N4_S1_but.isDouble()) {
    N4_spots.rightNowOn = 0; // убираем флаг мгновенного нажатия
    if (N4_museums.state) {
      N4_museums.state = 0;
      N4_museums.lamp1 = 0; // не особо нужно в данном случае, разве для модбаса
      N4_museums.lamp2 = 0; // не особо нужно в данном случае, разве для модбаса
    } else {
      N4_museums.state = 1;
      N4_museums.lamp1 = 1; // не особо нужно в данном случае, разве для модбаса
      N4_museums.lamp2 = 1; // не особо нужно в данном случае, разве для модбаса
    }
    update_N4_museums();
  }
  // тройной клик. меняем состояние светильников на 1, 2, 1+2.
  if (N4_S1_but.isTriple())
  {
    N4_spots.rightNowOn = 0; // убираем флаг мгновенного нажатия
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
    update_N4_Lamps();     // включаем в зависимости от прошлого запомненного режима
    Serial.println("\nN4_S1_but Double\n");
    EE_N4_spots.update();                    // стараемся не вызывать часто эти данные
    EE_N4_museums.update();
  }

  // удержание. если флаг о включении возведен(т.е. он был выключен) включим весь свет в комнате,
  // иначе(если свет и так включен) выключаем весь свет в комнате, и даже тот за который не отвечаем
  if (N4_S1_but.isHolded()) {
    if (N4_spots.rightNowOn ) {
      N4_spots.rightNowOn = 0; // убираем флаг мгновенного нажатия
      N4_spots.state = 1;
      N4_museums.state = 1;
      N4_kitchen.state = 1;
      N4_tracks.state = 1;
      update_N4_Lamps();
      delay(50);
      update_N4_museums();
      delay(50);
      update_N4_Tracks();
      //    update_N4_kitchen(); // раскоментить как допишется кухня
    }
    else {
      //      тушим весь свет и отправляем режим ночь
      N4_spots.state = 0;
      N4_museums.state = 0;
      N4_kitchen.state = 0;
      N4_tracks.state = 0;
      update_N4_museums();
      delay(50);
      update_N4_Tracks();
      delay(50);
      update_N4_Lamps();
      //    update_N4_kitchen(); // раскоментить как допишется кухня
      //
      // тут отправляем команду на ночной режим
      //
      Serial.print("\n\n\t\tN4_S1_but  NIGHT MODE ON\n\n");// TODO отправка режима ночь !!!
    }
  }//holded

  if (N4_S1_but.hasClicks())
  {
    N4_spots.rightNowOn = 0; // убираем флаг мгновенного нажатия
    Serial.print("N4_S1_but multi Clicks: ");
    Serial.println(N4_S1_but.getClicks());
    // проверка на наличие нажатий
  }
  // если кнопка была удержана и хотим подсчитать время удержания
  //    if (button.isStep()) {
  //    value++;                                            // увеличивать/уменьшать переменную value с шагом и интервалом
  //    Serial.println(value);                              // для примера выведем в порт
  //  }
  update_N4_modbus();
}//handleN4_s1


void update_N4_Lamps() {
  if (N4_spots.state) {
    ha[N4_LIGHTS] = 1;
    digitalWrite(N4_SP1, N4_spots.lamp1);
    digitalWrite(N4_SP2, N4_spots.lamp2);
  }
  else
  {
    ha[N4_LIGHTS] = 0;
    digitalWrite(N4_SP1, OFF);
    digitalWrite(N4_SP2, OFF);
  }
}//update_N4_Lamps


void update_N4_museums() {
  if (N4_museums.state) {
    ha[N4_MUSEUMS_LIGHTS] = 1;
    digitalWrite(N4_MUS, N4_spots.lamp1);
    digitalWrite(N4_ERM, N4_spots.lamp2);
  }
  else
  {
    ha[N4_MUSEUMS_LIGHTS] = 0;
    digitalWrite(N4_MUS, OFF);
    digitalWrite(N4_ERM, OFF);
  }
}//update_N4_museums()



// обработка modbus
//N4_LIGHTS 34
//N4_MUSEUMS_LIGHTS 17
//N4_TRACK_LIGHTS 15
//N4_KITCHEN_LIGHTS 16
//N4_MEAL_LIGHTS 18
void update_N4_modbus() {
  if ((N4_spots.state == 0) && (ha[N4_LIGHTS] == 1)) { //свет потушен а с ha пришло - включить
    N4_spots.state = 1;
    update_N4_Lamps();
  }
  else if ((N4_spots.state == 1) && (ha[N4_LIGHTS] == 0)) { //включенный свет надо потушить
    N4_spots.state = 0;
    update_N4_Lamps();
  }
  if ((N4_museums.state == 0) && (ha[N4_MUSEUMS_LIGHTS] == 1)) { //свет потушен а с ha пришло - включить
    N4_museums.state = 1;
    update_N4_museums();
  }
  else if ((N4_museums.state == 1) && (ha[N4_MUSEUMS_LIGHTS] == 0)) { //включенный свет надо потушить
    N4_museums.state = 0;
    update_N4_museums();
  }
}//update_N4_modbus()
