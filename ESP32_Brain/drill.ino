void drill() {
  unsigned long drillStart = millis();
  unsigned long drillnow = millis();
  while ((drillnow - drillStart) < 3000) {
    // Serial.println("Drilling Soil.....");
    drillnow = millis();
    // Serial.println(drillnow - drillStart);
  }
  // Serial.println("Done Drilling.....");
  // Serial.println("Done Drilling.....");
  // Serial.println("Done Drilling.....");
  // Serial.println("Done Drilling.....");
  // Serial.println("Done Drilling.....");
}