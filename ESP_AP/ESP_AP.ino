
// Include required libraries
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
String adminCode = "";

//Default Parameters
float farmLength = 0;
float farmBreadth = 0;
int kernelsPerHole = 0;
float farmArea = 0;

// Parameters for planting
float totaldistanceCovered = 0;
int currentSeedSpacing = 0;
int currentSeedDepth = 0;
int currentRowSpacing = 0;
int currentKernelsPerHole = 0;
int holesDug = 0;
int rowsCompleted = 0;
int rowsPossible = 0;
int holesPossible = 0;

String cropType = " ";
String soilType = " ";
String stLane = " ";  // Planter Starting Lane (along Length or Breadth of Farm)
String needturn = " ";

// Get x, y, speed, angle, and brake values from controls
int x = 0;
int y = 0;
int Speed = 0;
int Angle = 0;
int brakeValue = 0;

// User database in JSON format stored in SPIFFS (userData.json)
const char *userDatabase = "/userData.json";

// State of brake to update server
int brakeState;

unsigned long previousMillis = 0;

// Boolean to keep track of the planter's current planting status
bool plantingStatus;

// JSON document to store parameter data for transfer to webpage client
StaticJsonDocument<1024> parameter_tx;

// JSON document to receive control parameter data from client webpage controller
StaticJsonDocument<1024> parameter_rx;

// JSON document to receive shared parameter data from brain
StaticJsonDocument<1024> share_rx;

// JSON document to share details between ESPs
StaticJsonDocument<1024> return_tx;

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
WebSocketsServer webSocket = WebSocketsServer(81);          //Socket for dashboard updates. Default port for websocket is 81.
WebSocketsServer webSocketControl = WebSocketsServer(100);  //Socket for control values
WebSocketsServer webSocketShare = WebSocketsServer(120);    //Socket for sharing details between ESPs

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


/************** SET-UP ****************************************/

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
  // WiFi.mode(WIFI_AP_STA);
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
    if (plantingStatus == false) {
      if (SPIFFS.exists("/controller.html")) {
        request->send(SPIFFS, "/controller.html", "text/html");
      } else {
        notFound(request);
      }
    } else {
      String htmlResponse = "<html><head><script>setTimeout(function(){ window.location.href = '/dashboard'; }, 4000);</script><style>body{align-items:center;}</style></head><body><h1>Auto Planting Process currently running, <br> Manual Control NOT ALLOWED!</h1></body></html>";
      request->send(200, "text/html", htmlResponse);
    }
  });

  // Route for configure
  server.on("/configure", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (plantingStatus == false) {
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

  //Route for share
  server.on("/share", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (SPIFFS.exists("/share.html")) {
      request->send(SPIFFS, "/share.html", "text/html");
    } else {
      notFound(request);
    }
  });

    //Route for retsrting server
  server.on("/restartserver", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (SPIFFS.exists("/dashboard.html")) {
      String htmlResponse = "<html><head><script>setTimeout(function(){ window.location.href = '/homepage'; }, 10000);</script><style>body{align-items:center;}</style></head><body><h1>Restarting Server...wait 10 seconds</h1></body></html>";
      request->send(200, "text/html", htmlResponse);
      ESP.restart();
    } else {
      notFound(request);
    }
  });

  //Route to stop planting
  server.on("/stopplanting", HTTP_GET, handleStop);


  /////////////////------VIEW DATABSE---------///////////////////////////

  ///////////////////////////////////////////////////////////////////////


  server.onNotFound(notFound);

  // Captive portal-like behavior
  dnsServer.start(53, "*", myIP);
  // server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);  //only when requested from AP

  // Start server and websockets
  server.begin();
  webSocket.begin();
  webSocketControl.begin();
  webSocketShare.begin();
  webSocket.onEvent(onWebSocketEvent);
  webSocketControl.onEvent(onWebSocketEventControl);
  webSocketShare.onEvent(onWebSocketEventShare);

  Serial.println("Server successfully started");
  Serial.println("....");
}


/************** LOOP ****************************************/

