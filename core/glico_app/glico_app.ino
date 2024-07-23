#include <BluetoothSerial.h>
#include <math.h>

BluetoothSerial SerialBT;

const float pi = 3.14159265358979323846;
const float frequency = 1000.0;  // Frequência da onda senoidal em Hz
const int sampleRate = 10000;  // Taxa de amostragem em Hz (deve ser maior que 2x a frequência)
int acquisitionRate = 100;  // Taxa de aquisição em milissegundos
bool isAcquiring = false;  // Flag para aquisição de dados
unsigned long lastConnectionAttempt = 0;  // Timestamp da última tentativa de conexão
const unsigned long connectionInterval = 5000;  // Intervalo entre tentativas de conexão (em milissegundos)
int sampleIndex = 0;  // Índice de amostra atual

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_glucometer");  // Nome do dispositivo Bluetooth
  Serial.println("O dispositivo está pronto para emparelhamento!");
}

void loop() {
  if (SerialBT.hasClient()) {
    if (SerialBT.available()) {
      String command = SerialBT.readStringUntil('\n');
      handleCommand(command);
    }
    if (isAcquiring) {
      float sample = generateSineWave(sampleIndex, frequency, sampleRate);
      SerialBT.println(sample, 4);  // Envia a amostra da onda senoidal com 4 casas decimais
      sampleIndex++;
      if (sampleIndex >= sampleRate / frequency) {
        sampleIndex = 0;  // Reinicia o índice após um ciclo completo
      }
      delay(acquisitionRate);  // Taxa de aquisição
    }
  } else {
    unsigned long currentMillis = millis();
    if (currentMillis - lastConnectionAttempt >= connectionInterval) {
      Serial.println("Aguardando conexão do cliente...");
      lastConnectionAttempt = currentMillis;
    }
  }
}

float generateSineWave(int index, float freq, int sampleRate) {
  return sin(2 * pi * freq * index / sampleRate);
}

void handleCommand(String command) {
  command.trim();
  if (command.startsWith("START")) {
    isAcquiring = true;
    Serial.println("Iniciando aquisição de dados...");
  } else if (command.startsWith("STOP")) {
    isAcquiring = false;
    Serial.println("Parando aquisição de dados...");
  } else if (command.startsWith("RATE")) {
    int rate = command.substring(5).toInt();
    if (rate > 0) {
      acquisitionRate = rate;
      Serial.print("Taxa de aquisição ajustada para ");
      Serial.print(acquisitionRate);
      Serial.println(" ms");
    }
  }
}
