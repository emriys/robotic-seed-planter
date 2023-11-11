void AutoMove() {
  if (plantingStatus == true) {

    // Turn off brake
    digitalWrite(PIN_BRAKE, HIGH);  // RELAY ACTIVE LOW, Driver Active HIGH
    brakeState = 0;


    /************************** FORWARD ****************************/

    // Start counting immediately wheel turns on and convert to seconds
    unsigned long wheelStart = millis() / 1000;
    unsigned long wheelNow = millis() / 1000;

    while ((wheelNow - wheelStart) < 20) {
      Serial.println("MOVING..........");

      // These relays are ACTIVE LOW
      // // Left hindWheel forward
      // digitalWrite(hindWheelsF1, LOW);
      // digitalWrite(hindWheelsB1, HIGH);

      // // Right hindWheel backward
      // digitalWrite(hindWheelsF2, LOW);
      // digitalWrite(hindWheelsB2, HIGH);

      // These relays are ACTIVE HIGH
      // Left hindWheel forward
      digitalWrite(hindWheelsF1, HIGH);
      digitalWrite(hindWheelsB1, LOW);

      // Right hindWheel backward
      digitalWrite(hindWheelsF2, HIGH);
      digitalWrite(hindWheelsB2, LOW);

      Serial.println("HIND WHEELS FORWARD..........");


      wheelNow = millis() / 1000;

      // Fail-Safe
      if (plantingStatus == 0) {
        break;
      }

      //
    }


    /********************* Reverse  *********************/

    // Start counting immediately wheel turns on and convert to seconds
    // unsigned long wheelReverseStart = millis() / 1000;
    // unsigned long wheelReverseNow = millis() / 1000;

    // while ((wheelReverseNow - wheelReverseStart) < 5) {
    //   Serial.println("MOVING..........");

    //   // Left hindWheel reverse
    //   digitalWrite(hindWheelsF1, HIGH);
    //   digitalWrite(hindWheelsB1, LOW);

    //   // Right hindwheel reverse
    //   digitalWrite(hindWheelsF2, HIGH);
    //   digitalWrite(hindWheelsB2, LOW);

    //   Serial.println("HIND WHEELS REVERSE..........");
    //   wheelReverseNow = millis() / 1000;

    //   // Fail-Safe
    //   if (plantingStatus == 0) {
    //     break;
    //   }
    // }

    // Stop the hind wheels ACTIVE LOW
    // digitalWrite(hindWheelsF1, HIGH);
    // digitalWrite(hindWheelsB1, HIGH);
    // digitalWrite(hindWheelsF2, HIGH);
    // digitalWrite(hindWheelsB2, HIGH);

    // Stop the hind wheels ACTIVE HIGH
    digitalWrite(hindWheelsF1, LOW);
    digitalWrite(hindWheelsB1, LOW);
    digitalWrite(hindWheelsF2, LOW);
    digitalWrite(hindWheelsB2, LOW);

    // Turn on brake
    digitalWrite(PIN_BRAKE, LOW);  // RELAY ACTIVE LOW, Driver Active HIGH
    brakeState = 1;

  }
}