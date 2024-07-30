#include <WiFi.h>
#include <WebServer.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Definição das credenciais de WiFi
const char *ssid = "Martins Wifi6";
const char *password = "17031998";

WebServer server(80);
const int bufferSize = 100;  // Número máximo de pontos no gráfico
float vADCBuffer[bufferSize];
int bufferIndex = 0;
bool updatingData = false;
unsigned long acquisitionRate = 500;
const int pinvADC = 33;

TaskHandle_t vADCTaskHandle = NULL;
SemaphoreHandle_t bufferMutex;

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
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    if (updatingData) {
      float vADCvalue = readvADCValue();
      if (xSemaphoreTake(bufferMutex, (TickType_t)10) == pdTRUE) {
        vADCBuffer[bufferIndex] = vADCvalue;
        bufferIndex = (bufferIndex + 1) % bufferSize;
        xSemaphoreGive(bufferMutex);
      }
      vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(acquisitionRate));
    } else {
      vTaskDelay(100 / portTICK_PERIOD_MS);  // Aguarda 100ms antes de verificar novamente
    }
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
  if (xSemaphoreTake(bufferMutex, (TickType_t)10) == pdTRUE) {
    memset(vADCBuffer, 0, sizeof(vADCBuffer));  // Limpa o buffer
    bufferIndex = 0;
    xSemaphoreGive(bufferMutex);
    Serial.println("Buffer limpo.");
  }
}

void setup() {
  pinMode(26, OUTPUT);
  pinMode(pinvADC, INPUT);
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

  server.on("/updateAcquisitionRate", HTTP_GET, []() {
    sendCORSHeaders();
    if (server.hasArg("rate")) {
      acquisitionRate = server.arg("rate").toInt();
      clearBuffer();  // Limpa o buffer quando a taxa de aquisição é alterada
      Serial.print("Taxa de aquisição atualizada para: ");
      Serial.println(acquisitionRate);
    }
    server.send(200, "text/plain", "Taxa de aquisição atualizada e buffer limpo");
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

  server.begin();
  Serial.println("Servidor iniciado");

  bufferMutex = xSemaphoreCreateMutex();
  xTaskCreate(vADCTask, "vADCTask", 2048, NULL, 1, &vADCTaskHandle);
}

void loop() {
  server.handleClient();
}
