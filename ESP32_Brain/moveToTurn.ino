void moveToTurn() {
  // Move forward slightly before turning to create allowance
  // and accurately position the planter on the next planting row

  /************************** FORWARD ****************************/

    // Start counting immediately wheel turns on and convert to seconds
    unsigned long wheelStart = millis() / 1000;
    unsigned long wheelNow = millis() / 1000;

    while ((wheelNow - wheelStart) < 5) {
      Serial.println("MOVING TO TURN..........");
      digitalWrite(hindWheelsF1, LOW);
      digitalWrite(hindWheelsB1, HIGH);

      // // Left hindWheel forward
      // digitalWrite(hindWheelsF1, LOW);
      // digitalWrite(hindWheelsB1, HIGH);

      // // Right hindWheel backward
      // digitalWrite(hindWheelsF2, LOW);
      // digitalWrite(hindWheelsB2, HIGH);

      Serial.println("HIND WHEELS FORWARD TO TURN..........");


      wheelNow = millis() / 1000;

      // Fail-Safe
      if (plantingStatus == 0) {
        break;
      }

      //
    }
}