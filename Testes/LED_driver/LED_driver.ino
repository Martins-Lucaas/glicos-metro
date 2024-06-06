const int LED1_PIN = 2;
const int LED2_PIN = 4;
const int ANALOG_PIN = 36;

const int INTERVALO_LED1 = 1000;
const int INTERVALO_LED2 = 500;

int frequenciaLeitura = 200;

TaskHandle_t Task1Handle;
TaskHandle_t Task2Handle;
TaskHandle_t AnalogReadHandle;

void setup() {
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  Serial.begin(115200);
  xTaskCreatePinnedToCore(Task1, "Task1", 1000, NULL, 1, &Task1Handle, 0);
  xTaskCreatePinnedToCore(Task2, "Task2", 1000, NULL, 1, &Task2Handle, 1);
  xTaskCreatePinnedToCore(AnalogReadTask, "AnalogReadTask", 1000, NULL, 1, &AnalogReadHandle, 1);
}

void loop() {
}

void Task1(void *pvParameters) {
  while(1) {
    digitalWrite(LED1_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(INTERVALO_LED1));
    digitalWrite(LED1_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(INTERVALO_LED1));
  }
}

void Task2(void *pvParameters) {
  while(1) {
    digitalWrite(LED2_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(INTERVALO_LED2));
    digitalWrite(LED2_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(INTERVALO_LED2));
  }
}

void AnalogReadTask(void *pvParameters) {
  while(1) {
    int valorAnalogico = analogRead(ANALOG_PIN);
    Serial.println(valorAnalogico);
    vTaskDelay(pdMS_TO_TICKS(frequenciaLeitura));
  }
}
