//Arquivo dedicado para a configuração partircular de cada sensor, para que seja feita sua leitura e retorne dados úteis.
#include <Arduino.h>


int configSAI1(const int senPin) {
    //Configuração individual para este sensor em específico
    return analogRead(senPin);
}


float configSAF1(const int senPin) {
    //Sensor de umidade de solo, retorna um valor entre 0 e 100 representando a porcentagem de umidade
    //Configuração individual para este sensor em específico
    return (analogRead(senPin) / 4095.0) * 100.0;
}

