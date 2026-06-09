#include <Arduino.h>
#include <cstdlib>

int forceData() {
    // Gera um número aleatório entre 0 e 4095 (resolução do ADC do ESP32)
    return rand()%4096;
}

