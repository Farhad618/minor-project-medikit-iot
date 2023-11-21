// Import required libraries
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "esp32";
const char* password = "amibolbona";

const char* http_username = "admin";
const char* http_password = "admin";

const char* PARAM_INPUT_1 = "wifiName";
const char* PARAM_INPUT_2 = "wifiPassword";
const char* PARAM_INPUT_3 = "userId";
const char* PARAM_INPUT_4 = "userPassword";

const int output = 2;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// index page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Login Page</title>
  <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">
</head>
<body>
  <div class="container">
    <div class="row">
      <div class="col-md-6 offset-md-3">
        <h2 class="text-center">Login</h2>        
          <div class="mb-3">
            <label for="wifiName" class="form-label">Your Wifi Name</label>
            <input type="text" class="form-control" id="wifiName" name="wifiName" required>
          </div>
          <div class="mb-3">
            <label for="wifiPassword" class="form-label">Your Wifi Password</label>
            <input type="password" class="form-control" id="wifiPassword" name="wifiPassword" required>
          </div>
          <div class="mb-3">
            <label for="userId" class="form-label">Your User ID</label>
            <input type="text" class="form-control" id="userId" name="userId" required>
          </div>
          <div class="mb-3">
            <label for="userPassword" class="form-label">Your Password</label>
            <input type="password" class="form-control" id="userPassword" name="userPassword" required>
          </div>
          <button onClick="sendText()" class="btn btn-primary">Submit</button>        
      </div>
    </div>
  </div>
  <script>
  function sendText() {
    var xhr = new XMLHttpRequest();
    // let text1 = document.getElementById("text1").value;
    let wifiName = document.getElementById("wifiName").value;
    let wifiPassword = document.getElementById("wifiPassword").value;
    let userId = document.getElementById("userId").value;
    let userPassword = document.getElementById("userPassword").value;
    // xhr.open("GET", `/msg?text=${text1}`, true); 
	xhr.open("GET", `/data?wifiName=${wifiName}&wifiPassword=${wifiPassword}&userId=${userId}&userPassword=${userPassword}`, true); 
    xhr.send();
  }
  </script>
</body>
</html>
)rawliteral";

void setup() {
// Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(output, OUTPUT);
  digitalWrite(output, LOW);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    // if(!request->authenticate(http_username, http_password))
    //   return request->requestAuthentication();
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/msg?text=<inputMessage>
  server.on("/data", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // if(!request->authenticate(http_username, http_password))
    //   return request->requestAuthentication();
    String inputMessage;
    String wifiName;
    String wifiPassword;
    String userId;
    String userPassword;
    String inputParam;
    // GET input1 value on <ESP_IP>/msg?text=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2) && request->hasParam(PARAM_INPUT_3) && request->hasParam(PARAM_INPUT_4)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      wifiName = request->getParam(PARAM_INPUT_1)->value();
      wifiPassword = request->getParam(PARAM_INPUT_2)->value();
      userId = request->getParam(PARAM_INPUT_3)->value();
      userPassword = request->getParam(PARAM_INPUT_4)->value();
      // inputParam = PARAM_INPUT_1;
      // digitalWrite(output, inputMessage.toInt());
      // Serial.println("inputMessage " + inputMessage);
      Serial.println("wifiName " + wifiName);
      Serial.println("wifiPassword " + wifiPassword);
      Serial.println("userId " + userId);
      Serial.println("userPassword " + userPassword);
      // Serial.println("inputParam " + inputParam);

      
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println("end");
    request->send(200, "text/plain", "OK");
  });

  // Start server
  server.begin();

}

void loop() {
  // put your main code here, to run repeatedly:

}
