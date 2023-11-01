void databaseExists(AsyncWebServerRequest *request) {
  // Check if "userData.json" exists
  if (!SPIFFS.exists(userDatabase)) {
    Serial.println("Database file does not exist, creating file...");
    if (createDatabaseFile()) {
      Serial.println("Database file created successfully.");
      // registerUser();
    } else {
      Serial.println("Failed to create database file.");
    }

  } else {
    // If it exists, read the file and parse the JSON data into the userData object
    Serial.println("Database file exists, users can register or login.");
  }


  // Print the content of "userData.json" after successful creation
  File file = SPIFFS.open(userDatabase, "r");
  if (file) {
    Serial.println("userData.json content after successful creation:");
    while (file.available()) {
      Serial.write(file.read());
    }
    file.close();
    Serial.println();
  }

}
