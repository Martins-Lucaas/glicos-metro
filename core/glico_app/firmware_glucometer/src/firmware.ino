#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED_V 32
#define LED_IV 33
#define pinvADC 34

volatile uint8_t ledState = 0;

// Definição das credenciais de WiFi
const char *ssid = "Martins Wifi6";
const char *password = "17031998";

WebServer server(80);
const int bufferSize = 1000;  // Número máximo de pontos no gráfico
float vADCBuffer[bufferSize];
int bufferIndex = 0;
bool updatingData = false;
int acquisitionRate = 10;  // Taxa de aquisição inicial em milissegundos

int blinkRate = 10;  // Taxa inicial de piscar em milissegundos

TaskHandle_t vADCTaskHandle = NULL;
TaskHandle_t blinkTaskHandle = NULL;
SemaphoreHandle_t bufferMutex;

// Função para ler o valor do sinal vADC
float readvADCValue() {
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
      xSemaphoreTake(bufferMutex, portMAX_DELAY); // Proteção do buffer
      vADCBuffer[bufferIndex] = vADCvalue;
      bufferIndex = (bufferIndex + 1) % bufferSize;
      xSemaphoreGive(bufferMutex);
      vTaskDelay(pdMS_TO_TICKS(acquisitionRate));
    } else {
      vTaskDelay(100 / portTICK_PERIOD_MS);  // Aguarda 100ms antes de verificar novamente
    }
  }
}

void blinkTask(void *pvParameters) {
  while (true) {
    ledState = (ledState + 1) % 3;
    if (ledState == 0) {
      digitalWrite(LED_V, HIGH);
      digitalWrite(LED_IV, LOW);
    } else if (ledState == 1) {
      digitalWrite(LED_V, LOW);
      digitalWrite(LED_IV, HIGH);
    } else {
      digitalWrite(LED_V, LOW);
      digitalWrite(LED_IV, LOW);
    }
    vTaskDelay(blinkRate / portTICK_PERIOD_MS);
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
  xSemaphoreTake(bufferMutex, portMAX_DELAY); // Proteção do buffer
  memset(vADCBuffer, 0, sizeof(vADCBuffer));  // Limpa o buffer
  bufferIndex = 0;
  xSemaphoreGive(bufferMutex);
  Serial.println("Buffer limpo.");
}

void handleSetAcquisitionRate() {
  if (server.hasArg("rate")) {
    int newRate = server.arg("rate").toInt();
    if (newRate > 0) {
      acquisitionRate = newRate;
      Serial.print("Taxa de aquisição ajustada para: ");
      Serial.print(acquisitionRate);
      Serial.println(" ms");
      sendCORSHeaders();
      server.send(200, "text/plain", "Taxa de aquisição ajustada");
    } else {
      sendCORSHeaders();
      server.send(400, "text/plain", "Taxa inválida");
    }
  } else {
    sendCORSHeaders();
    server.send(400, "text/plain", "Parâmetro 'rate' não encontrado");
  }
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
  pinMode(pinvADC, INPUT);
  pinMode(LED_V, OUTPUT);
  pinMode(LED_IV, OUTPUT);
  Serial.begin(115200);

  // Inicializa o mutex para proteger o buffer
  bufferMutex = xSemaphoreCreateMutex();

  // Inicializa a conexão WiFi
  Serial.println("Inicializando WiFi...");
  WiFi.begin(ssid, password);

  // Verifica o status da conexão WiFi
  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < 20) {
    delay(1000);
    Serial.print("Tentando conectar ao WiFi... tentativa ");
    Serial.println(retryCount + 1);
    retryCount++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Conectado ao WiFi");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Falha ao conectar ao WiFi.");
    esp_deep_sleep_start(); // Entrar em modo de economia de energia se não conectar
    return;
  }

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

  server.on("/setAcquisitionRate", HTTP_GET, handleSetAcquisitionRate);
  server.on("/setBlinkRate", HTTP_GET, handleSetBlinkRate);

  server.begin();
  Serial.println("Servidor iniciado");

  xTaskCreate(vADCTask, "vADCTask", 2048, NULL, 1, &vADCTaskHandle);
  xTaskCreate(blinkTask, "blinkTask", 2048, NULL, 1, &blinkTaskHandle);
}

void loop() {
  static int wifiReconnectAttempts = 0;
  const int maxReconnectAttempts = 10;

  if (WiFi.status() == WL_CONNECTED) {
    server.handleClient();
    wifiReconnectAttempts = 0; // Reset after a successful connection
  } else {
    if (wifiReconnectAttempts < maxReconnectAttempts) {
      Serial.println("WiFi desconectado, tentando reconectar...");
      WiFi.reconnect();
      wifiReconnectAttempts++;
      delay(1000);
    } else {
      Serial.println("Falha ao reconectar, entrando em modo de economia de energia...");
      esp_deep_sleep_start(); // Entrar em modo de economia de energia
    }
  }
}
