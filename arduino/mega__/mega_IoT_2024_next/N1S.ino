// обработка нажатий клавиш N1.S1 ( у входа в гостевую, возле планшета)
// функция по тройному нажатию меняет состояния свечений на 1, 2, 1+2
// по одинарному нажатию включает\выключает свет
// по долгому удержанию, выключает свет и посылает сигнал - темноты
void handleN1S1() {
  /*
    документация:
    //  if (N1_S1_but.isPress()) Serial.println("Preuss");         // первое срабатывание. если выключен свет, включим и установим флаг о включении
    //  if (N1_S1_but.isSingle()) Serial.println("Single\n");       // одиночный клик. если флаг о включенном свете возведен, ничего не делаем, иначе убираем флаг и меняем состояние света
    //  if (N1_S1_but.isDouble()) Serial.println("Double\n");       // двойной клик. доп освещение
    //  if (N1_S1_but.isTriple()) Serial.println("Triple\n");       // тройной клик. меняем состояние светильников на 1, 2, 1+2.
    //  if (N1_S1_but.isHolded()) Serial.println("Holded\n");      0
    //struct LampTriple       // структура для работы ламп
    //{
    //  bool lamp1;   // состояние первой лампы
    //  bool lamp2;   // второй лампы
    //  bool state;   // включены или выключены лампы на данный момент
    //  bool rightNowOn;  // маякует о том, что уже было включение ламп, чтобы повторно не включать
    //  uint8_t mode; // режим ( первая лампа включена, вторая включена или обе включены)
    //};
  */
  N1_S1_but.tick();
  EE_N1_spots.tick();

  // первое срабатывание. если выключен свет, включим и установим флаг о включении
  if (N1_S1_but.isPress()) {
    if (!N1_spots.state) {
      N1_spots.state = 1; // включаем
      N1_spots.rightNowOn = 1;// возводим флаг, что произошло мгновенное включение
      update_N1_Lamps(); // зажжем нужные лампы
      Serial.println("\n N1_S1_but Press");
      // обновить модбас
    }
  }

  // одиночный клик. если флаг о включенном свете возведен, убираем его и ничего не делаем,
  if (N1_S1_but.isSingle())
  {
    if (N1_spots.rightNowOn) { // если мгновенно включен свет
      N1_spots.rightNowOn = 0; // ничего не делаем, убираем флаг
    } else {
      N1_spots.state = 0; // выключаем
      update_N1_Lamps();
      Serial.println("/nN1_S1_but Single");
    }
  }//N1_spots.isSingle

  // тройной клик. меняем состояние светильников на 1, 2, 1+2.
  if (N1_S1_but.isTriple())
  {
    N1_spots.rightNowOn = 0; // флаг сбрасываем ( за ним приходится следить из каждого вызова кнопок
    // циклично меняем режимы работы 1..3
    switch (N1_spots.mode)
    {
      case 0:
        N1_spots.mode = 1;
        N1_spots.lamp1 = OFF;
        N1_spots.lamp2 = ON;
        break;
      case 1:
        N1_spots.mode = 2;
        N1_spots.lamp1 = ON;
        N1_spots.lamp2 = ON;
        break;
      case 2:
        N1_spots.mode = 0;
        N1_spots.lamp1 = ON;
        N1_spots.lamp2 = OFF;
        break;
    }
    update_N1_Lamps();     // включаем в зависимости от прошлого запомненного режима
    Serial.println("\nN1_S1_but Triple \n");
    //    EE_N1_spots.update();                    // закомментировал в процессе отловки бага
  }

  // удержание. если флаг о включении возведен(т.е. он был выключен) включим весь свет в комнате,
  // иначе(если свет и так включен) выключаем весь свет в комнате, и даже тот за который не отвечаем
  if (N1_S1_but.isHolded()) {
    N1_spots.rightNowOn = 0; // убираем флаг
    //      тушим весь свет и отправляем режим ночь
    N1_spots.state = 0;
    Serial.print("\n\n\t\tN1_S1_but  NIGHT MODE ON\n\n");// TODO отправка режима ночь !!!
    update_N1_Lamps();
  }

  if (N1_S1_but.hasClicks())
  {
    N1_spots.rightNowOn = 0; //убираем флаг
    Serial.print("N1_S1_but multi Clicks: ");
    Serial.println(N1_S1_but.getClicks());
    // проверка на наличие нажатий
  }
  // если кнопка была удержана и хотим подсчитать время удержания
  //    if (button.isStep()) {
  //    value++;                                            // увеличивать/уменьшать переменную value с шагом и интервалом
  //    Serial.println(value);                              // для примера выведем в порт
  //  }
  update_N1_modbus();
}//handleN4_s1


void update_N1_Lamps() {
  if (N1_spots.state) {
    ha[N1_LIGHTS] = 1;
    digitalWrite(N1_SP1, N1_spots.lamp1);
    digitalWrite(N1_SP2, N1_spots.lamp2);
    //    digitalWrite(N1_SP1, ON); //временный костыль для проверки. выше правильно
    //    digitalWrite(N1_SP2, ON);
  }
  else
  {
    ha[N1_LIGHTS] = 0;
    digitalWrite(N1_SP1, OFF);
    digitalWrite(N1_SP2, OFF);

  }
}//update_N1_Lamps


// обработка modbus
//N1_LIGHTS 31
void update_N1_modbus() { 
  if ((N1_spots.state == 0) && (ha[N1_LIGHTS] == 1)) { //свет потушен а с ha пришло - включить
    N1_spots.state = 1;
    update_N1_Lamps();
  }
  else if ((N1_spots.state == 1) && (ha[N1_LIGHTS] == 0)) { //включенный свет надо потушить
    N1_spots.state = 0;
    update_N1_Lamps();

  }
}//update_N1_modbus()
