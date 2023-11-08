void ManualMove(int x, int y, int Speed, int Angle) {
  // RELAYS ARE ACTIVE LOW

  if ((-60 < x && x < 60) && y < -150) {
    Serial.println("Forward...");
    digitalWrite(hindWheelsF1, LOW);
    digitalWrite(hindWheelsB1, HIGH);

    // // Left hindWheel forward
    // digitalWrite(hindWheelsF1, LOW);
    // digitalWrite(hindWheelsB1, HIGH);

    // // Right hindWheel backward
    // digitalWrite(hindWheelsF2, LOW);
    // digitalWrite(hindWheelsB2, HIGH);
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

  else if ((-60 < x && x < 60) && y > 150) {
    Serial.println("Reverse...");
    digitalWrite(hindWheelsF1, HIGH);
    digitalWrite(hindWheelsB1, LOW);

    // // Left hindWheel reverse
    // digitalWrite(hindWheelsF1, HIGH);
    // digitalWrite(hindWheelsB1, LOW);

    // // Right hindwheel reverse
    // digitalWrite(hindWheelsF2, HIGH);
    // digitalWrite(hindWheelsB2, LOW);
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
    digitalWrite(hindWheelsF1, HIGH);
    digitalWrite(hindWheelsB1, HIGH);

    stop();

    // Stop the hind wheels
    // digitalWrite(hindWheelsF1, HIGH);
    // digitalWrite(hindWheelsB1, HIGH);
    // digitalWrite(hindWheelsF2, HIGH);
    // digitalWrite(hindWheelsB2, HIGH);

    // Turn off the steering motor
    digitalWrite(turnRight, HIGH);
    digitalWrite(turnLeft, HIGH);

    // Serial.print("x= " + String(x));
    // Serial.print(", y= " + String(y));
    // Serial.print(", speed= " + String(Speed));
    // Serial.print(", angle= " + String(Angle));
    // Serial.println();
  }
}