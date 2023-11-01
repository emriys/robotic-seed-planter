void dropSeed() {
  unsigned long dropStart = millis();
  unsigned long dropnow = millis();
  while ((dropnow - dropStart) < 3000) {
    // Serial.println("Dropping Seeds.....");
    dropnow = millis();
    // Serial.println(dropnow - dropStart);
  }
//   Serial.println("Done Seeding.....");
//   Serial.println("Done Seeding.....");
//   Serial.println("Done Seeding.....");
//   Serial.println("Done Seeding.....");
//   Serial.println("Done Seeding.....");
}