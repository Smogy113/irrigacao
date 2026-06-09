#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

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

dataPacket receivedPacked;

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    memcpy(receivedPacked.rawBytes, incomingData, len);
    Serial.print("Bytes recebidos: ");
    Serial.println(len);    
    Serial.println("Número da mensagem: " + String(receivedPacked.packet.header.messageNum));
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    esp_wifi_set_channel(11, WIFI_SECOND_CHAN_NONE);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Erro ao inicializar ESP-NOW");
        return;
    }

    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    
}
