#include <Arduino.h>
#include <Servo.h> // Standard Servo library works with ESP8266
#include <ESP8266WiFi.h>      // For WiFi connectivity on ESP8266
#include <ESP8266WebServer.h> // For creating a web server on ESP8266




// --- Global Variables ---
// Replace with your network credentials
const char* ssid = "Fortune";
const char* password = "favorsthebold";




// Declare the server object globally using the ESP8266WebServer type
ESP8266WebServer server(80); // <--- KEY CHANGE: Use ESP8266WebServer




Servo reload; //reloading servo
Servo sideswipe; //servo that controls lateral movement
Servo verticle; //servo that controls vertical movement




// Global variables to store the current angle of each servo
// IMPORTANT: Initialize these to match the initial servo.write() values in setup()
int reloading = 0; //positioning of the reload servo
int sideswiping = 60; //positioning of the lateral servo
int verticling = 60; //positioning of the vertical servo




// Pin definitions (these look like correct GPIOs for ESP8266)
const int motorDC1 = 14;
const int motorDC2 = 16;
const int reloadPIN = 2; // GPIO2 (D4 on NodeMCU)
const int sideswipePIN = 0; // GPIO0 (D3 on NodeMCU)
const int verticlePIN = 4; // GPIO4 (D2 on NodeMCU)




// --- HTML Content (stored in Program Memory) ---
const char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
   <head>
       <meta name="viewport" content="width=device-width, initial-scale=1">
       <title>Shot of Fortune</title>
       <script>
           document.addEventListener('keydown', function(event) {
               const duration = 1000; // Example duration in ms
               let endpoint = '';




               switch (event.key) {
                   case 'a': // Key for leftRotate
                       endpoint = '/moveLeft';
                       break;
                   case 'd': // Key for rightRotate
                       endpoint = '/moveRight';
                       break;
                   case 'w': // Key for upShift
                       endpoint = '/moveUp';
                       break;
                   case 's': // Key for downShift
                       endpoint = '/moveDown';
                       break;
                   case 'q': // Key for stopping fly wheels
                       endpoint = '/Stopfly';
                       break;
                   case 'e': // Key for starting fly wheels
                       endpoint = '/Startfly';
                       break;
                   case ' ': // Key for reloading (spacebar)
                       endpoint = '/gears';
                       break;
               }




               if (endpoint) {
                   fetch(endpoint)
                       .then(response => response.text())
                       .then(data => console.log(data))
                       .catch(error => console.error('Error:', error));
               }
           });
       </script>
       <style>
           body {
               font-family: Arial, sans-serif;
               background-color: #f0f0f0;
               color: #333;
               text-align: center;
               padding: 50px;
               background-color: antiquewhite;
           }
           h1 {
               color: #4CAF50;
           }
           button {
               padding: 10px 20px;
               font-size: 16px;
               background-color: #4CAF50;
               color: white;
               border: none;
               border-radius: 5px;
               cursor: pointer;
           }
           button:hover {
               background-color: #45a049;
           }
       </style>
   </head>
   <body>
       <h1>Welcome to Shot of Fortune</h1>
       <p>This is a simple game where you can test your luck!</p>
       <p>Click the button below to start playing.</p>
       <button onclick="startGame()">Start Game</button>




       <script>
           function startGame() {
               alert('Game started! Good luck!');
           }
       </script>
   </body>
</html>
)=====";




// --- Function Definitions ---




void flyWheels() {
 digitalWrite(motorDC1, HIGH);
 digitalWrite(motorDC2, HIGH);
 Serial.println("Starting fly wheels");
}




void quitFlyWheels() {
 digitalWrite(motorDC1, LOW);
 digitalWrite(motorDC2, LOW);
 Serial.println("Stopping fly wheels");
}




void gears() {
 Serial.println("Activating gears (reloading)");
 reload.write(180); // Move servo to 135 degrees
 delay(1000);
 reload.write(0); // Move servo back to 0 degrees
}




void leftmove() {
 if (sideswiping < 120) {
    sideswiping += 5;
    sideswipe.write(sideswiping); // Move servo 2 to 120 degrees
    Serial.print("Moving left to ");
    Serial.println(sideswiping);
 }
 else {
    Serial.println("Too far left");
 }
}




void rightmove() {
 if (sideswiping > 0) {
    sideswiping -= 5;
    sideswipe.write(sideswiping); // Move servo 2 to 120 degrees
    Serial.print("Moving right to ");
    Serial.println(sideswiping);
    }
 else {
    Serial.println("Too far right");
 }
}




void upmove() {
  if (verticling < 120) {
    verticling += 5;
    verticle.write(verticling); // Move servo 2 to 120 degrees
    Serial.print("Moving up to ");
    Serial.println(verticling);
    }
    else {
        Serial.println("Too far up");
    }
}




void downmove() {
 if (verticling > 0) {
    verticling -= 5;
    verticle.write(verticling); // Move servo 2 to 120 degrees
    Serial.print("Moving down to ");
    Serial.println(verticling);
    }
    else {
        Serial.println("Too far down");
    }
}




// --- Setup Function ---
void setup() {
 Serial.begin(115200);
 delay(1000);
 WiFi.softAP(ssid, password); //begin WiFi access point
 Serial.println("");


 pinMode(motorDC1, OUTPUT);
 pinMode(motorDC2, OUTPUT);




 reload.attach(reloadPIN);
 sideswipe.attach(sideswipePIN);
 verticle.attach(verticlePIN);




 reload.write(0);
 sideswipe.write(60);
 verticle.write(90);




 Serial.print("Connecting to WiFi");


 // --- Define Web Server Routes ---
 server.on("/", HTTP_GET, [](){ // HTTP_GET is defined within ESP8266WebServer.h
   server.send_P(200, "text/html", index_html);
 });




 server.on("/Startfly", HTTP_GET, [](){
   server.send_P(200, "text/plain", "Starting fly wheels...");
   flyWheels();
 });




 server.on("/Stopfly", HTTP_GET, [](){
   server.send_P(200, "text/plain", "Stopping fly wheels...");
   quitFlyWheels();
 });




 server.on("/gears", HTTP_GET, [](){
   server.send_P(200, "text/plain", "Activating gears...");
   gears();
 });




 server.on("/moveLeft", HTTP_GET, [](){
   server.send_P(200, "text/plain", "Moving left...");
   leftmove();
 });




 server.on("/moveRight", HTTP_GET, [](){
   server.send_P(200, "text/plain", "Moving right...");
   rightmove();
 });




 server.on("/moveUp", HTTP_GET, [](){
   server.send_P(200, "text/plain", "Moving up...");
   upmove();
 });




 server.on("/moveDown", HTTP_GET, [](){
   server.send_P(200, "text/plain", "Moving down...");
   downmove();
 });




 server.begin();
 Serial.println("HTTP server started");
}




// --- Loop Function ---
void loop(void) {
 server.handleClient();
}
