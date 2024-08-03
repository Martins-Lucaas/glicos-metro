#include <WiFi.h>
#include <WebServer.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Definição das credenciais de WiFi
const char *ssid = "Martins Wifi6";
const char *password = "17031998";

WebServer server(80);
const int bufferSize = 100;  // Número máximo de pontos no gráfico
float vADCBuffer[bufferSize];
int bufferIndex = 0;
bool updatingData = false;
const unsigned long acquisitionRate = 20;  // Taxa de aquisição fixa em 20ms
const int pinvADC = 33;
const int pinLED = 25;

int blinkRate = 10;  // Taxa inicial de piscar em milissegundos

TaskHandle_t vADCTaskHandle = NULL;
TaskHandle_t blinkTaskHandle = NULL;

// Função para ler o valor do sinal vADC
float readvADCValue() {
  digitalWrite(26, !digitalRead(26));
  int valorADC = analogRead(pinvADC);
  float tensao = ((valorADC * 3.3) / 4095); // Convertendo para volts
  return tensao;
}

void sendCORSHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "*");
}

void vADCTask(void *pvParameters) {
  while (1) {
    if (updatingData) {
      float vADCvalue = readvADCValue();
      vADCBuffer[bufferIndex] = vADCvalue;
      bufferIndex = (bufferIndex + 1) % bufferSize;
      vTaskDelay(pdMS_TO_TICKS(acquisitionRate));
    } else {
      vTaskDelay(100 / portTICK_PERIOD_MS);  // Aguarda 100ms antes de verificar novamente
    }
  }
}

void blinkTask(void *pvParameters) {
  while (1) {
    digitalWrite(pinLED, HIGH);
    vTaskDelay(pdMS_TO_TICKS(blinkRate));
    digitalWrite(pinLED, LOW);
    vTaskDelay(pdMS_TO_TICKS(blinkRate));
  }
}

void startAcquisition() {
  updatingData = true;
  Serial.println("Aquisição de dados iniciada.");
}

void stopAcquisition() {
  updatingData = false;
  Serial.println("Aquisição de dados parada.");
}

void clearBuffer() {
  memset(vADCBuffer, 0, sizeof(vADCBuffer));  // Limpa o buffer
  bufferIndex = 0;
  Serial.println("Buffer limpo.");
}

void handleSetBlinkRate() {
  if (server.hasArg("rate")) {
    blinkRate = server.arg("rate").toInt();
    if (blinkRate <= 0) {
      blinkRate = 10;  // Valor padrão caso a taxa seja inválida
    }
    Serial.print("Taxa de piscar do LED definida para: ");
    Serial.print(blinkRate);
    Serial.println(" ms");
    sendCORSHeaders();
    server.send(200, "text/plain", "Taxa de piscar do LED definida");
  } else {
    sendCORSHeaders();
    server.send(400, "text/plain", "Parâmetro 'rate' não encontrado");
  }
}

void setup() {
  pinMode(26, OUTPUT);
  pinMode(pinvADC, INPUT);
  pinMode(pinLED, OUTPUT);
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }

  Serial.println("Conectado ao WiFi");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_OPTIONS, []() {
    sendCORSHeaders();
    server.send(200);
  });

  server.on("/vADCvalue", HTTP_GET, []() {
    sendCORSHeaders();
    float value = readvADCValue();
    server.send(200, "text/plain", String(value, 4)); // Precisão de 4 casas decimais
  });

  server.on("/startAcquisition", HTTP_GET, []() {
    sendCORSHeaders();
    startAcquisition();
    server.send(200, "text/plain", "Aquisição iniciada");
  });

  server.on("/stopAcquisition", HTTP_GET, []() {
    sendCORSHeaders();
    stopAcquisition();
    server.send(200, "text/plain", "Aquisição parada");
  });

  server.on("/clearBuffer", HTTP_GET, []() {
    sendCORSHeaders();
    clearBuffer();
    server.send(200, "text/plain", "Buffer limpo");
  });

  server.on("/setBlinkRate", HTTP_GET, handleSetBlinkRate);

  server.begin();
  Serial.println("Servidor iniciado");

  xTaskCreate(vADCTask, "vADCTask", 2048, NULL, 1, &vADCTaskHandle);
  xTaskCreate(blinkTask, "blinkTask", 1024, NULL, 1, &blinkTaskHandle);
}

void loop() {
  server.handleClient();
}
