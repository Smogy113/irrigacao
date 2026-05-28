//Arquivo dedicado para a configuração partircular de cada sensor, para que seja feita sua leitura e retorne dados úteis.
#include <Arduino.h>

int configSAI1(const int senPin) {
    //Configuração individual para este sensor em específico
    return analogRead(senPin);
}

float configSAF1(const int senPin) {
    //Configuração individual para este sensor em específico
    return analogRead(senPin);
}