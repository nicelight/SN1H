// отладка
void makeCOM() // получаем стринг по ком порту, отправляем на распарс
{
  if (Serial.available()) {
    delay(3);
    String  request = Serial.readString(); // важно чтобы эта переменная была как можно короче, потому и нужно передавать в серийнике меньше инфы. это помимо того что много инфы в серийнике это еще и беда с быстро заканчивающимся местом под код. в области FLASH
    //Serial.println("  polu4eno iz serial ");
    if (request != "") parceRequest(&request); // функция парсинга uart строки (т.е. полученной по серийному порту)
  }// make com port
}
  
//  распарс стринга
//старый формат команд: "arg1=arg2;"
//новый формат команд: "03-1" // 3 й пин сделать единичкой
void parceRequest(String* request) {
  //  String command = request->substring(0, request->indexOf(';') );
  String command = request->substring(0, 4);
  command.trim();
  String arg1 = "";
  String arg2 = "";

  if (command.indexOf('-') > 0) {
    arg1 = command.substring(0, 2);  // первый и второй символ команды
    //    arg2 = command.substring(command.indexOf('-') + 1, command.indexOf(';') );
    //    arg2 = command.substring(command.indexOf('-') + 1,  command.indexOf('-') + 3); //  два символа после тире
    arg2 = command.substring(3, 5); //  два символа после тире
    //отладка
    //      Serial.print(" First argument of serial request is: ");
    //      Serial.println(arg1); // тут напечатается
    //      Serial.print(" Second argument of serial request is: ");
    //      Serial.println(arg2);
    //
    int pin = arg1.toInt();
    int st = arg2.toInt();
    Serial.print("pin:");
    Serial.print(pin); // тут напечатается
    Serial.print("\tst: ");
    Serial.println(st);

    digitalWrite(pin, st);
    delay(50);

  }//else indexOf =
}//parce quest
