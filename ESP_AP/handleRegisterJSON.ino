void handleRegisterJSON(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
  if (request->method() == HTTP_POST) {
    Serial.println("POST METHOD: REGISTRATION DETECTED");

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

    String fname = doc["firstname"].as<String>();    // "Idris"
    String username = doc["username"].as<String>();  // "Malouda"
    String password = doc["password"].as<String>();  // "agba"
    int admin_code = doc["admincode"].as<int>();     // "2345"

    #if 1 // Set to 1 to activate or 0 to deactivate
      Serial.println(" ");
      Serial.print("FirstName: ");
      Serial.println(fname);
      Serial.print("Username: ");
      Serial.println(username);
      Serial.print("Password: ");
      Serial.println(password);
      Serial.print("Admin-Code: ");
      Serial.println(admin_code);
    #endif

    if (adminCode == "") {
      adminCode = doc["admincode"].as<String>();
    }

    // Changing admin code to string for comparison purpose
    String admincode_s = doc["admincode"].as<String>();

    // Respond with "Registration Successful" page
    if (checkUserExistsR(fname, username, password, admincode_s)) {
      String htmlResponse = "<html><head><script>setTimeout(function(){ window.location.href = '/login'; }, 2000);</script><style>h1{align-items:center;}</style></head><body><h1>Registration Successful</h1></body></html>";
      request->send(200, "text/html", htmlResponse);
    } else {
      //Respond with a "Username Already Exists" alert
      String errorMessage = "Username Already Exists. Please retry!";
      AsyncWebServerResponse *response = request->beginResponse(400, "application/json", errorMessage);
      request->send(response);
    }

  } else {
    Serial.println("POST request does not have a body.");
    request->send(405);  // Method Not Allowed
  }
}
