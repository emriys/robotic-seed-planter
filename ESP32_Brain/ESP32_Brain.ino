//ALL DIMENSIONS ARE IN CENTIMETERS (CM)
// ses=Seed Spacing, sedep=Seed Depth, nsph=number of seeds per hole,
// nhc=number of holes completed, nrc=number of rows completed
// ncp=number of columns possible, nrp=number of rows possible
// nhp=number of holes possible,
// rws=row spacing,
// Lf=Length of farm in meters, Bf=Breadth of farm in meters
// tdc = total distance covered
// distperRow = total distance possible per row based on farm Length/Breadth and number of holes possible per row

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
int kernelsPerHole;
float farmLength, farmBreadth;

//Declare planting variables to update
int ses, sedep;
int rws, nhc = 0, nrc = 0, ncp, nrp, nsph, nhp;
float Lf;
float Bf;
float tdc, farmArea;
int kernelsph;

// Boolean to keep track of the planter's current planting status
bool plantingStatus = false;
int acknowledge;

// Initialize network parameters
const char* ssid = "Planter Bot";
const char* password = "YOUR_PASSWORD";

// Variables to check if connection to server is lost and then reset planting status
unsigned long connectLost;
unsigned long mayconnectLost;

// BLDC Wheels dimensions
const double WHEEL_DIAMETER_IN = 9.75;        // Motor wheel diamater (inches)
const double WHEEL_CIRCUMFERENCE_IN = 30.64;  // Motor wheel circumference (inches)
const double WHEEL_DIAMETER_CM = 24.77;       // Motor wheel diamater (centimeters)
const double WHEEL_CIRCUMFERENCE_CM = 77.82;  // Motor wheel circumference (centimeters)

// Pin Declarations for one motor
#define PIN_BRAKE 4  // Motor brake signal (Active LOW)

// Distance variables
double distperRow;     // Possible distance per row
double totaldistCovR;  // Wheel distance covered per row
float totaldistCovM;   // Wheel distance covered totally in meters


// Declare pins for Drill retractor, Seedmotor, hind wheels, drills relays, and steering relays
#define drillDeploy 16
#define drillRetract 17
#define seedMotor 15
#define hindWheelsF1 12
#define hindWheelsB1 14
#define hindWheelsF2 22
#define hindWheelsB2 23
#define drills 13
#define turnRight 19
#define turnLeft 21



