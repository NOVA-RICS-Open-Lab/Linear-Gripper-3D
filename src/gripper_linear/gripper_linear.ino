#include <AccelStepper.h> // Found in Arduino Library or https://www.airspayce.com/mikem/arduino/AccelStepper/
#include "secrets.h" // Containing WiFi Information
#include <WiFi.h>


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
WiFiServer server(80);
// Variable to store the HTTP request
String header;
// Set your Static IP address
IPAddress local_IP(192, 168, 1, 2);
// Set your Gateway IP address
IPAddress gateway(192, 168, 250, 1);
IPAddress subnet(255, 255, 0, 0);

String readString;
// Client variables 
String response;
String dimension;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


int calcSteps(int dimension){
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


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

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
  server.begin();

  stepper.setCurrentPosition(2223);
  stepper.setMaxSpeed(2000); //SPEED Steps / second
  stepper.setAcceleration(1000); // ACCELERATION Steps/(second)^2
  
}

void loop() {

  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        readString += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            if (readString.indexOf("/close") >0){
            Serial.print(readString);
            if (readString.indexOf("?dimension=") >0){    
              int pos1 = readString.indexOf("=");
              Serial.println(pos1);              
              int pos2 = readString.indexOf("HTTP");
              Serial.println(pos2); 
              dimension = readString.substring(pos1+1,pos2-1);
              Serial.print("dimension:");
              Serial.println(dimension);
              response= "skill close done";
              int x=dimension.toInt();
              int steps = 0;
              steps=calcSteps(x);
              if (steps == 0){
                Serial.println ("Nao é possivel apanhar uma peça desse tamanho");
                response= "skil close - the dimension is to big";
              }
              else{
                Serial.println(steps);
                stepper.runToNewPosition(steps);
                delay(5000);
                response = "skill close done";
              }
              
            }                      
          }
          if (readString.indexOf("/open") >0){
            //Serial.print(readString);
            Serial.println("Skill open done");
            stepper.runToNewPosition(0);
            response= "skill open done"; 
          }
            
            // The HTTP response ends with another blank line
            client.println(response);
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    readString = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  /*
  stepper.runToNewPosition(0);
  delay(5000);
  int steps = 0;
  steps=calcSteps(58);
  if (steps == 0){
    Serial.println ("Nao é possivel apanhar uma peça desse tamanho");
  }
  else{
    Serial.println(steps);
    stepper.runToNewPosition(steps);
    delay(5000);
  }
  

  delay(5000);
  stepper.runToNewPosition(0);
  delay(5000);
  stepper.runToNewPosition(2223);
  delay(5000);*/
}