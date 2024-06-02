#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>
#include <ArduinoJson.h>

#define WIFI_SSID "Martins WiFi6"
#define WIFI_PASSWORD "17031998"
#define FIREBASE_HOST "https://esp32-biomedicaleng-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "hYKKOkhqxLbpNhNRIWdPKdkWwdntFZyoAp20DR5y"

const int pot = 5;
int bufferSize = 1;
int buffer[bufferSize];
int bufferIndex = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(pot, INPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("Conectando ao wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("Wifi funcionando");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  int teste = analogRead(pot);
  buffer[bufferIndex++] = teste;
  
  if (bufferIndex >= bufferSize) {
    bufferIndex = 0;
    for (int i = 0; i < bufferSize; i++) {
      Firebase.setInt("potenciometro", buffer[i]);}

  Serial.println(teste);
}
}