#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// Criação do pacote de dados recebido (Atualmente com 22 Bytes no total)
typedef struct __attribute__((packed)) {
  // Header do pacote (4 Bytes)
  uint8_t espId;
  uint8_t messageType;
  uint8_t messageNum;
  uint8_t emergency;

  // Payload do pacote (16 Bytes)
  uint16_t fas1Data; // Sensores Analógicos - Decimais
  uint16_t fas2Data;
  uint16_t ias1Data; // Sensores Analógicos - Inteiros
  uint16_t ias2Data;
  
  uint16_t fds1Data; // Sensores Digitais - Decimais
  uint16_t fds2Data;
  uint16_t ids1Data; // Sensores Digitais - Inteiros
  uint16_t ids2Data;

  // Tail do pacote (2 Bytes)
  uint16_t gabarito;
} dataReceived;

dataReceived receivedPacked;

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    memcpy(&receivedPacked, incomingData, sizeof(receivedPacked));
    Serial.print("Bytes recebidos: ");
    Serial.println(len);    
    Serial.println("Número da mensagem: " + String(receivedPacked.messageNum));
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Erro ao inicializar ESP-NOW");
        return;
    }

    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    
}