void loop() {
  // put your main code here, to run repeatedly:
  dnsServer.processNextRequest();
  webSocket.loop();
  webSocketControl.loop();
  webSocketShare.loop();
  // onWebSocketEventControl();

  now = millis();

  /************** WebSocket Code to update all clients on dashboard *************/
  // Update every 2 seconds
  if (now - previousMillis > 2000) {
    //Send data to client
    String jsonString = "";
    JsonObject object = parameter_tx.to<JsonObject>();
    object["cropType"] = cropType;
    object["soilType"] = soilType;
    object["stLane"] = stLane;
    object["distanceCovered"] = totaldistanceCovered;
    object["currentSeedSpacing"] = currentSeedSpacing;
    object["currentSeedDepth"] = currentSeedDepth;  //currentSeedDepth;
    object["currentRowSpacing"] = currentRowSpacing;
    object["currentKernelsPerHole"] = currentKernelsPerHole;
    object["farmLength"] = farmLength;  //farmLength;
    object["farmBreadth"] = farmBreadth;
    object["farmArea"] = farmArea;
    object["SerialNumber"] = SerialNumber;
    object["planterStatus"] = plantingStatus;
    object["holesDug"] = holesDug;
    object["rowsCompleted"] = rowsCompleted;
    object["holesPossible"] = holesPossible;
    object["rowsPossible"] = rowsPossible;
    object["turn"] = needturn;


    //Dashboard "Start/Stop/Resume" dynamic button display logic
    if ((0 < holesDug) && (holesDug < holesPossible) && (rowsCompleted > 0) && (rowsCompleted < rowsPossible) && (plantingStatus == false)) {
      object["dynamicbutton"] = "Resume Planting";
      object["turn"] = "Row completed, please turn manually";
    } else if (plantingStatus == false) {
      object["dynamicbutton"] = "Start Planting";
      object["turn"] = " ";
    } else {
      object["dynamicbutton"] = "Stop Planting";
      object["turn"] = " ";
    }


    //Signup "Set/Enter" admincode logic
    if (adminCode == "") {
      object["codestatus"] = "Set";
    } else {
      object["codestatus"] = "Enter";
    }

    serializeJson(parameter_tx, jsonString);
    // Serial.println(jsonString);
    webSocket.broadcastTXT(jsonString);
    previousMillis = now;
  }
}


/************** FUNCTIONS ****************************************/

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
  if (plantingStatus == true) {
    plantingStatus = false;
    String htmlResponse = "<html><head><script>setTimeout(function(){ window.location.href = '/dashboard'; }, 3000);</script><style>body{align-items:center;}</style></head><body><h1>Planting Process Stopped!</h1></body></html>";
    request->send(200, "text/html", htmlResponse);

    //Transfer back to Websocket Client on Share
    String payload3 = "";
    JsonObject object = return_tx.to<JsonObject>();
    object["plantingStatus"] = plantingStatus;
    Serial.println((String) "PlantingStatus: " + plantingStatus);

    serializeJson(return_tx, payload3);

    webSocketShare.broadcastTXT(payload3);
    // webSocketShare.sendTXT(payload);
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
      Serial.println("Client Disconnected From Control");
      break;

    case WStype_CONNECTED:
      {
        // IPAddress ip = webSocket.remoteIP(client_num);
        // Serial.printf("[%u] Connection from ", client_num);
        // Serial.println(ip.toString());
        Serial.println("Client Connected To Control");
      }
      break;

    case WStype_TEXT:

      /************** WebSocket Code to update collect controller values *************/
      // Serial.println(payload.as<String>());

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
        brakeValue = parameter_rx["brakeSet"].as<int>();

        #if 0  // // Set to 1 to activate or 0 to deactivate
          Serial.print("x= " + String(x));
          Serial.print(", y= " + String(y));
          Serial.print(", speed= " + String(Speed));
          Serial.print(", angle= " + String(Angle));
          Serial.print(", brake= " + String(brakeValue));
        #endif

        if ((-60 < x && x < 60) && y < -150 && brakeValue == 0) {
          Serial.println("Forward...");
        } else if ((-290 < x && x < -80) && y < -30) {
          Serial.println("Turning Left...");
        } else if ((80 < x && x < 290) && y < -30) {
          Serial.println("Turning Right...");
        } else if ((-60 < x && x < 60) && y > 150 && brakeValue == 0) {
          Serial.println("Reverse...");
        } else if ((-290 < x && x < -80) && y > 30) {
          Serial.println("Reversing Left...");
        } else if ((80 < x && x < 290) && y > 30) {
          Serial.println("Reversing Right...");
        } else {
          Serial.print("x= " + String(x));
          Serial.print(", y= " + String(y));
          Serial.print(", speed= " + String(Speed));
          Serial.print(", angle= " + String(Angle));
          Serial.print(", brake= " + String(brakeValue));
          Serial.println();
        }

        // Serial.println();

        //Transfer back to Websocket Client on Share
        String payload2 = "";
        JsonObject object = return_tx.to<JsonObject>();
        object["x"] = x;
        object["y"] = y;
        object["speed"] = Speed;
        object["angle"] = Angle;
        object["brakeValue"] = brakeValue;

        serializeJson(return_tx, payload2);

        // webSocketControl.broadcastTXT(payload2);
        webSocketShare.broadcastTXT(payload2);
      }
      break;
  }
}

