static uint32_t recupDelay, recupMs;
static byte recupState = 0;

void recuperation() {
  // проверка входящих по рекуператору
  if ((ha[RECUPERATOR] == 1) && (!recupState)) {
      recupState = 1; //  запуск рекуперации на время
  } else if ((ha[RECUPERATOR] == 0) && (recupState)) {
      recupState = 7; //  останов рекуперации 
  }
  
  switch (recupState) {
    // ожидание
    case 0:
      break;
    // запуск вытяжки на пол часа
    case 1:
      recupMs = millis();
        digitalWrite(RECUPERATOR, ON);
      recupState = 5;
      recupDelay = 1800000; // пол часа
      break;
    // ожидание окончания
    case 5:
      if ((millis() - recupMs) > recupDelay) {
        recupState = 7;
      }
      break;
    case 7:
        digitalWrite(RECUPERATOR, OFF);
        recupState = 0;
        break;

      }//switch

}//recuperation()
