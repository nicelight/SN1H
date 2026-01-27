// Обработчик трековой кнопки на кухне (lamp1 из N4_tracks)
void handleN4K2() {
  /*
    Жесты:
      if (N4_S_KITH_TRACK_but.isSingle())  // одиночный клик — переключаем трековый свет
      if (N4_S_KITH_TRACK_but.isDouble())  // двойной клик — только лог
      if (N4_S_KITH_TRACK_but.isHolded())  // удержание — сцены «все включить» / «ночь»
  */
  N4_S_KITH_TRACK_but.tick();
  EE_N4_tracks.tick();

  if (N4_S_KITH_TRACK_but.isSingle()) {
    N4_tracks.state = !N4_tracks.state;
    N4_tracks.lamp1 = N4_tracks.state ? ON : OFF;
    update_N4_Tracks();
    Serial.println("\nN4_S_KITH_TRACK_but Single");
  }

  if (N4_S_KITH_TRACK_but.isDouble()) {
    Serial.println("\nN4_S_KITH_TRACK_but Double");
  }

  if (N4_S_KITH_TRACK_but.isHolded()) {
    if (!N4_tracks.state) {
      N4_tracks.state = 1;
      N4_tracks.lamp1 = ON;
      N4_tracks.lamp2 = ON;
      N4_spots.state = 1;
      N4_museums.state = 1;
      N4_kitchen.state = 1;
      N4_kitchen.lamp1 = ON;
      update_N4_Tracks();
      delay(50);
      update_N4_Lamps();
      delay(50);
      update_N4_museums();
      delay(50);
      update_N4_kitchen();
      Serial.print("\n\n\t\tN4_S_KITH_TRACK_but ALL LIGHTS MODE ON\n\n");

    } else {
      N4_tracks.state = 0;
      N4_tracks.lamp1 = OFF;
      N4_tracks.lamp2 = OFF;
      N4_spots.state = 0;
      N4_museums.state = 0;
      N4_kitchen.state = 0;
      N4_kitchen.lamp1 = OFF;
      update_N4_museums();
      delay(50);
      update_N4_Lamps();
      delay(50);
      update_N4_Tracks();
      delay(50);
      update_N4_kitchen();
      Serial.print("\n\n\t\tN4_S_KITH_TRACK_but NIGHT MODE ON\n\n");
    }
  }

  if (N4_S_KITH_TRACK_but.hasClicks()) {
    Serial.print("N4_S_KITH_TRACK_but multi Clicks: ");
    Serial.println(N4_S_KITH_TRACK_but.getClicks());
  }

  update_N4_tr_modbus();
}//handleN4K2()
