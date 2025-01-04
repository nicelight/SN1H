void testPirs() {
  if (each100ms.ready()) { // каждых 100 мс
    testPIRS = digitalRead(N6_SENS_PIR);
    if (testPIRS != prevtestpirs) {
      prevtestpirs = testPIRS;
      Serial.print("\n\t");
      Serial.print(millis() >> 10); // сколько ~секунд прошло
      Serial.print("\t pir = ");
      Serial.println(testPIRS);
    }
  }
}//testPirs()
