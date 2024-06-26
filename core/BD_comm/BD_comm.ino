#include <Arduino.h>

#define LED1_PIN 14
#define LED2_PIN 12
#define PHOTODIODE_PIN 34

volatile uint8_t ledState = 0;

void setup() {
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  
  pinMode(PHOTODIODE_PIN, INPUT);

  // Criação das tarefas FreeRTOS
  xTaskCreatePinnedToCore(
    toggleLEDs,          
    "Toggle LEDs",       
    1024,          
    NULL,              
    1,                   
    NULL,                
    1                    
  );

  xTaskCreatePinnedToCore(
    readPhotodiode,     
    "Read Photodiode", 
    1024,            
    NULL,              
    1,                  
    NULL,              
    1                    
  );
}

void loop() {
}

void toggleLEDs(void *parameter) {
  while (true) {
    ledState = (ledState + 1) & 0x03; 


    digitalWrite(LED1_PIN, (ledState == 0) ? HIGH : LOW);
    digitalWrite(LED2_PIN, (ledState == 2) ? HIGH : LOW);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void readPhotodiode(void *parameter) {
  while (true) {
    int sensorValue = analogRead(PHOTODIODE_PIN);
    Serial.println(sensorValue);
    
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
