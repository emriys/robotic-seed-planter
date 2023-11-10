void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {

  if (type == WStype_TEXT) {
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, payload);

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }

    // Get x, y, speed, angle, and velocity values from JSON
    x = doc["x"].as<int>();
    y = doc["y"].as<int>();
    Speed = doc["speed"].as<int>();
    Angle = doc["angle"].as<int>();

    cropType = doc["cropType"].as<String>();
    soilType = doc["soilType"].as<String>();
    kernelsPerHole = doc["kernelsPerHole"].as<int>();
    farmLength = doc["farmLength"].as<float>();
    farmBreadth = doc["farmBreadth"].as<float>();
    stLane = doc["startingLane"].as<String>();
    planterStartingPoint = doc["planterStartingPoint"].as<String>();

    plantingStatus = doc["plantingStatus"].as<bool>();

    #if 0 // Set to 1 to activate
      Serial.print("Planting Status1: ");
      Serial.println(plantingStatus);
      Serial.println(" ////");
    #endif

    // Check type of information received
    if (doc["cropType"].isNull() && doc["plantingStatus"].isNull()) {
      #if 0  // Set to 1 to activate
        Serial.print("x= " + String(x));
        Serial.print(", y= " + String(y));
        Serial.print(", speed= " + String(Speed));
        Serial.print(", angle= " + String(Angle));
        Serial.println(" ////");
      #endif
      ManualMove(x, y, Speed, Angle);

    } else if (doc["cropType"].isNull() && doc["x"].isNull()) {
        #if 1  // Set to 1 to activate or 0 to deactivate
          Serial.println(" ////");
          Serial.print("Planting Status: ");
          Serial.println(plantingStatus);
          Serial.println(" ////");
          if (plantingStatus == 0) {
            acknowledge = 0;
          }
        #endif
    } else {
      #if 1 // Set to 1 to activate or 0 to deactivate
        Serial.println(" ");
        Serial.println("/////////////////////////////////////////////////////");
        Serial.print("Crop: ");
        Serial.println(cropType);
        Serial.print("Soil: ");
        Serial.println(soilType);
        Serial.print("KernelsPerHole: ");
        Serial.println(kernelsPerHole);
        Serial.print("FarmLength: ");
        Serial.println(farmLength);
        Serial.print("FarmBreadth: ");
        Serial.println(farmBreadth);
        Serial.print("StartingLane: ");
        Serial.println(stLane);
        Serial.print("At Starting Point?: ");
        Serial.println(planterStartingPoint);
        Serial.println(" ");

        Serial.print("Planting Status: ");
        Serial.println(plantingStatus);
        Serial.println("/////////////////////////////////////////////////////");
      #endif
      acknowledge = 1;
      plantingReset == 0;

      /************************ MANUAL TURNING CONFIRMATION ************************/
      // Check if planter is starting new planting session or resuming after manual turning
      if (nrc >= nrp) {
        nhc = nhp = nrc = nrp = totaldistCovR = totaldistCovM = tdc = 0;
      } else {
        Serial.println("Resuming Planting...");
      }
      /****************************************************************************/


      // Send acknowledgement to the client
      StaticJsonDocument<200> doc;
      String payload1 = "";
      JsonObject object = doc.to<JsonObject>();
      object["received"] = acknowledge;
      serializeJson(doc, payload1);
      webSocket.sendTXT(payload1);

      unsigned long sendstart = millis();
      unsigned long sendnow = millis();

      while ((sendnow - sendstart) < 10000) {
        Serial.println("Preparing To Plant...");
        // Update sendnow
        sendnow = millis();
      }
    }

    webSocket.sendTXT("{\"status\":\"OK\"}");
  }
}