
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <SPIFFSEditor.h>
#include "SPIFFS.h"
#include "FS.h"
#include <WiFi.h>
#include "DNSServer.h"
#include "ArduinoJson.h"
#include <ESPmDNS.h>
#include <WebSocketsServer.h>

String SerialNumber = "0001";

//Default Parameters
int farmLength = 0;
int farmBreadth = 0;
int kernelsPerHole = 0;

// Parameters for planting
int distanceCovered = 500;
int currentSeedSpacing = 20;
int currentSeedDepth = 5;
int currentRowSpacing = 35;
int currentKernelsPerHole = kernelsPerHole;

// Get x, y, speed, angle, and velocity values from controls
int x = 0;
int y = 0;
int Speed = 0;
int Angle = 0;
int velocity = 0;


// User database in JSON format stored in SPIFFS (userData.json)
const char *userDatabase = "/userData.json";

int currentIndex = 0;

unsigned long previousMillis = 0;

// Index to keep track of the planter's current planting status
int plantingStatus = 0;

// JSON document to store parameter data for transfer to client
StaticJsonDocument<1024> parameter_tx;

// JSON document to receive parameter data from client controller
StaticJsonDocument<1024> parameter_rx;

// JSON object to store user data
StaticJsonDocument<1024> userData;

// char jsonDatabase[128];

//Timer Interval
unsigned long now;


// Storage for values to update to page
char jsonOutput[256];
//Buffer holder for incoming messages
char msg_buf[64];

// Set desired Access Point credentials.
const char *ssid = "Planter Bot";
// const char *password = "yourPassword";
// const IPAddress apIP(192, 168, 4, 1);
// const IPAddress netMsk(255, 255, 255, 0);


// Create AsyncWebServer object on port of your choice default=80
AsyncWebServer server(80);
DNSServer dnsServer;
WebSocketsServer webSocket = WebSocketsServer(81);  //Socket for dashboard updates. Default port for websocket is 81.
WebSocketsServer webSocketControl = WebSocketsServer(100); //Socket for control values

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Page Not Found");
}

//===================================================

//CAPTIVE PORTAL HANDLER

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request) {
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  }
};

