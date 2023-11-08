void stop() {
  //Stop wheel movement. RELAYS ARE ACTIVE LOW

  digitalWrite(hindWheelsF1, HIGH);
  digitalWrite(hindWheelsB1, HIGH);

  // Turn off the steering motor
  digitalWrite(turnRight, HIGH);
  digitalWrite(turnLeft, HIGH);

  // Stop the hind wheels
  digitalWrite(hindWheelsF1, HIGH);
  digitalWrite(hindWheelsB1, HIGH);
  digitalWrite(hindWheelsF2, HIGH);
  digitalWrite(hindWheelsB2, HIGH);

  digitalWrite(PIN_BRAKE, true);  // Active HIGH
}