/************** SET-UP ****************************************/

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set pin modes
  pinMode(PIN_BRAKE, OUTPUT);

  pinMode(drillDeploy, OUTPUT);
  pinMode(drillRetract, OUTPUT);
  pinMode(seedMotor, OUTPUT);
  pinMode(hindWheelsF1, OUTPUT);
  pinMode(hindWheelsF2, OUTPUT);
  pinMode(hindWheelsB1, OUTPUT);
  pinMode(hindWheelsB2, OUTPUT);
  pinMode(drills, OUTPUT);
  pinMode(turnRight, OUTPUT);
  pinMode(turnLeft, OUTPUT);

  // Set initial pin states
  digitalWrite(PIN_BRAKE, false);
  digitalWrite(drillDeploy, HIGH);
  digitalWrite(drillRetract, HIGH);
  digitalWrite(seedMotor, HIGH);

  digitalWrite(hindWheelsF1, HIGH);
  digitalWrite(hindWheelsB1, HIGH);
  digitalWrite(hindWheelsF2, HIGH);
  digitalWrite(hindWheelsB2, HIGH);

  digitalWrite(drills, HIGH);
  digitalWrite(turnRight, HIGH);
  digitalWrite(turnLeft, HIGH);

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

    // Reset plantingStatus if connection to server is lost for more than 20 seconds
    if ((connectLost - mayconnectLost) > 20000) {
      plantingStatus = 0;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Reconnection successful!");
      break;
    }
  }


  //Collect all data from server
  // Serial.println(plantingStatus);
  if (cropType == "maize") {
    if (plantingStatus == true) {
      maize();
    }
  }

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
  Lf = farmLength;
  Bf = farmBreadth;
  farmArea = Lf * Bf;  // Area of farm in meters
  int Lfc = Lf * 100;  // Convert Length of farm from meters to centimeters
  int Bfc = Bf * 100;  // Convert Breadth of farm from meters to centimeters

  if (stLane == "length") {
    nrp = Bfc / rws;  //number of rows possible
    // Possible distance per row. Add 1 since it starts planting from 0.
    distperRow = (int(Lfc / ses)) * ses;

    // If total number of holes possible is odd, add 1
    nhp = ((int(Lfc / ses) + 1) * nrp) * 2;  //number of holes possible totally, drilling 2 simultaneously

    return nrp, nhp;
  }

  else if (stLane == "breadth") {
    nrp = Lfc / rws;  //number of rows possible
    // Possible distance per row. Add 1 since it starts planting from 0.
    distperRow = (int(Bfc / ses)) * ses;

    // If total number of holes possible is odd, add 1
    nhp = ((int(Bfc / ses) + 1) * nrp) * 2;  //number of holes possible totally, drilling 2 simultaneously

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

  // Update number of holes completed
  nhc += 2;
  Serial.println((String) "nhc: " + nhc);

  // 1 second delay to give breathing space
  delay(3000);

  // If total farm distance covered and total holes possible is completed, then stop planting process
  if (nhc >= nhp) {
    plantingStatus = false;
    acknowledge = 0;
    nrc += 1;
    stop();
    Serial.println("Planting DONE...");
  }

  delay(2000);

  // Check if per row distance is completed then turn, else move forward

  if ((stLane == "length") && (totaldistCovR >= distperRow)) {
    Serial.println("In Length...");
    moveToTurn();
    delay(2000);
    turn();             // Turn right
    nrc += 1;           // Update number of rows complted
    totaldistCovR = 0;  // Reset distance covered per row
  }

  else if ((stLane == "breadth") && (totaldistCovR >= distperRow)) {
    Serial.println("In Breadth...");
    moveToTurn();
    delay(2000);
    turn();             // Turn right
    nrc += 1;           // Update number of rows complted
    totaldistCovR = 0;  // Reset distance covered per row
  }

  else {
    AutoMove();  // Move robot to next location
    // Update distance travelled after first holes have been dug
    if (2 <= nhc && nhc <= nhp) {
      tdc += 13;
      totaldistCovR += 13;
      totaldistCovM = tdc / 100;
    }
  }

  /************ Update server with completed parameters ************/

  //Transfer back to Websocket Client on Share
  // Update Server
  String payload = "";
  JsonObject object = return_tx.to<JsonObject>();
  object["tdc"] = totaldistCovM;
  object["ses"] = ses;
  object["sedep"] = sedep;
  object["rws"] = rws;
  object["nhc"] = nhc;
  object["nrc"] = nrc;
  object["nhp"] = nhp;
  object["nrp"] = nrp;
  object["Lf"] = farmLength;
  object["Bf"] = farmBreadth;
  object["farmArea"] = farmArea;
  object["kernelsph"] = kernelsPerHole;
  object["cropType"] = cropType;
  object["soilType"] = soilType;
  object["stLane"] = stLane;
  object["received"] = acknowledge;
  serializeJson(return_tx, payload);
  webSocket.sendTXT(payload);

  #if 1 // Set to 1 to activate or 0 to deactivate
    Serial.println((String) "nhc: " + nhc);
    Serial.println((String) "nrc: " + nrc);
    Serial.println((String) "tdc: " + tdc);
    Serial.println((String) "Total Row Distance: " + totaldistCovR);
    Serial.println((String) "Total Distance: " + totaldistCovM);
  #endif

  // If total farm distance covered and total holes possible is completed, then stop planting process
  if (nhc >= nhp) {
    // Reset planting parameters
    nhc = nhp = nrc = nrp = totaldistCovR = totaldistCovM = tdc = 0;
    stop();
    Serial.println("Planting RESET...");
  }

  delay(5000);
}
