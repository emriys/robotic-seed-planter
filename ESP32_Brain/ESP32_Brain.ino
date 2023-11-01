//ALL DIMENSIONS ARE IN CENTIMETERS (CM)
// ses=Seed Spacing, sedep=Seed Depth, nsph=number of seeds per hole,
// nhc=number of holes completed, nrc=number of rows completed
// ncp=number of columns possible, nrp=number of rows possible
// nhp=number of holes possible,
// rws=row spacing,
// Lf=Length of farm, Bf=Breadth of farm
// tdc = total distance covered

// Include required libraries
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

// Declare websocket client class variable
WebSocketsClient webSocket;

// Allocate the JSON document for sharing
StaticJsonDocument<200> doc;

// JSON document to return updated data to server ESP
StaticJsonDocument<1024> return_tx;

//Declare control variables
int x = 0, y = 0, Speed = 0, Angle = 0;

//Declare planting parameters
String cropType = "", soilType = "", stLane = "", planterStartingPoint = "";
int kernelsPerHole, farmLength, farmBreadth;

//Declare planting variables to update
float ses, sedep;
int rws, nhc, nrc, ncp, nrp, nsph, nhp;
int Lf, Bf;
int tdc, kernelsph, farmArea;

// Boolean to keep track of the planter's current planting status
bool plantingStatus = false;
int acknowledge;

// Initialize network parameters
const char* ssid = "Planter Bot";
const char* password = "YOUR_PASSWORD";

// Variables to check if connection to server is lost and then reset planting status
unsigned long connectLost;
unsigned long mayconnectLost;

// Constants
const unsigned long SPEED_TIMEOUT = 500000;   // Time used to determine wheel is not spinning
const unsigned int UPDATE_TIME = 500;         // Time used to output serial data
const unsigned int BUFFER_SIZE = 16;          // Serial receive buffer size
const double BAUD_RATE = 115200;              // Serial port baud rate
const double WHEEL_DIAMETER_IN = 9.75;        // Motor wheel diamater (inches)
const double WHEEL_CIRCUMFERENCE_IN = 30.64;  // Motor wheel circumference (inches)
const double WHEEL_DIAMETER_CM = 24.77;       // Motor wheel diamater (centimeters)
const double WHEEL_CIRCUMFERENCE_CM = 77.82;  // Motor wheel circumference (centimeters)

// // Pin Declarations for one motor
const int PIN_DIR = 2;    // Motor direction signal
const int PIN_BRAKE = 3;  // Motor brake signal (active low)
const int PIN_STOP = 5;
const int PIN_PWM = 0;     // PWM motor speed control
const int PIN_SPEED = 12;  // SC Speed Pulse Output from RioRand board

// // Variables used in move function
double _freq;                               // Frequency of the signal on the speed pin
double _rpm;                                // Wheel speed in revolutions per minute
double _mph;                                // Wheel speed in miles per hour
double _kph;                                // Wheel speed in kilometers per hour
double _cmps;                               // Wheel speed in centimeters per second
double totaldistCov;                        // Wheel distance covered in centimeters
double totaldistCovM = totaldistCov / 100;  // Wheel distance covered in meters

int data;
bool state;

/************** SET-UP ****************************************/

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set pin directions
  pinMode(PIN_SPEED, INPUT);
  pinMode(PIN_PWM, OUTPUT);
  pinMode(PIN_BRAKE, OUTPUT);
  pinMode(PIN_DIR, OUTPUT);
  pinMode(PIN_STOP, OUTPUT);

  // // Set initial pin states
  digitalWrite(PIN_BRAKE, false);
  digitalWrite(PIN_DIR, false);
  analogWrite(PIN_PWM, 0);

  // Set device as a Wi-Fi Station and Connect to
  WiFi.mode(WIFI_STA);
  // WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid);

  //Wait for Wi-Fi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(" ");
    Serial.println("Connecting to WiFi...");
  }
  Serial.println(" ");
  Serial.println("Connected to WiFi");

  // Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // server address, port, and URL path
  webSocket.begin("192.168.4.1", 120, "/share");

  // event handler
  webSocket.onEvent(webSocketEvent);

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);
}


/************** LOOP ****************************************/

void loop() {
  mayconnectLost = millis();
  while (WiFi.status() != WL_CONNECTED) {
    connectLost = millis();
    Serial.println("WiFi Connection Lost...Retrying");
    delay(1000);

    if ((connectLost - mayconnectLost) > 20000) {
      plantingStatus = 0;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Reconnection successful!");
      break;
    }
  }


  //Collect all data from server

  if (cropType == "maize") {
    if (plantingStatus == true) {
      maize();
    }
  }
  // farmCalc(Lf, Bf, rws, ses, sedep);
  webSocket.loop();
}


/************** FUNCTIONS ****************************************/

void webSocketEvent();
// void move();
void drill();
void dropSeed();

void update() {
}

int farmCalc(float rws, float ses, float sedep) {
  //Calculations for farm parameters about rows, holes, e.t.c
  farmArea = Lf * Bf;  //Area of farm in meters
  int Lfc = Lf * 100;  // Convert Length of farm from meters to centimeters
  int Bfc = Bf * 100;  //Convert Breadth of farm from meters to centimeters
  if (stLane == "Length") {
    nrp = Bfc / rws;          //number of rows possible
    nhp = (Lfc / ses) * nrp;  //number of holes possible totally

    return nrp, nhp;
  }

  else if (stLane == "Breadth") {
    nrp = Lfc / rws;          //number of rows possible
    nhp = (Bfc / ses) * nrp;  //number of holes possible totally

    return nrp, nhp;
  }
}

void maize() {
  ses = 13;
  sedep = 11;
  rws = 84;
  //Planting process sequence
  farmCalc(rws, ses, sedep);
  // If planter at starting position
  drill();     // Release then retract drills
  dropSeed();  // Drop seeds into holes
  AutoMove();  // Move robot to next location


  tdc += 10;
  nhc += 2;
  nrc += 3;
  Lf = random(500);
  Bf = random(500);

  /************ Update server with completed parameters ************/

  //Transfer back to Websocket Client on Share
  String payload = "";
  JsonObject object = return_tx.to<JsonObject>();
  // object["sedep"] = sedep;
  object["tdc"] = totaldistCovM;
  object["ses"] = ses;
  object["sedep"] = sedep;
  object["rws"] = rws;
  object["nhc"] = nhc;
  object["nrc"] = nrc;
  object["nhp"] = nhp;
  object["nrp"] = nrp;
  object["Lf"] = Lf;
  object["Bf"] = Bf;
  object["farmArea"] = farmArea;
  object["kernelsph"] = kernelsPerHole;
  object["cropType"] = cropType;
  object["soilType"] = soilType;
  object["stLane"] = stLane;
  object["received"] = acknowledge;


  serializeJson(return_tx, payload);

  webSocket.sendTXT(payload);
}
