bool checkUserExistsR(const String& firstname, const String& username, const String& password) {
  // Check if the user already exists in the database
  File file = SPIFFS.open(userDatabase, "r+");
  if (file) {
    bool userExists = false;

    // Read existing data line by line and check for existing usernames
    // Load existing JSON data
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, file);

    // Check if username already exists
    for (const JsonVariant& entry : doc.as<JsonArray>()) {
      if (entry["username"] == username) {
        userExists = true;
        // Username exists
      }
    }

    if (userExists) {
      Serial.println("Username already exists, registration failed");
      file.close();
      return false;
    } else {
      // Append user data to the CSV file
      // Create the new element
      DynamicJsonDocument doc(8192);

      // Read the file
      File addfile = SPIFFS.open(userDatabase, "r");
      deserializeJson(doc, addfile);
      addfile.close();

      // Append new element
      JsonObject obj = doc.createNestedObject();
      obj["firstname"] = firstname;
      obj["username"] = username;
      obj["password"] = password;

      // Write the file
      addfile = SPIFFS.open(userDatabase, "w");
      serializeJson(doc, addfile);
      addfile.close();


      file.close();

      //Prompt monitor
      Serial.println("User registered successfully");



      // Read the new updated database
      File filee = SPIFFS.open(userDatabase, "r");
      if (filee) {
        Serial.println("userData.json content after successful registration: ");
        while (filee.available()) {
          Serial.write(filee.read());
        }
        filee.close();
        Serial.println();
      }

      
      return true;
    }

  } else {
    Serial.println("Error opening database file for registration");
    file.close();
    return false;
  }
}
