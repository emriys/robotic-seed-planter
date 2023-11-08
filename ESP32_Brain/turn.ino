void turn() {
  unsigned long turnStart = millis();
  unsigned long turnnow = millis();
  while ((turnnow - turnStart) < 5000) {
    Serial.println("Turning Right.....");

    // Assuming CW as right
    digitalWrite(turnRight, LOW);
    digitalWrite(turnLeft, HIGH);

    turnnow = millis();

    // Fail-Safe
    if (plantingStatus == 0) {
      break;
    }
  }

  // Turn off the steering motor
  digitalWrite(turnLeft, HIGH);
  digitalWrite(turnRight, HIGH);
}