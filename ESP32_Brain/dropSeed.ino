void dropSeed() {
  if (plantingStatus == true) {

    // Turn on seed motor
    unsigned long dropStart = millis();
    unsigned long dropnow = millis();
    while ((dropnow - dropStart) < 5000) {
      Serial.println("Dropping Seeds.....");
      digitalWrite(seedMotor, LOW);
      dropnow = millis();

      // Fail-Safe
      if (plantingStatus == 0) {
        break;
      }
    }

    // Turn off seed motor
    digitalWrite(seedMotor, HIGH);
    Serial.println("Done Seeding.....");
  }
}