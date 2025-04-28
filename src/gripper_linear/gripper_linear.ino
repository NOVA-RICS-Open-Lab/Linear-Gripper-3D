#include <AccelStepper.h> // Found in Arduino Library or https://www.airspayce.com/mikem/arduino/AccelStepper/
#include "secrets.h" // Containing WiFi Information
#include <WiFi.h>
#include <WebServer.h>

#define dir 33
#define step 32
#define motorInterfaceType 1
#define MAX_TRAVEL 112 //distancia em mm das guias lineares
#define MAX_DIST 82 // distancia (mm) entre pinças abertas
#define MAX_HALF_DIST 41// distancia (mm) de uma pinça entre o centro e a abertura maxima
//#define RESOLUTION 0.25 // resolucao é de 1/4
//#define STEP_ANGLE 1.8 // angulo por step
//#define STEPS_360 200 // steps para dar uma volta 
#define STEP_ANGLE 0.45 // angulo por step
#define STEPS_360 800 // steps para dar uma volta completa -> corresponde a 29.50mm entre 
#define DIST_PER_STEP 0.036875 //distancia em mm por cada step

AccelStepper stepper(motorInterfaceType, step, dir); //direction Digital 33, pulse Digital 32 CLK

char ssid[] = SECRET_SSID;   // your network SSID (name)
char password [] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key index number (needed only for WEP)

// Set web server port number to 80
WebServer server(80);
// Variable to store the HTTP request
String header;
// Set your Static IP address
IPAddress local_IP(192, 168, 1, 2);
// Set your Gateway IP address
IPAddress gateway(192, 168, 250, 1);
IPAddress subnet(255, 255, 0, 0);

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// Setup /////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void SetupWifi() {
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void SetupWiFiServer() {

  server.on("/open/", HTTP_POST, HandleOPEN);
  server.on("/close/", HTTP_POST, HandleCLOSE);
  server.onNotFound(HandleNotFound);
  
  server.begin();
}

void SetupStepper() {
  stepper.setCurrentPosition(2223);
  stepper.setMaxSpeed(2000); //SPEED Steps / second
  stepper.setAcceleration(1000); // ACCELERATION Steps/(second)^2
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  SetupWifi();
  SetupWiFiServer();
  SetupStepper();
}

//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// Loop //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  server.handleClient();
  delay(1);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// Server Handlers /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void HandleNotFound() {
  server.send(200, "text/plain", "SKILL NOT FOUND");
}

void HandleOPEN() {
  stepper.runToNewPosition(0);
  server.send(200, "text/plain", "Skill OPEN: Done");
}

void HandleCLOSE() {

  String response;
  String dimension;

  if (server.hasArg("dimension")) {
    dimension = server.arg("dimension");
  }

  int x=dimension.toInt();
  int steps = 0;
  steps=CalcSteps(x);

  if (steps == 0){
    Serial.println ("Not possible to pickup a piece that big");
    response= "The dimension is to big";
  }
  else{
    Serial.println(steps);
    stepper.runToNewPosition(steps);
    delay(5000);
    response = "Done";
  }

  server.send(200, "text/plain", "Skill CLOSE: " + response);
}

//////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// Auxiliary Functions ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

int CalcSteps(int dimension){
  if (dimension > MAX_DIST) {
    return 0;
  }
  else {
     int steps = 0;
    int travel=0;
    travel = MAX_DIST-dimension;
    Serial.println(travel);
    steps=travel/DIST_PER_STEP;
    return steps;
  }
 
}