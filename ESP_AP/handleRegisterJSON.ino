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

    Serial.println(" ");
    Serial.print("FirstName: ");
    Serial.println(fname);
    Serial.print("Username: ");
    Serial.println(username);
    Serial.print("Password: ");
    Serial.println(password);
    Serial.print("Admin-Code: ");
    Serial.println(admin_code);

    // void databaseExists();
    // databaseExists;

    // Respond with "Registration Successful" page
    if (checkUserExistsR(fname, username, password)) {
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

// Parse the received JSON data
// StaticJsonDocument<200> jsonDocument; // Adjust the size according to your JSON data
// DeserializationError error = deserializeJson(jsonDocument, input);

//// Now you can access the JSON data using the jsonDocument object
//    const char* value = jsonDocument["key"]; // Replace "key" with your actual JSON key



//    const char* fname = doc["fname"]; // "Idris"
//    const char* username = doc["username"]; // "Malouda"
//    const char* password = doc["password"]; // "agba"
//    const char* confirm_password = doc["confirm-password"]; // "agba"
//    const char* admin_code = doc["admin-code"]; // "2345"

//      String fname = doc["firstname"].as<String>(); // "Idris"
//      String username = doc["username"].as<String>(); // "Malouda"
//      String password = doc["password"].as<String>(); // "agba"
//      int admin_code = doc["admin-code"].as<int>(); // "2345"


// Save the user data to a file
//    File file = SPIFFS.open("/users.json", "a");
//    file.print(firstname + "," + username + "," + password + "\n");
//    file.close();
//
//    // Redirect to the login page
//    server.sendHeader("Location", "/login");
//    server.send(302);