//===================================================

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(" ");
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    while (1)
      ;
    // return;
  } else {
    Serial.println("SPIFFS Initialized...");
  }

  ///////////------SPIFFS FORMATTING-------//////////////////

  //Clear SPIFFS incase of error
  // SPIFFS.format();
  // Serial.println("SPIFFS Formatted...");

  ///////////////////////////////////////////////////////////

  // Print the content of "userData.json" after successful registration
  File file = SPIFFS.open(userDatabase, "r");
  if (file) {
    Serial.println("userData.json content upon restart:");
    while (file.available()) {
      Serial.write(file.read());
    }
    file.close();
    Serial.println();
    Serial.println("File read!");
    Serial.println();
  } else {
    Serial.println("No database file available");
  }

  //  void handleRegister();
  void databaseExists();
  delay(1000);
  databaseExists;

  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the Access Point to be open.
  WiFi.softAP(ssid);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);


  // Start the mDNS responder
  if (MDNS.begin("PlanterBot")) {
    Serial.println("mDNS responder started");
  }

  // server.serveStatic("/", SPIFFS, "/");

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (SPIFFS.exists("/index.html")) {
      request->send(SPIFFS, "/index.html", "text/html");
    } else {
      notFound(request);
    }
  });

  // Route for login
  server.on("/login", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (SPIFFS.exists("/login.html")) {
      request->send(SPIFFS, "/login.html", "text/html");
    } else {
      notFound(request);
    }
  });

  // Route to check login data
  server.on("/check", HTTP_POST, onRequest, onFileUpload, handleLogin);

  // Route for register
  server.on("/register", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (SPIFFS.exists("/register.html")) {
      request->send(SPIFFS, "/register.html", "text/html");
    } else {
      notFound(request);
    }
  });

  //Route for registration check
  server.on("/add", HTTP_POST, onRequest, onFileUpload, handleRegisterJSON);

  //Route for homepage
  server.on("/homepage", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (SPIFFS.exists("/homepage.html")) {
      request->send(SPIFFS, "/homepage.html", "text/html");
    } else {
      notFound(request);
    }
  });

  //Route for dashboard
  server.on("/dashboard", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (SPIFFS.exists("/dashboard.html")) {
      request->send(SPIFFS, "/dashboard.html", "text/html");
    } else {
      notFound(request);
    }
  });

  // Route for controller
  server.on("/controller", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (plantingStatus == 0) {
      if (SPIFFS.exists("/controller.html")) {
        request->send(SPIFFS, "/controller.html", "text/html");
      } else {
        notFound(request);
      }
    } else {
      String htmlResponse = "<html><head><script>setTimeout(function(){ window.location.href = '/dashboard'; }, 4000);</script><style>body{align-items:center;}</style></head><body><h1>Auto Planting Process currently running, <br> Manual Control NOT ALLOWED!.</h1></body></html>";
      request->send(200, "text/html", htmlResponse);
    }
  });

  // Route for configure
  server.on("/configure", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (plantingStatus == 0) {
      if (SPIFFS.exists("/configure.html")) {
        request->send(SPIFFS, "/configure.html", "text/html");
      } else {
        notFound(request);
      }
    } else {
      String htmlResponse = "<html><head><script>setTimeout(function(){ window.location.href = '/dashboard'; }, 4000);</script><style>body{align-items:center;}</style></head><body><h1>Planting Process currently running! Stop current process before reconfiguring.</h1></body></html>";
      request->send(200, "text/html", htmlResponse);
    }
  });

  //Route to collect planting parameters
  server.on("/collect", HTTP_POST, onRequest, onFileUpload, handleCollect);

  //Route to stop planting
  server.on("/stopplanting", HTTP_GET, handleStop);


  /////////////////------VIEW DATABSE---------///////////////////////////

  ///////////////////////////////////////////////////////////////////////


  server.onNotFound(notFound);

  // Captive portal-like behavior
  dnsServer.start(53, "*", myIP);
  // server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);  //only when requested from AP

  // Start server
  server.begin();
  webSocket.begin();
  webSocketControl.begin();
  webSocket.onEvent(onWebSocketEvent);
  webSocketControl.onEvent(onWebSocketEventControl);

  Serial.println("Server successfully started");
  Serial.println("....");
}


void loop() {
  // put your main code here, to run repeatedly:
  dnsServer.processNextRequest();
  webSocket.loop();
  webSocketControl.loop();
  // onWebSocketEventControl();

  now = millis();
  /************** WebSocket Code to update all clients on dashboard *************/
  if (now - previousMillis > 1000) {
    //Send data to client
    String jsonString = "";
    JsonObject object = parameter_tx.to<JsonObject>();
    object["distanceCovered"] = distanceCovered;
    object["currentSeedSpacing"] = currentSeedSpacing;
    object["currentSeedDepth"] = random(500);  //currentSeedDepth;
    object["currentRowSpacing"] = currentRowSpacing;
    object["currentKernelsPerHole"] = kernelsPerHole;
    object["farmLength"] = random(100);  //farmLength;
    object["farmBreadth"] = farmBreadth;
    object["SerialNumber"] = SerialNumber;
    object["planterStatus"] = plantingStatus;
    if (plantingStatus == 0) {
      object["dynamicbutton"] = "Start Planting";
    } else {
      object["dynamicbutton"] = "Stop Planting";
    }
    serializeJson(parameter_tx, jsonString);
    // Serial.println(jsonString);
    webSocket.broadcastTXT(jsonString);
    previousMillis = now;
  }
}



// Function to handle requests
void databaseExists();
void handleRegisterJSON();
void handleLogin();
void handleCollect();


/////////------CREATE DATABASE--------////////////////////

bool createDatabaseFile() {
  File file = SPIFFS.open(userDatabase, "w");
  if (!file) {
    return false;
  }

  // Initial array structure
  file.println("[]");

  // Create an empty JSON array
  // StaticJsonDocument<1024> doc;
  // JsonArray array = doc.to<JsonArray>();
  // serializeJson(doc, file);


  file.close();
  return true;
}

///////////////////////////////////////////////////////////


