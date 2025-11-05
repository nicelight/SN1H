

// обработчик кухни в зоне N4
void handleN4K() {
  /*
    Документация жестов:
      if (N4_S_KITCHEN_but.isPress())   // первое касание. если свет был выключен — включаем и поднимаем флаг rightNowOn
      if (N4_S_KITCHEN_but.isSingle())  // одиночный клик. если rightNowOn ещё поднят — просто сбрасываем его, иначе переключаем свет
      if (N4_S_KITCHEN_but.isDouble())  // двойной клик. в нашей логике только записываем в Serial
      if (N4_S_KITCHEN_but.isHolded())  // удержание. общий сценарий «все включить / уйти из комнаты»
  */
  N4_S_KITCHEN_but.tick();
  EE_N4_kitchen.tick();

  if (N4_S_KITCHEN_but.isPress()) {
    if (!N4_kitchen.state) {
      N4_kitchen.state = 1;
      N4_kitchen.rightNowOn = 1;
      N4_kitchen.lamp1 = ON;
      update_N4_kitchen();
      Serial.println("\n N4_S_KITCHEN_but Press");
    }
  }

  if (N4_S_KITCHEN_but.isSingle()) {
    if (N4_kitchen.rightNowOn) {
      N4_kitchen.rightNowOn = 0;
    } else {
      N4_kitchen.state = !N4_kitchen.state;
      N4_kitchen.lamp1 = N4_kitchen.state ? ON : OFF;
      update_N4_kitchen();
      Serial.println("\nN4_S_KITCHEN_but Single");
    }
  }

  if (N4_S_KITCHEN_but.isDouble()) {
    Serial.println("\nN4_S_KITCHEN_but Double");
  }

  if (N4_S_KITCHEN_but.isHolded()) {
    if (N4_kitchen.rightNowOn) {
      N4_kitchen.rightNowOn = 0;
      N4_kitchen.state = 1;
      N4_kitchen.lamp1 = ON;
      N4_spots.state = 1;
      N4_museums.state = 1;
      N4_tracks.state = 1;
      update_N4_Lamps();
      delay(50);
      update_N4_museums();
      delay(50);
      update_N4_Tracks();
      delay(50);
      update_N4_kitchen();
    } else {
      N4_kitchen.rightNowOn = 0;
      N4_kitchen.state = 0;
      N4_kitchen.lamp1 = OFF;
      N4_spots.state = 0;
      N4_museums.state = 0;
      N4_tracks.state = 0;
      update_N4_museums();
      delay(50);
      update_N4_Tracks();
      delay(50);
      update_N4_Lamps();
      delay(50);
      update_N4_kitchen();
      Serial.print("\n\n\t\tN4 kitchen NIGHT MODE ON\n\n");
    }
  }

  if (N4_S_KITCHEN_but.hasClicks()) {
    N4_kitchen.rightNowOn = 0;
    Serial.print("N4_S_KITCHEN_but multi Clicks: ");
    Serial.println(N4_S_KITCHEN_but.getClicks());
  }

  update_N4_k_modbus();
}//handleN4K()


void update_N4_kitchen() {
  Serial.print("\n\tN4_kitchen.state = ");
  Serial.print(N4_kitchen.state);
  Serial.print("\tN4_kitchen.lamp1 = ");
  Serial.print(N4_kitchen.lamp1);
  if (N4_kitchen.state) {
    ha[N4_KITCHEN_LIGHTS] = 1;
    digitalWrite(N4_KITCH1, N4_kitchen.lamp1);
    digitalWrite(N4_KITCH2, N4_kitchen.lamp1);
  } else {
    ha[N4_KITCHEN_LIGHTS] = 0;
    digitalWrite(N4_KITCH1, OFF);
    digitalWrite(N4_KITCH2, OFF);
  }
}//update_N4_kitchen


// синхронизация по modbus
//N4_KITCHEN_LIGHTS 16
void update_N4_k_modbus() {
  if ((N4_kitchen.state == 0) && (ha[N4_KITCHEN_LIGHTS] == 1)) {
    N4_kitchen.state = 1;
    N4_kitchen.lamp1 = ON;
    update_N4_kitchen();
  } else if ((N4_kitchen.state == 1) && (ha[N4_KITCHEN_LIGHTS] == 0)) {
    N4_kitchen.state = 0;
    N4_kitchen.lamp1 = OFF;
    update_N4_kitchen();
  }
}//update_N4_k_modbus()
