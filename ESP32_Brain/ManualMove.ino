void ManualMove(int x, int y, int Speed, int Angle, int brakeSet) {
  // RELAYS ARE ACTIVE LOW

  // Turn ON or OFF the brake
  // if (brakeSet == 0) {
  //     // Turn off brake
  //     digitalWrite(PIN_BRAKE, HIGH);  // RELAY ACTIVE LOW, Driver Active HIGH
  //     brakeState = brakeSet;
  //   } else {
  //     // Turn on brake
  //     digitalWrite(PIN_BRAKE, LOW);  // RELAY ACTIVE LOW, Driver Active HIGH
  //     brakeState = brakeSet;
  // }

  if ((-60 < x && x < 60) && y < -150 && brakeSet == 0) {
    Serial.println("Forward...");

    // Turn off the steering motor
    digitalWrite(turnRight, HIGH);
    digitalWrite(turnLeft, HIGH);

    // Left hindWheel forward ACTIVE LOW
    // digitalWrite(hindWheelsF1, LOW);
    // digitalWrite(hindWheelsB1, HIGH);

    // // Right hindWheel forward
    // digitalWrite(hindWheelsF2, LOW);
    // digitalWrite(hindWheelsB2, HIGH);

    // Turn off brake
    digitalWrite(PIN_BRAKE, HIGH);  // RELAY ACTIVE LOW, Driver Active HIGH

    // These relays are ACTIVE HIGH
    // Left hindWheel forward
    digitalWrite(hindWheelsF1, HIGH);
    digitalWrite(hindWheelsB1, LOW);

    // Right hindWheel forward
    digitalWrite(hindWheelsF2, HIGH);
    digitalWrite(hindWheelsB2, LOW);

  }

  else if ((-290 < x && x < -80) && y < -30) {
    Serial.println("Turning Left...");
    // Assuming CCW as Left
    digitalWrite(turnRight, HIGH);
    digitalWrite(turnLeft, LOW);


    // // Left hindWheel forward
    // digitalWrite(hindWheelsF1, LOW);
    // digitalWrite(hindWheelsB1, HIGH);

    // // Right hindWheel backward
    // digitalWrite(hindWheelsF2, LOW);
    // digitalWrite(hindWheelsB2, HIGH);
  }

  else if ((80 < x && x < 290) && y < -30) {
    Serial.println("Turning Right...");
    // Assuming CW as right
    digitalWrite(turnRight, LOW);
    digitalWrite(turnLeft, HIGH);

    // // Left hindWheel forward
    // digitalWrite(hindWheelsF1, LOW);
    // digitalWrite(hindWheelsB1, HIGH);

    // // Right hindWheel backward
    // digitalWrite(hindWheelsF2, LOW);
    // digitalWrite(hindWheelsB2, HIGH);
  }

  else if ((-60 < x && x < 60) && y > 150 && brakeSet == 0) {
    Serial.println("Reverse...");

    // Turn off brake
    digitalWrite(PIN_BRAKE, HIGH);  // RELAY ACTIVE LOW, Driver Active HIGH

    // Turn off the steering motor ACTIVE LOW
    digitalWrite(turnRight, HIGH);
    digitalWrite(turnLeft, HIGH);

    // Left hindWheel reverse ACTIVE LOW
    // digitalWrite(hindWheelsF1, HIGH);
    // digitalWrite(hindWheelsB1, LOW);

    // // Right hindwheel reverse
    // digitalWrite(hindWheelsF2, HIGH);
    // digitalWrite(hindWheelsB2, LOW);

    // Turn off brake
    digitalWrite(PIN_BRAKE, HIGH);  // RELAY ACTIVE LOW, Driver Active HIGH

    // These relays are ACTIVE HIGH
    // Left hindWheel forward
    digitalWrite(hindWheelsF1, LOW);
    digitalWrite(hindWheelsB1, HIGH);

    // Right hindWheel backward
    digitalWrite(hindWheelsF2, LOW);
    digitalWrite(hindWheelsB2, HIGH);

  }

  else if ((-290 < x && x < -80) && y > 30) {
    Serial.println("Reversing Left...");
    // Assuming CCW as Left
    digitalWrite(turnRight, HIGH);
    digitalWrite(turnLeft, LOW);
  }

  else if ((80 < x && x < 290) && y > 30) {
    Serial.println("Reversing Right...");
    // Assuming CW as right
    digitalWrite(turnRight, LOW);
    digitalWrite(turnLeft, HIGH);
  }

  else {
    Serial.println("Stationary...");

    // // Turn on brake
    // digitalWrite(PIN_BRAKE, LOW);  // RELAY ACTIVE LOW, Driver Active HIGH

    // Turn off the steering motor ACTIVE LOW
    digitalWrite(turnRight, HIGH);
    digitalWrite(turnLeft, HIGH);

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

    #if 0 // Set to 1 to activate and 0 to deactivate
    Serial.print("x= " + String(x));
    Serial.print(", y= " + String(y));
    Serial.print(", speed= " + String(Speed));
    Serial.print(", angle= " + String(Angle));
    Serial.print(", brake= " + String(brakeSet));
    Serial.println();
    #endif
  }
}