void handleStop(AsyncWebServerRequest *request) {
  if (plantingStatus == 1) {
    plantingStatus = 0;
    String htmlResponse = "<html><head><script>setTimeout(function(){ window.location.href = '/dashboard'; }, 3000);</script><style>body{align-items:center;}</style></head><body><h1>Planting Process Stopped!</h1></body></html>";
    request->send(200, "text/html", htmlResponse);
  } else {
    String htmlResponse = "<html><head><script>setTimeout(function(){ window.location.href = '/homepage'; }, 4000);</script><style>body{align-items:center;}</style></head><body><h1>Planting Process Currently Not Running. <br> <br> Go to Planting Parameters, To Start Planting!</h1></body></html>";
    request->send(200, "text/html", htmlResponse);
  }
}

void onWebSocketEvent(uint8_t client_num, WStype_t type, uint8_t *payload, size_t length) {
  // Figure out the Websocket type
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("%[u] Disconnected!\n", client_num);
      break;

    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(client_num);
        Serial.printf("[%u] Connection from ", client_num);
        Serial.println(ip.toString());
      }
      break;

    case WStype_TEXT:
      Serial.printf("[%u] Received text: %s\n", client_num, payload);

      /************** WebSocket Code to update individual clients as requested *************/

      if (strcmp((char *)payload, "SerialNumber") == 0) {
        sprintf(msg_buf, "%s", SerialNumber);
        Serial.printf("Sending to [%u]: %s\n", client_num, msg_buf);
        webSocket.sendTXT(client_num, msg_buf);
      }

      if (strcmp((char *)payload, "getPlanterStatus") == 0) {
        if (now - previousMillis > 1000) {
          //Send data to client
          String jsonString = "";
          JsonObject object = parameter_tx.to<JsonObject>();
          object["distanceCovered"] = distanceCovered;
          object["currentSeedSpacing"] = currentSeedSpacing;
          object["currentSeedDepth"] = random(500);
          object["currentRowSpacing"] = currentRowSpacing;
          object["currentKernelsPerHole"] = kernelsPerHole;
          object["farmLength"] = random(100);
          object["farmBreadth"] = farmBreadth;
          object["SerialNumber"] = SerialNumber;
          object["planterStatus"] = plantingStatus;
          if (plantingStatus == 0) {
            object["dynamicbutton"] = "Start Planting";
          } else {
            object["dynamicbutton"] = "Stop Planting";
          }
          serializeJson(parameter_tx, jsonString);
          Serial.println(jsonString);
          // webSocket.broadcastTXT(jsonString);
          webSocket.sendTXT(client_num, jsonString);
          previousMillis = now;
        }
      }

      else {
        Serial.println("[%u] Message not recognized!");
      }
      break;

    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

void onWebSocketEventControl(uint8_t client_num, WStype_t type, uint8_t *payload, size_t length) {
  // Figure out the Websocket type
  switch (type) {
    case WStype_DISCONNECTED:
      // Serial.printf("%[u] Disconnected!\n", client_num);
      Serial.println("Client Disconnected");
      break;

    case WStype_CONNECTED:
      {
        // IPAddress ip = webSocket.remoteIP(client_num);
        // Serial.printf("[%u] Connection from ", client_num);
        // Serial.println(ip.toString());
        Serial.println("Client Connected");
      }
      break;

    case WStype_TEXT:

      /************** WebSocket Code to update collect controller values *************/

      DeserializationError error = deserializeJson(parameter_rx, payload);

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      } else {
        // Get x, y, speed, angle, and velocity values from JSON
        x = parameter_rx["x"].as<int>();
        y = parameter_rx["y"].as<int>();
        Speed = parameter_rx["speed"].as<int>();
        Angle = parameter_rx["angle"].as<int>();
        velocity = parameter_rx["velocity"].as<int>();

        Serial.print("x= " + String(x));
        Serial.print(", y= " + String(y));
        Serial.print(", speed= " + String(Speed));
        Serial.print(", angle= " + String(Angle));
        Serial.print(", velocity=" + String(velocity));
        Serial.println();
      }
      break;
  }
}

void onRequest(AsyncWebServerRequest *request) {
  // dummy callback function for handling params, etc.
  return;
}

void onFileUpload(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
  // dummy callback function signature, not in used in our code
  return;
}
