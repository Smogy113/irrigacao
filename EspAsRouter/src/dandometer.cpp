//código para leitura de dandômetro
#include <Arduino.h>

const int timeDelay = 30000; //tempo entre leituras em milissegundos
unsigned long lastTime = 0; //variável para armazenar o tempo da última leitura
int numLeituras = 50;
unsigned long somaLeituras = 0;
float posAtual = 0;
int adc = 4095;
float maxDiameter = 50.8;
float deslocamento = 0;

float dandometerSetup(int dandPin) {
    if (millis() - lastTime < timeDelay) {
        somaLeituras = 0;

        for (int i =0; i < numLeituras; i++){
            somaLeituras += analogRead(dandPin);    
        }

        float mediaAdc = (float)somaLeituras/numLeituras;
        deslocamento = (mediaAdc/adc) * maxDiameter;
        return deslocamento;

    }
}
