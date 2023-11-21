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

const char* PARAM_INPUT_1 = "text";

const int output = 2;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// index page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<body>
  <h2>ESP Web Server</h2>
  
  <input type="text" id="text1" /><br>
  <button onClick="sendText()">Send</button>
  
<script>
function sendText() {
  var xhr = new XMLHttpRequest();
  let text1 = document.getElementById("text1").value;
  xhr.open("GET", `/msg?text=${text1}`, true); 
    
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
  server.on("/msg", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // if(!request->authenticate(http_username, http_password))
    //   return request->requestAuthentication();
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/msg?text=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      // digitalWrite(output, inputMessage.toInt());
      // Serial.println(inputMessage);
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  // Start server
  server.begin();

}

void loop() {
  // put your main code here, to run repeatedly:

}
