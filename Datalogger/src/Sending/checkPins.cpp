#include <Arduino.h>

//Sensores Analógicos Inteiros
const int senAnIn1 = 36;
const int senAnIn2 = 39;
//Sensores Analógicos Flutuantes
const int senAnFl1 = 34;
const int senAnFl2 = 35;

//Sensores Digitais Inteiros
const int senDiIn1 = 13;
const int senDiIn2 = 14;
//Sensores Digitais Flutuantes
const int senDiFl1 = 16;
const int senDiFl2 = 17;


void defAndCheckPins() {
  pinMode(senAnIn1, INPUT);
  pinMode(senAnIn2, INPUT);
  pinMode(senAnFl1, INPUT);
  pinMode(senAnFl2, INPUT);

  pinMode(senDiIn1, INPUT_PULLDOWN);
  pinMode(senDiIn2, INPUT_PULLDOWN);
  pinMode(senDiFl1, INPUT_PULLDOWN);
  pinMode(senDiFl2, INPUT_PULLDOWN);

  uint8_t sAnList[] = {senAnIn1, senAnIn2, senAnFl1, senAnFl2};
  uint8_t sDiList[] = {senDiIn1, senDiIn2, senDiFl1, senDiFl2};

  Serial.println("----- Verificando Sensores... -----");
  delay(1000);

  for (int i = 0; i < 4; i++) {
    if (analogRead(sAnList[i]) == 0) {
      Serial.println("Nenhum sensor analógico detectado no Pino " + String(sAnList[i]));
    } else {
      Serial.println("Sensor analógico detectado no Pino " + String(sAnList[i]) + " Valor de leitura: " + String(analogRead(sAnList[i])));
    }
  }

  for (int i = 0; i < 4; i++) {
    if (digitalRead(sDiList[i]) == LOW) {
      Serial.println("Nenhum sensor digital detectado no Pino " + String(sDiList[i]));
    } else {
      Serial.println("Sensor digital detectado no Pino " + String(sDiList[i]) + " Valor de leitura: " + String(digitalRead(sDiList[i])));
    }
  }

}

