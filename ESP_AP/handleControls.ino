void handleControls(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
  if (request->method() == HTTP_POST) {
//    Serial.println("POST METHOD: Controls DETECTED");

    // Convert the uint8_t data to a String
//    String jsonData(reinterpret_cast<char *>(data), len);
//    Serial.println("Received JSON data: " + jsonData);

    DynamicJsonDocument logger(256);

    DeserializationError error = deserializeJson(logger, data);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    ////////////////////////////////////////////////////////////////////////////////

    // Get x, y, speed, angle, and velocity values from JSON
    int x = logger["x"];
    int y = logger["y"];
    int Speed = logger["speed"];
    int Angle = logger["angle"];
    int velocity = logger["velocity"];
    //      controldata[] = {x, y, Speed, Angle};

    Serial.print("x= ");
    Serial.print(x);
    Serial.print(", y= ");
    Serial.print(y);
    Serial.print(", speed= ");
    Serial.print(Speed);
    Serial.print(", angle= ");
    Serial.print(Angle);
    Serial.print(", velocity=");
    Serial.println(velocity);
    Serial.println();

    // Store the values in the arrays
//    xValues[currentIndex] = x;
//    yValues[currentIndex] = y;
//    speedValues[currentIndex] = Speed;
//    angleValues[currentIndex] = Angle;
//    velocityValues[currentIndex] = velocity;

    // Increment the currentIndex and wrap around if it exceeds maxValues
//    currentIndex = (currentIndex + 1) % maxValues;

    // Print the collected values for debugging
    // Print the latest values for each parameter
    // Serial.print("Latest values: ");
    // Serial.print("x=");
    // Serial.print(xValues[currentIndex]);
    // Serial.print(", y=");
    // Serial.print(yValues[currentIndex]);
    // Serial.print(", speed=");
    // Serial.print(speedValues[currentIndex]);
    // Serial.print(", angle=");
    // Serial.print(angleValues[currentIndex]);
    // Serial.print(", velocity=");
    // Serial.println(velocityValues[currentIndex]);
    // Serial.println();

    // request->send(200, "text/plain", "Controls received successfully");
    //  request->send(SPIFFS, "/results.html", "text/html");
  }
}
