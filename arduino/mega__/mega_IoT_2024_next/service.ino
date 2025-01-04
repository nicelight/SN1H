
void updateAllLights() {

  update_N4_Lamps();
  update_N4_Tracks();
  update_N5_Lamps();
  update_N6_Lamps();
  update_N7_Lamps();
}

void pirN7() {
  if (each100msPirN7.ready()) { // каждых 100 мс

    statePIR7 = digitalRead(N7_SENS_PIR);
    if (statePIR7 && !N7_spots.state) { // сработал датчик и свет не горел
      digitalWrite(N7_SP, ON);
      if (!startPirLightN7) {
        startPirLightN7 = 1;
        each5secForN7.rst();
      }
    }
    //    Serial.print("\n\t");
    //    Serial.print(millis() >> 10); // сколько ~секунд прошло
    //    Serial.print("\t\t\t\t pir 7 = ");
    //    Serial.println(statePIR7);

    // прошло время служения PIR N7 и состояния света от кнопки по прежнему выключенные
    // потушим принудительно
    if (each5MinForN7.ready()) {
      if (!N7_spots.state && startPirLightN7) { // вышел таймаут служения светом от сенсора и за это время не произошло включения с кнопки
        digitalWrite(N7_SP, OFF);
      }
      startPirLightN7 = 0;
    }

  }// each 100 ms
}//pirN7()
//
//
//
