//Task0code: on/off leds, send email, reed switch (under loop())
void Task0code( void * pvParameters ){
  // if any box is open initially then stuck the program and blink the corresponding  led
  while(!digitalRead(BTN_0) || !digitalRead(BTN_1) || !digitalRead(BTN_2) || !digitalRead(BTN_3)){
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

  // check data != null then start buzzer
  if(data[0]='n'){
    buzzerStatus = 0;
  } else {
    buzzerStatus = 1;
  }

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

  // if medi not taken in the mentioned time frame then send email
  if(mediNotTakenTimer<=0){
    http.end();
    Serial.println("Sending Email.");
    http.begin(String("http://192.168.43.223:3001/api/esp32/send/")+userId);
    while (http.GET() != 200) {
      Serial.print(".");
      delay(500);
    }
    Serial.println("Email sent.");
    http.end();
    http.begin(String("http://192.168.43.223:3001/api/esp32/activate/")+userId+"/4");
    mediNotTakenTimer=600;
  } else {
    // loop repeat after every 60000 milliseconds
    delay(60000);    
  }
}