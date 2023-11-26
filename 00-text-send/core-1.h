//Task1code: Buzzer beep
void Task1code( void * pvParameters ){
  while(buzzerStatus){
    beep(NOTE_A4, 500);
    delay(300);
    beep(NOTE_A4, 500);
    delay(300);
  }
}