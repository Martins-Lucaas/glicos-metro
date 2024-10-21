#define LED_RED_PIN 18
#define LED_GREEN_PIN 19

#define PHOTODIODE_PIN 33

volatile uint8_t ledState = 0;

void setup() {
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);

  pinMode(PHOTODIODE_PIN, INPUT);
  Serial.begin(115200);  // Inicialize a comunicação serial  
  xTaskCreatePinnedToCore(toggleLEDs, "Toggle LEDs", 1024, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(readPhotodiode, "Read Photodiode", 1024, NULL, 1, NULL, 1);
}

void loop() {
}

void toggleLEDs(void *parameter) {
  while (true) {
    switch (ledState) {
      case 0:
        digitalWrite(LED_RED_PIN, HIGH);
        digitalWrite(LED_GREEN_PIN, LOW);
        Serial.println("LED Vermelho Ligado");
        break;
      case 1:
        digitalWrite(LED_RED_PIN, LOW);
        digitalWrite(LED_GREEN_PIN, HIGH);
        Serial.println("LED Verde Ligado");
        break;
    }
    ledState = (ledState + 1) % 2;

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
