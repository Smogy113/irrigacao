//Código para envio de dados de sensores e transmissão
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include "defSensors.h"
#include "checkPins.h"


void readMacAddress() {
    uint8_t baseMac[6];
    esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
    if (ret == ESP_OK) {
        Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
    } else {
        Serial.println("Failed to get MAC address");
    }
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) { //Verificar se o envio foi bem-sucedido
    Serial.print("Status do último envio: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sucesso" : "Falha");
}


// Pinos dos Sensores Analógicos
const int AnSensor1 = 34;
const int AnSensor2 = 35;
const int AnSensor3 = 32;
const int AnSensor4 = 33;

// Pinos dos Sensores Digitais
const int DiSensor1 = 4;
const int DiSensor2 = 13;
const int DiSensor3 = 14;
const int DiSensor4 = 25;

uint8_t broadcastAddress[] = {0x8C, 0x4F, 0x00, 0x28, 0xEA, 0xB4}; // Endereço MAC de broadcast (todos os dispositivos)

// Criação do pacote de dados (Atualmente com 22 Bytes no total)
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
} dataPacket;

dataPacket sendPacked;
esp_now_peer_info_t peerInfo;

// --- FUNÇÃO 1: INSPEÇÃO DE BYTES (HEX DUMP) ---
void printHexElegante(uint8_t *dados, size_t tamanho) {
  Serial.println("\n--- [INSPEÇÃO DE MEMÓRIA BRUTA] ---");
  for (size_t i = 0; i < tamanho; i++) {
    Serial.printf("%02X ", dados[i]);
    if ((i + 1) % 4 == 0) Serial.print("  "); // Agrupa de 4 em 4 bytes
  }
  Serial.println("\n-----------------------------------");
}

// --- FUNÇÃO 2: LEITURA HUMANIZADA (CONVERSÃO) ---
void printPacoteHumanizado(dataPacket *pacote) {
  Serial.println("\n=================================");
  Serial.println("       DADOS DO PACOTE ESP32     ");
  Serial.println("=================================");
  Serial.printf("ID do ESP:   %d\n", pacote->espId);
  Serial.printf("Tipo Msg:    %d\n", pacote->messageType);
  Serial.printf("Num Msg:     %d\n", pacote->messageNum);
  Serial.printf("Emergência:  %s\n", pacote->emergency ? "SIM" : "NÃO");
  Serial.println("---------------------------------");
  Serial.println("Leituras dos Sensores:");
  Serial.printf(" - Analógico 1 (Decimal): %d\n", pacote->fas1Data);
  Serial.printf(" - Analógico 2 (Decimal): %d\n", pacote->fas2Data);
  Serial.printf(" - Analógico 1 (Inteiro): %d\n", pacote->ias1Data);
  Serial.printf(" - Analógico 2 (Inteiro): %d\n", pacote->ias2Data);
  Serial.printf(" - Digital 1   (Decimal): %d\n", pacote->ids1Data); // Mostra o tamanho total
  Serial.printf(" - Digital 2   (Decimal): %d\n", pacote->ids2Data); // Mostra o tamanho total
  Serial.printf(" - Digital 1   (Inteiro): %d\n", pacote->fds1Data); // Mostra o valor real (0 ou 1)
  Serial.printf(" - Digital 2   (Inteiro): %d\n", pacote->fds2Data); // Mostra o valor real (0 ou 1)
  Serial.println("---------------------------------");
  Serial.printf("Gabarito/Tail: 0x%04X\n", pacote->gabarito);
  Serial.println("=================================\n");
}


void setup() {
  Serial.begin(115200);

  // Zera a memória do pacote ao ligar o ESP32
  memset(&sendPacked, 0, sizeof(dataPacket));


  // Preenchimento de teste
  sendPacked.espId = 1;
  sendPacked.messageType = 1;
  sendPacked.messageNum = 0;
  sendPacked.emergency = 0;

  sendPacked.fas1Data = 1;
  sendPacked.fas2Data = 2;
  sendPacked.ias1Data = 3;
  sendPacked.ias2Data = 4;
  sendPacked.fds1Data = 5;
  sendPacked.fds2Data = 6;
  sendPacked.ids1Data = 7;
  sendPacked.ids2Data = 8;
  
  sendPacked.gabarito = 0b0000000000000000; // Gabarito de teste (2 Bytes)


  uint8_t *buffer_bytes = (uint8_t*)&sendPacked;

  delay(1000);

  // Modo 2: Ver os dados como o usuário veria
  printPacoteHumanizado(&sendPacked);

  WiFi.begin();
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) { // Inicializa o ESP-NOW
    Serial.println("Erro ao inicializar ESP-NOW");
    return;
  }

  esp_now_register_send_cb(esp_now_send_cb_t(OnDataSent)); // Registra a função de callback para envio de dados

  //Registra o peer (destinatário) para o envio de dados
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0; // Canal padrão
  peerInfo.encrypt = false; // Sem criptografia

  //Adiciona o peer para a lista de peers do ESP-NOW
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Erro ao adicionar peer");
    return;
  }




}

void loop() {

  defAndCheckPins();
  
  //Definindo o pacote de dados a ser enviado (preenchido com valores de teste)
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&sendPacked, sizeof(sendPacked)); // Envia o pacote de dados para o endereço de broadcast

  if (result == ESP_OK) {
    Serial.println("Pacote de dados enviado com sucesso");
  } else {
    Serial.println("Erro ao enviar pacote de dados");
  }

  sendPacked.messageNum += 1;

  delay(2000);

  // Modo 1: Ver os bytes bonitinhos
  //printHexElegante(buffer_bytes, sizeof(dataPacket));
   
}
