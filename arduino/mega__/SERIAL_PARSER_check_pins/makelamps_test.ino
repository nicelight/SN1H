
//обновленная функция мейклампс
// в аргумент pinState будем передавать statelamp1 допустим
//а в аргумент pin будем передавать LAMP1 допустим
// дим без изменений
//
// эту функцию просто надо будет вызывать несколько раз везде, где нужно сразу переключить больше одной лампы



// кибитка цифровых рабов
void makelamps( byte pin, bool pinState) {

  // теперь работа цифровых рабов сводится до утопической простоты
    if(pinState)  digitalWrite(pin, RELAY_ON);
    else digitalWrite(pin, RELAY_OFF);
    // отчет 
    sendPinsEsp(pin, pinState);

}//makelamps







// кибитка аналоговых рабов
void makeAnaloglamps( byte pin, byte dim) {

  //***************************************************

  // ПОДСВЕТКА КНОПОК!!!!*******************************

  //***************************************************
    analogWrite(pin, dim);
    // отчет 
    sendPinsEsp(pin, dim);

    if (!ledstatergbwstrip) analogWrite(LEDRGBWSTRIP, dim);
    else analogWrite(LEDRGBWSTRIP, 0);
    // отчет 
    sendPinsEsp(LEDRGBWSTRIP, dim);

}//makeAnaloglamps()


// суровый прораб
void makeAllLamps(){
makelamps(LAMP1, statelamp1);
makelamps(LAMP2, statelamp2);
//...
//и до последней лампы
//..
}//makelamps 


// функция отправки состояний пинов в esp 
  // формирует на выходе сообщение в начале будет pn=  (чтобы парсер понял что речь идет о пинах)
  //далее 5 цифр  допустим pn=09255; это значит пин 9 состояние 255
  // или pn=23001 это значит пин 23 в состоянии 1.
void sendPinsEsp(byte whoIs, byte whatIs) {
  String report = "";
  report += "pn=";
  if(whoIs<10) report += "0";
  report += String(whoIs);
  if(whatIs<10) report += "00";
  else if(whatIs<100) report += "0";
  report += String(whatIs);
  report += ";";
  // расскомментируйте строчку ниже для отправки сообщения в esp 
  //Serial2.print(report);
  //#ifdef DEBUG
  Serial.print("to esp report is: "); 
  Serial.println(report); 
  //#endif
}//report
