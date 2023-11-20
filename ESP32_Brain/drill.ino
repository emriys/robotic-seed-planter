void drill() {
  if (plantingStatus == true) {

    // Deploy drill motor (ACTIVE LOW)
    unsigned long drillStart = millis();
    unsigned long drillnow = millis();
    while ((drillnow - drillStart) < 10000 && plantingStatus == true) {
      Serial.println("Drilling Soil.....");
      // // Turn on drills
      // digitalWrite(drillsF1, LOW);
      // digitalWrite(drillsB1, HIGH);

      // Turn on drills
      digitalWrite(drillsF1, HIGH);
      digitalWrite(drillsB1, LOW);
      Serial.println("Drills ON .....");


      // Deploy drill motors (CW) (ACTIVE LOW)
      digitalWrite(drillDeploy, LOW);
      digitalWrite(drillRetract, HIGH);
      drillnow = millis();

      // Fail-Safe
      if (plantingStatus == 0) {
        break;
      }
    }

    // Turn OFF drills
    digitalWrite(drillsF1, HIGH);
    digitalWrite(drillsB1, HIGH);
    
    // Turn OFF the drill retractor (ACTIVE LOW)
    digitalWrite(drillDeploy, HIGH);
    digitalWrite(drillRetract, HIGH);
    Serial.println("Retractor OFF .....");
    delay(2000);


    // Retract drill motor and turn them off
    unsigned long retractStart = millis();
    unsigned long retractnow = millis();
    while ((retractnow - retractStart) < 10000 && plantingStatus == true) {
      Serial.println("Retracting .....");

      // Reverse drills
      digitalWrite(drillsF1, HIGH);
      digitalWrite(drillsB1, HIGH);
      Serial.println("Drills OFF .....");


      //Retract drill motors ACTIVE LOW
      digitalWrite(drillDeploy, HIGH);
      digitalWrite(drillRetract, LOW);
      retractnow = millis();

      // Fail-Safe
      if (plantingStatus == 0) {
        break;
      }
    }

    // Turn OFF drills
    digitalWrite(drillsF1, HIGH);
    digitalWrite(drillsB1, HIGH);

    // Turn OFF the drill retractor (ACTIVE LOW)
    digitalWrite(drillDeploy, HIGH);
    digitalWrite(drillRetract, HIGH);
    Serial.println("Retractor OFF .....");


    Serial.println("Done Drilling.....");
  }
}