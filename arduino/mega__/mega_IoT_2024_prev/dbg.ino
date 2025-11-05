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


// если время loop превышено, напечатаем его
void checkLoopTIme(uint32_t maxMs) {
  uint32_t looptime = millis() - prevMs;
  static uint32_t prevTime = 0;
  prevMs  = millis();
  if (looptime > maxMs) {
    if (looptime != prevTime) {
      prevTime = looptime;
      Serial.print("\n\n\n\t\t\t HUGE LOOP TIME: ");
      Serial.println(looptime);
      Serial.print("\n\n\n");
    }
  }
}//checkLoopTIme()


//void testPirs() {
//  if (each100ms.ready()) { // каждых 100 мс
//
//    testPIR3 = digitalRead(N3_SENS_PIR);
//    if (testPIR3 != prevtestpir3) {
//      prevtestpir3 = testPIR3;
//      if (testPIR3 && !starttestLightN3) {
//        starttestLightN3 = 1;
//        //        digitalWrite(N2_SP, ON);
//      }
//
//      Serial.print("\n\t");
//      Serial.print(millis() >> 10); // сколько ~секунд прошло
//      Serial.print("\t pir 3 = ");
//      Serial.println(testPIR3);
//    }
//
//    testPIR4 = digitalRead(N4_SENS_PIR);
//    if (testPIR4 != prevtestpir4) {
//      prevtestpir4 = testPIR4;
//      if (testPIR4) {
//        if (!starttestLightN6) {
//          starttestLightN4 = 1;
//        }
//        //        digitalWrite(N7_SP, ON);
//      }//
//
//      Serial.print("\n\t");
//      Serial.print(millis() >> 10); // сколько ~секунд прошло
//      Serial.print("\t\t pir 4 = ");
//      Serial.println(testPIR4);
//    }
//
//    testPIR6 = digitalRead(N6_SENS_PIR);
//    if (testPIR6 != prevtestpir6) {
//      prevtestpir6 = testPIR6;
//      //      if (testPIR6) {
//      //        if (!starttestLightN6) {
//      //          starttestLightN6 = 1;
//      //          each5secForN6.rst();
//      //        }
//      //        digitalWrite(N6_LED, ON);
//      //        digitalWrite(N6_SP, ON);
//      //      }
//      Serial.print("\n\t");
//      Serial.print(millis() >> 10); // сколько ~секунд прошло
//      Serial.print("\t\t\t pir 6 = ");
//      Serial.println(testPIR6);
//    }
//
//    //    testPIR7 = digitalRead(N7_SENS_PIR);
//    //    if (testPIR7 != prevtestpir7) {
//    //      prevtestpir7 = testPIR7;
//    ////      if (testPIR7 && !starttestLightN7) {
//    ////        starttestLightN7 = 1;
//    ////        each5secForN7.rst();
//    ////        digitalWrite(N7_SP, ON);
//    ////      }
//    //      Serial.print("\n\t");
//    //      Serial.print(millis() >> 10); // сколько ~секунд прошло
//    //      Serial.print("\t\t\t\t pir 7 = ");
//    //      Serial.println(testPIR7);
//    //    }
//
//    //    // выключение  спустя 5 секунд
//    //    if (each5secForN3.ready()) {
//    //      digitalWrite(N2_SP, OFF);
//    //      starttestLightN3 = 0;
//    //    }
//    //    if (each5secForN4.ready()) {
//    //      digitalWrite(N7_SP, OFF);
//    //      starttestLightN4 = 0;
//    //    }
//    //    if (each5secForN6.ready()) {
//    //      digitalWrite(N6_LED, OFF);
//    //      digitalWrite(N6_SP, OFF);
//    //      starttestLightN6 = 0;
//    //    }
//    //    if (each5secForN7.ready()) {
//    //      digitalWrite(N7_SP, OFF);
//    //      starttestLightN7 = 0;
//    //    }
//
//  }// each 100 sec
//}//testPirs()
//
//
//
