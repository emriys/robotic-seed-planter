void handleCollect(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
  if (request->method() == HTTP_POST) {

    Serial.println("POST METHOD: INSTRUCTIONS DETECTED");

    // Convert the uint8_t data to a String
    String input(reinterpret_cast<char *>(data), len);
    Serial.println("Received JSON data: " + input);

    DynamicJsonDocument doc(256);

    DeserializationError error = deserializeJson(doc, data);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    cropType = doc["cropType"].as<String>();
    String soilType = doc["soilType"].as<String>();
    kernelsPerHole = doc["kernelsPerHole"].as<int>();
    farmLength = doc["farmLength"].as<float>();
    farmBreadth = doc["farmBreadth"].as<float>();
    String startingLane = doc["startingLane"].as<String>();
    String planterStartingPoint = doc["planterStartingPoint"].as<String>();

    ////////////////////////////////////////////////////////////////////////////////
    #if 1 // Set to 1 to activate or 0 to deactivate
      Serial.println(" ");
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
      Serial.println(startingLane);
      Serial.print("At Starting Point?: ");
      Serial.println(planterStartingPoint);
    #endif

    //Send initial plantingStatus to brain
    bool plantingStatusInit = true;

    //Transfer back to Websocket Client on Share
    StaticJsonDocument<1024> plant_tx;
    String payload3 = "";
    JsonObject object = plant_tx.to<JsonObject>();
    object["cropType"] = cropType;
    object["soilType"] = soilType;
    object["kernelsPerHole"] = kernelsPerHole;
    object["farmLength"] = farmLength;
    object["farmBreadth"] = farmBreadth;
    object["startingLane"] = startingLane;
    object["planterStartingPoint"] = planterStartingPoint;
    object["plantingStatus"] = plantingStatusInit;

    serializeJson(plant_tx, payload3);
    webSocketShare.broadcastTXT(payload3);

    String htmlResponse = "<html><head><script>setTimeout(function(){ window.location.href = '/dashboard'; }, 2000);</script><style>body{align-items:center;}</style></head><body><h1>Instructions sent. Planting will start soon!</h1></body></html>";
    request->send(200, "text/html", htmlResponse);
  } else {
    request->send(405);  // Method Not Allowed
  }
}