void stop() {
  //Stop wheel movement. RELAYS ARE ACTIVE LOW

  Serial.println("Stopped!....");

  // Turn on brake
  digitalWrite(PIN_BRAKE, LOW);  // RELAY ACTIVE LOW, Driver Active HIGH

  // Turn off the steering motor
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
}