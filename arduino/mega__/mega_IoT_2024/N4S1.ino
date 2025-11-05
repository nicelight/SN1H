
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
      N4_kitchen.lamp1 = ON;
      N4_tracks.state = 1;
      update_N4_Lamps();
      delay(50);
      update_N4_museums();
      delay(50);
      update_N4_Tracks();
      delay(50);
      update_N4_kitchen();
    }
    else {
      //      тушим весь свет и отправляем режим ночь
      N4_spots.state = 0;
      N4_museums.state = 0;
      N4_kitchen.state = 0;
      N4_kitchen.lamp1 = OFF;
      N4_tracks.state = 0;
      update_N4_museums();
      delay(50);
      update_N4_Tracks();
      delay(50);
      update_N4_Lamps();
      delay(50);
      update_N4_kitchen();
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


/* Документация по логке вкладки 


Зона N4 управляется обработчиком handleN4S1() и вспомогательными функциями в N4S1.ino.
Кнопка N4_S1_but обрабатывает все типы нажатий, а структура N4_spots хранит режим (lamp1, lamp2, state, rightNowOn, mode).
Ниже логика события за событием.

**Основной цикл**

 - Каждый проход вызывает N4_S1_but.tick() и EE_N4_spots.tick() (N4S1.ino (lines 23-24)),
 обновляя состояние кнопки и менеджера EEPROM.
- Дальше идут обработчики разных жестов, которые выставляют флаги и вызывают update_* для вывода на пины.

**Первое нажатие (Press)**

Если общий свет (N4_spots.state) был выключен, ставится state = 1 и флаг rightNowOn = 1,
после чего включаются лампы N4_SP1/N4_SP2 (N4S1.ino (lines 27-34), update_N4_Lamps() в N4S1.ino (lines 140-150)).
Это мгновенное включение при входе.

**Одиночный клик (Single)**

Если только что был Press и флаг rightNowOn ещё поднят, одиночный клик просто снимает этот флаг,
оставляя свет в текущем состоянии — защита от мгновенного двойного реагирования (N4S1.ino (lines 41-44)).
Иначе одиночный клик выключает общий свет (state = 0) и отправляет команды на пины и
Modbus (N4S1.ino (lines 44-47) + update_N4_Lamps()).

**Двойной клик (Double)**

- Сбрасывает rightNowOn и переключает подсветку «музей/эрмитаж» (структура N4_museums): 
если горела — гасит обе линии, если была выключена — включает обе (N4S1.ino (lines 51-63)).
- После обновления состояния вызывается update_N4_museums() (N4S1.ino (lines 152-162)),
который пишет значения на пины N4_MUS/N4_ERM и в coil ha[N4_MUSEUMS_LIGHTS].

**Тройной клик (Triple)**

- Используется для перебора трёх сцен основного света: только лампа 1, обе лампы, только лампа 2. 
Перебор идёт через поле mode, при каждой смене соответствующим образом выставляются lamp1/lamp2 (N4S1.ino (lines 65-87)).
- После выбора сцены update_N4_Lamps() вносит изменения в физические выходы и Modbus (N4S1.ino (line 88)),
а EE_N4_spots.update() и EE_N4_museums.update() сохраняют новую комбинацию в EEPROM (N4S1.ino (lines 89-91)).

**Долгое удержание (Holded)**

- Если флаг rightNowOn ещё активен (то есть свет только что включили), удержание трактуется как «режим присутствия»: включает все связанные группы (spots, museums, kitchen, tracks) и обновляет их по очереди с небольшими задержками (N4S1.ino (lines 96-108)).
Если флаг неактивен, удержание считается «ночным режимом»: выключает те же группы и также вызывает update_* последовательно (N4S1.ino (lines 110-126)). Вывод для кухни закомментирован — выглядит как временное решение, пока не реализован update_N4_kitchen().

**Прочее**

hasClicks() сбрасывает rightNowOn и печатает количество кликов, но не меняет выходы (N4S1.ino (lines 129-135)).
В конце update_N4_modbus() сравнивает локальные флаги со значениями, полученными 
по Modbus (coils ha[N4_LIGHTS] и ha[N4_MUSEUMS_LIGHTS]), и при расхождении синхронизирует 
состояние ламп (N4S1.ino (lines 170-190)).
Это позволяет внешнему мастеру включать/выключать те же группы, а прошивка самостоятельно приведёт 
аппаратное состояние в соответствие.
В итоге N4_S1 обслуживает: мгновенное включение при входе, обычное выключение,
сцену музея/эрмитажа, цикл из трёх режимов спотов и два сценарных режима по удержанию (всё включить или всё выключить).

 
 */
