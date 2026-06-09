/* #include <Arduino.h>

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







 typedef struct __attribute__((packed)) {
  // Header do pacote (8 Bytes)
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
} */
