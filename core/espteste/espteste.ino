#include <WiFi.h>
#include <WebServer.h>

// Definição das credenciais de WiFi
const char *ssid = "Martins WiFi6";
const char *password = "17031998";

// Inicialização do servidor web na porta 80
WebServer server(80);
const int redLed = 26;
const int greenLed = 27;
const int analog = 33;

// Função para lidar com requisições na rota principal ("/")
void handleRoot() {
}

// Função de configuração inicial do dispositivo
void setup() {
  Serial.begin(115200);
  // Conecta-se à rede WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  server.begin();

  Serial.println("Servidor iniciado");
}

// Função principal que é executada repetidamente
void loop() {
}