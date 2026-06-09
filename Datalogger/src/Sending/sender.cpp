//Código para envio de dados de sensores e transmissão
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include "defSensors.h"
#include "checkPins.h"
#include "dadosAleatorios.h"


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
typedef union {
    //Permite acesso aos dados de maneira manual e por meio de loops
    struct __attribute__((packed)) {

        //Header (4 Bytes)
        struct __attribute__((packed)) {
            uint8_t espId;
            uint8_t messageType;
            uint8_t messageNum;
            uint8_t emergency;
        } header;

        //Payload (16 bytes) organizado em arrays
        struct __attribute__((packed)) {
            uint16_t fasData[2]; // Sensores Analógicos - Decimais (Antigos fas1 e fas2)
            uint16_t iasData[2]; // Sensores Analógicos - Inteiros (Antigos ias1 e ias2)
            uint16_t fdsData[2]; // Sensores Digitais - Decimais   (Antigos fds1 e fds2)
            uint16_t idsData[2]; // Sensores Digitais - Inteiros   (Antigos ids1 e ids2)
        } payload;

        //Tail (2 Bytes)
        uint16_t gabarito;
    } packet;

    uint8_t rawBytes[22]; // Permite acesso aos dados como um array de bytes para inspeção e envio

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



void setup() {
  Serial.begin(115200);

  // Zera a memória do pacote ao ligar o ESP32
  memset(&sendPacked, 0, sizeof(dataPacket));

  sendPacked.packet.gabarito = 0b0000000000000000; // Gabarito de teste (2 Bytes)

  uint8_t *buffer_bytes = (uint8_t*)&sendPacked;

  delay(1000);

  WiFi.begin();
  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(11, WIFI_SECOND_CHAN_NONE);

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
  esp_err_t result = esp_now_send(broadcastAddress, sendPacked.rawBytes, sizeof(sendPacked)); // Envia o pacote de dados para o endereço de broadcast

  sendPacked.packet.header.messageNum++; // Incrementa o número da mensagem para cada envio (simulando mensagens sequenciais)

  sendPacked.packet.payload.fasData[0] = forceData();
  sendPacked.packet.payload.fasData[1] = forceData();
  sendPacked.packet.payload.iasData[0] = forceData();
  sendPacked.packet.payload.iasData[1] = forceData();
  sendPacked.packet.payload.fdsData[0] = forceData();
  sendPacked.packet.payload.fdsData[1] = forceData();
  sendPacked.packet.payload.idsData[0] = forceData();
  sendPacked.packet.payload.idsData[1] = forceData();

  if (result == ESP_OK) {
    Serial.println("Pacote de dados enviado com sucesso");
  } else {
    Serial.println("Erro ao enviar pacote de dados");
  }

  Serial.println(WiFi.channel());

  delay(10000);

  // Modo 1: Ver os bytes bonitinhos
  //printHexElegante(buffer_bytes, sizeof(dataPacket));
   
}