void onWebSocketEventShare(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {

  if (type == WStype_TEXT) {
    // processs any returned data
    // Serial.printf("payload [%u]: %s\n", num, payload);
  }

  //Figure out the Websocket type
  switch (type) {
    case WStype_DISCONNECTED:
      // Serial.printf("%[u] Disconnected!\n", client_num);
      Serial.println("Client Disconnected From Share");
      break;

    case WStype_CONNECTED:
      {
        // IPAddress ip = webSocket.remoteIP(client_num);
        // Serial.printf("[%u] Connection from ", client_num);
        // Serial.println(ip.toString());
        Serial.println("Client Connected To Share");
      }
      break;

    case WStype_TEXT:

      /************** WebSocket Code to update collect controller values *************/

      DeserializationError error = deserializeJson(share_rx, payload);

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      } else {
        // Get x, y, speed, angle, and velocity values from JSON
        totaldistanceCovered = share_rx["tdc"].as<float>();
        currentSeedSpacing = share_rx["ses"].as<float>();
        currentSeedDepth = share_rx["sedep"].as<float>();
        currentRowSpacing = share_rx["rws"].as<int>();
        holesDug = share_rx["nhc"].as<int>();
        rowsCompleted = share_rx["nrc"].as<int>();
        holesPossible = share_rx["nhp"].as<int>();
        rowsPossible = share_rx["nrp"].as<int>();
        farmLength = share_rx["Lf"].as<float>();
        farmBreadth = share_rx["Bf"].as<float>();
        farmArea = share_rx["farmArea"].as<float>();
        currentKernelsPerHole = share_rx["kernelsph"].as<int>();
        cropType = share_rx["cropType"].as<String>();
        soilType = share_rx["soilType"].as<String>();
        stLane = share_rx["stLane"].as<String>();
        needturn = share_rx["needturn"].as<String>();
        brakeState = share_rx["brakeState"].as<int>();

        //Receive acknowledgement for Planting Parameters
        int acknowledge = share_rx["received"].as<int>();
        // Serial.print("Acknowledge ");
        // Serial.println(acknowledge);
        if (acknowledge == 1) {
          plantingStatus = true;
        } else {
          plantingStatus = false;
        }

        #if 0  // Set to 1 to activate or 0 to deactivate
          Serial.print("x= " + String(x));
          Serial.print(", y= " + String(y));
          Serial.print(", speed= " + String(Speed));
          Serial.print(", angle= " + String(Angle));
          Serial.println();
        #endif

        //Transfer back to Websocket Client on Share
        StaticJsonDocument<200> brake_state;
        String payload9 = "";
        JsonObject object = brake_state.to<JsonObject>();
        object["brakeState"] = brakeValue;

        serializeJson(brake_state, payload9);

        // webSocketControl.broadcastTXT(payload2);
        webSocketControl.broadcastTXT(payload9);
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
