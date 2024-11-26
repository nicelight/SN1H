 #define LAMP1 A7
#define LAMP2 A7
#define  LEDRGBWSTRIP A7
#define RELAY_ON 0
#define RELAY_OFF 1

byte statelamp1 = 0, statelamp2 = 0, dim = 0, ledstatergbwstrip = 0 ;

byte mode_sm = 0; //автомат работы основной программы ( его нет в примере)


///// функция парсинга команд из серийноого порта
// функцию желательно прописывать до функции setup()
// но можно и в конце ( после функции loop). просто если вконце, у меня иногда не компилируется.
//функция объектно ориентированная с указателями ( *&) не пугайтесь их, просто пользуйтесь как есть.
//
//
// вызываться она будет ниже, в лупе.
//старый формат команд: "arg1=arg2;"
//новый формат команд: "23-1" // 23 й пин сделать единичкой

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


void setup(void) {
  Serial.begin(115200); // скорость ограничиваю ибо под квесты Fantasmia прогу адаптирую
  delay(10);
  for (int i = 2; i <= 21; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, 1);
  }
  for (int i = 22; i <= 31; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, 1);
  }
  for (int i = 38; i <= 53; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, 1);
  }
  for (int i = 32; i < 38; i++) pinMode(i, INPUT_PULLUP);
  for (int i = 54; i < 70; i++) pinMode(i, INPUT_PULLUP);

    digitalWrite(45, 0);// N4 SP1
delay(100);
digitalWrite(46, 0);// N4 SP2
delay(100);
    digitalWrite(47, 1); // N4 T1
delay(100);
    digitalWrite(48, 1);// N4 T2
delay(100);
    digitalWrite(38, 1);// N2 T
delay(100);
    digitalWrite(39 , 0);// N2 SP
delay(100);
    digitalWrite(40 , 0);// N1 S1
delay(100);
    digitalWrite(41 , 0);// N1 S2
delay(100);
    digitalWrite(5 , 0);// N5 S1
delay(100);
    digitalWrite(4 , 0);// N5 S2
delay(100);


}//setup

void loop(void) {

  //managing from com port
  if (Serial.available()) {
    delay(3);
    String  request = Serial.readString(); // важно чтобы эта переменная была как можно короче, потому и нужно передавать в серийнике меньше инфы. это помимо того что много инфы в серийнике это еще и беда с быстро заканчивающимся местом под код. в области FLASH
    //Serial.println("  polu4eno iz serial ");
    if (request != "") parceRequest(&request); // функция парсинга uart строки (т.е. полученной по серийному порту)
  }// make com port

  delay(1);

  /// ОСНОВНОЙ КОД ПРОГРАММЫ ЗДЕСЬ

}//loop
