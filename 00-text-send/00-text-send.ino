// Import required libraries
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#else
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include "html-login-page-var.h"
#include <HTTPClient.h>

// Replace with your network credentials
const char* ssid = "esp32";
const char* password = "amibolbona";

// get request params/ auth prams
const char* PARAM_INPUT_1 = "userId";
const char* PARAM_INPUT_2 = "userPassword";

// auth data id & pass
String userId = "NULL";
String userPassword;

// pin configuration
const int LED_BUILTIN = 2;
const int LED_3 = 16;
const int LED_2 = 17;
const int LED_1 = 18;
const int LED_0 = 19;
const int BTN_3 = 32; // btn
const int BTN_2 = 33;
const int BTN_1 = 25;
const int BTN_0 = 26;
const int BUZZER_1 = 23;
const int PULLUP_BUTTON_STOP_ALARM = 22;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// creating http client 
HTTPClient http;
String data; // api fetch data
int ifMediNotTakenTimer = 0; // if medicine not taken then timer will increase


// functions 
void checkBoxOpenClose(int pinNumber){
  while(digitalRead(pinNumber)){
    // Serial.println("not opened");
    delay(300);
  }
  while(!digitalRead(pinNumber)){
    // Serial.println("opened");
    delay(300);
  }
  if(digitalRead(pinNumber)){
    Serial.println("closed again");
  }
}


void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  // pin i/o setup
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_0, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(BTN_0, INPUT_PULLDOWN);
  pinMode(BTN_1, INPUT_PULLDOWN);
  pinMode(BTN_2, INPUT_PULLDOWN);
  pinMode(BTN_3, INPUT_PULLDOWN);
  pinMode(BUZZER_1, OUTPUT);

  // initially leds off state
  digitalWrite(LED_0, LOW);
  digitalWrite(LED_1, LOW);
  digitalWrite(LED_2, LOW);
  digitalWrite(LED_3, LOW);


  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/data?text=<inputMessage>
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest* request) {


    // GET input1 value on <ESP_IP>/data?text=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      userId = request->getParam(PARAM_INPUT_1)->value();
      userPassword = request->getParam(PARAM_INPUT_2)->value();

      Serial.println("userId " + userId);
      Serial.println("userPassword " + userPassword);
      server.end();

    } else {
      Serial.println("No data received");
    }
    Serial.println("end");
    request->send(200, "text/plain", "OK");
  });

  // Start server
  server.begin();

  // waiting for user to set userid
  Serial.println("Waiting for user id.");
  while(userId=="NULL"){
    Serial.print(".");
    delay(500);
  }

  Serial.print("User id got: ");
  Serial.println(userId);

  // initialize the medi-kit backend server
  Serial.println(String("http://192.168.43.223:3001/api/esp32/activate/")+userId+"/4");
  http.begin(String("http://192.168.43.223:3001/api/esp32/activate/")+userId+"/4");
  // http://localhost:3001/api/esp32/activate/t@t.t/4

  

  // http.end();
  // fetch end
}

void loop() {
  // put your main code here, to run repeatedly:

  // fetch start
  Serial.println("Tring to fetch data from API.");
  while (http.GET() != 200) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Data fetched successfully.");
  data = http.getString();
  Serial.println(data);

  // check for each index of data var
  if(data[0]=='1'){
    Serial.println("__0__");
    digitalWrite(LED_0, HIGH);
    Serial.println(digitalRead(BTN_0));
    checkBoxOpenClose(BTN_0);
    digitalWrite(LED_0, LOW);
  }
  if(data[1]=='1'){
    Serial.println("__1__");
    digitalWrite(LED_1, HIGH);
    Serial.println(digitalRead(BTN_1));
    checkBoxOpenClose(BTN_1);
    digitalWrite(LED_1, LOW);
  }
  if(data[2]=='1'){
    Serial.println("__2__");
    digitalWrite(LED_2, HIGH);
    Serial.println(digitalRead(BTN_2));
    checkBoxOpenClose(BTN_2);
    digitalWrite(LED_2, LOW);
  }
  if(data[3]=='1'){
    Serial.println("__3__");
    digitalWrite(LED_3, HIGH);
    Serial.println(digitalRead(BTN_3));
    checkBoxOpenClose(BTN_3);
    digitalWrite(LED_3, LOW);
  }


  // loop repeat after every 60000 milliseconds
  delay(60000); 
}
