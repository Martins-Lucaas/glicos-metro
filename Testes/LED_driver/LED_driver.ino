#define LED_RED_PIN 34
#define LED_GREEN_PIN 32
#define LED_BLUE_PIN 25
#define LED_IR_PIN 13

#define PHOTODIODE_PIN 26

volatile uint8_t ledState = 0;

void setup() {
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(LED_IR_PIN, OUTPUT);

  pinMode(PHOTODIODE_PIN, INPUT);
  Serial.begin(9600);  // Inicialize a comunicação serial
  delay(2000); // Espera para a inicialização da comunicação serial
  
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
        digitalWrite(LED_BLUE_PIN, LOW);
        digitalWrite(LED_IR_PIN, LOW);
        Serial.println("LED Vermelho Ligado");
        break;
      case 1:
        digitalWrite(LED_RED_PIN, LOW);
        digitalWrite(LED_GREEN_PIN, HIGH);
        digitalWrite(LED_BLUE_PIN, LOW);
        digitalWrite(LED_IR_PIN, LOW);
        Serial.println("LED Verde Ligado");
        break;
      case 2:
        digitalWrite(LED_RED_PIN, LOW);
        digitalWrite(LED_GREEN_PIN, LOW);
        digitalWrite(LED_BLUE_PIN, HIGH);
        digitalWrite(LED_IR_PIN, LOW);
        Serial.println("LED Azul Ligado");
        break;
      case 3:
        digitalWrite(LED_RED_PIN, LOW);
        digitalWrite(LED_GREEN_PIN, LOW);
        digitalWrite(LED_BLUE_PIN, LOW);
        digitalWrite(LED_IR_PIN, HIGH);
        Serial.println("LED IR Ligado");
        break;
    }
    ledState = (ledState + 1) % 4; // Alterna entre 0, 1, 2 e 3

    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}

void readPhotodiode(void *parameter) {
  while (true) {
    int sensorValue = analogRead(PHOTODIODE_PIN);
    Serial.println(sensorValue);
    
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
