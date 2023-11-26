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
#include "pitches.h"
#define NOTE_A4  440
// #include "core-0.h"
// #include "core-1.h"

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
const int BTN_3 = 32;  // btn
const int BTN_2 = 33;
const int BTN_1 = 25;
const int BTN_0 = 26;
const int BUZZER_1 = 23;
const int PULLUP_BUTTON_STOP_ALARM = 22;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// creating http client
HTTPClient http;
String data;                  // api fetch data
int mediNotTakenTimer = 600;  // if medicine not taken then timer will decrease
int buzzerStatus = 0;

// task init
TaskHandle_t Task0;
TaskHandle_t Task1;

// functions
void checkBoxOpenClose(int pinNumber) {  // box opened or not check
  while (digitalRead(pinNumber)) {
    if (mediNotTakenTimer <= 0) {
      return;
    }
    // Serial.println("not opened");
    delay(300);
    mediNotTakenTimer--;
  }
  while (!digitalRead(pinNumber)) {
    // Serial.println("opened");
    delay(300);
  }
  if (digitalRead(pinNumber)) {
    Serial.println("closed again");
  }
}
void beep(int note, int duration) {  // buzzer sound making func
  tone(BUZZER_1, note, duration);
  delay(duration);
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
  pinMode(BTN_0, INPUT);
  pinMode(BTN_1, INPUT);
  pinMode(BTN_2, INPUT);
  pinMode(BTN_3, INPUT);
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
  while (userId == "NULL") {
    Serial.print(".");
    delay(500);
  }

  Serial.print("User id got: ");
  Serial.println(userId);

  // initialize the medi-kit backend server
  Serial.println(String("http://192.168.43.223:3001/api/esp32/activate/") + userId + "/4");
  http.begin(String("http://192.168.43.223:3001/api/esp32/activate/") + userId + "/4");
  // http://localhost:3001/api/esp32/activate/t@t.t/4


  // http.end();
  // fetch end
  // ____________________________________________________________________________________________________________
  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    Task0code, /* Task function. */
    "Task0",   /* name of task. */
    100000,     /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &Task0,    /* Task handle to keep track of created task */
    0);        /* pin task to core 0 */
  delay(500);

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
    Task1code, /* Task function. */
    "Task1",   /* name of task. */
    10000,     /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &Task1,    /* Task handle to keep track of created task */
    1);        /* pin task to core 1 */
  delay(500);
}

// ____________________________________________________
//Task0code: on/off leds, send email, reed switch (under loop())
void Task0code(void* pvParameters) {
  Serial.print("Task0code() running on core ");
  Serial.println(xPortGetCoreID());
  while (1) {
    Serial.println("Task 0 running.");
    // if any box is open initially then stuck the program and blink the corresponding  led
    while (!digitalRead(BTN_0) || !digitalRead(BTN_1) || !digitalRead(BTN_2) || !digitalRead(BTN_3)) {
      digitalWrite(LED_0, !digitalRead(BTN_0));
      digitalWrite(LED_1, !digitalRead(BTN_1));
      digitalWrite(LED_2, !digitalRead(BTN_2));
      digitalWrite(LED_3, !digitalRead(BTN_3));
      delay(500);
      digitalWrite(LED_0, LOW);
      digitalWrite(LED_1, LOW);
      digitalWrite(LED_2, LOW);
      digitalWrite(LED_3, LOW);
      delay(500);
    }

    // fetch start
    Serial.println("Tring to fetch data from API.");
    while (http.GET() != 200) {
      Serial.print(".");
      delay(500);
    }
    Serial.println("Data fetched successfully.");
    data = http.getString();
    Serial.println(data);

    // check data == null then start buzzer
    if (data[0] == 'n') {
      buzzerStatus = 0;
    } else {
      buzzerStatus = 1;
    }

    Serial.println("checkpiont1");
    Serial.print("buzzerStatus ");
    Serial.println(buzzerStatus);

    // check for each index of data var
    if (data[0] == '1') {
      Serial.println("__0__");
      digitalWrite(LED_0, HIGH);
      Serial.println(digitalRead(BTN_0));
      checkBoxOpenClose(BTN_0);
      digitalWrite(LED_0, LOW);
    }
    if (data[1] == '1') {
      Serial.println("__1__");
      digitalWrite(LED_1, HIGH);
      Serial.println(digitalRead(BTN_1));
      checkBoxOpenClose(BTN_1);
      digitalWrite(LED_1, LOW);
    }
    if (data[2] == '1') {
      Serial.println("__2__");
      digitalWrite(LED_2, HIGH);
      Serial.println(digitalRead(BTN_2));
      checkBoxOpenClose(BTN_2);
      digitalWrite(LED_2, LOW);
    }
    if (data[3] == '1') {
      Serial.println("__3__");
      digitalWrite(LED_3, HIGH);
      Serial.println(digitalRead(BTN_3));
      checkBoxOpenClose(BTN_3);
      digitalWrite(LED_3, LOW);
    }

    // check data == null then start buzzer
    if (data[0] != 'n') {
      buzzerStatus = 0;
    }
    
    Serial.println("checkpiont2");

    // if medi not taken in the mentioned time frame then send email
    if (mediNotTakenTimer <= 0) {
    Serial.println("checkpiont3");
      http.end();
      Serial.println("Sending Email.");
      http.begin(String("http://192.168.43.223:3001/api/esp32/send/") + userId);
      while (http.GET() != 200) {
        Serial.print(".");
        delay(500);
      }
      Serial.println("Email sent.");
      http.end();
      http.begin(String("http://192.168.43.223:3001/api/esp32/activate/") + userId + "/4");
      mediNotTakenTimer = 600;
    } else {
    Serial.println("checkpiont4");
      // loop repeat after every 60000 milliseconds
      delay(60000);
    Serial.println("checkpiont6");
    }
  }
}

// _____________________________________________________
//Task1code: Buzzer beep
void Task1code(void* pvParameters) {
  Serial.print("Task1code() running on core ");
  Serial.println(xPortGetCoreID());
  while(1){
    while(buzzerStatus==1) {
      Serial.println("checkpiont7");
      Serial.println("Task 0 running.");
      beep(NOTE_A4, 500);
      // digitalWrite(BUZZER_1, HIGH);
      delay(300);
      beep(NOTE_A4, 500);
      // digitalWrite(BUZZER_1, LOW);
      delay(300);
    }
    delay(10);
  }

    // return;
    Serial.println("checkpiont5");
}

void loop() {
  // put your main code here, to run repeatedly:
}
