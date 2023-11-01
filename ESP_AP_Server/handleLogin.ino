void handleLogin(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
  if (request->method() == HTTP_POST) {
    Serial.println("POST METHOD: LOGIN DETECTED");

    // Convert the uint8_t data to a String
    String input(reinterpret_cast<char *>(data), len);
    Serial.println("Received JSON data: " + input);

    DynamicJsonDocument log(256);

    DeserializationError error = deserializeJson(log, data);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    String username = log["username"].as<String>();  // "Malouda"
    String password = log["password"].as<String>();  // "agba"

    Serial.println(" ");
    Serial.print("Username: ");
    Serial.println(username);
    Serial.print("Password: ");
    Serial.println(password);

    bool userpassMatch = false;
    File file = SPIFFS.open(userDatabase, "r");
    if (!file) {
      Serial.println("Database file not found! Internal Server Found.");
      request->send(500, "text/html");
    } else {
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, file);
      for (const JsonVariant &entry : doc.as<JsonArray>()) {
        if (entry["username"] == username && entry["password"] == password) {
          userpassMatch = true;  // Username and password match
        }
      }

      if (userpassMatch) {
        String htmlResponse = "<html><head><script>setTimeout(function(){ window.location.href = '/homepage'; }, 2000);</script><style>body{align-items:center;}</style></head><body><h1>Login Successful</h1></body></html>";
        Serial.println("Login Successful!");
        request->send(200, "text/html", htmlResponse);
      } else {
        //Respond with a "Username or password incorrect" alert
        String errorMessage = "Username or password incorect. Please try again!";
        AsyncWebServerResponse *response = request->beginResponse(400, "text", errorMessage);
        request->send(response);
        Serial.println("Username or password incorrect, login failed");
      }
    }
  } else {
    request->send(405);  // Method Not Allowed
  }
}
