#pragma once
#include <Arduino.h>

class Botao {
private:
  uint8_t  pino;
  uint32_t tempoDebounce;
  uint32_t ultimoTempo;
  int      estadoAnterior;
  int      estadoValidado;

public:
  Botao(uint8_t pinoEscolhido, uint32_t tempoFiltro = 20)
    : pino(pinoEscolhido), tempoDebounce(tempoFiltro),
      ultimoTempo(0), estadoAnterior(HIGH), estadoValidado(HIGH) {}

  bool foiClicado() {
    bool ocorreuClique = false;
    int  leitura = digitalRead(pino);

    if (leitura != estadoAnterior) {
      ultimoTempo = millis();
    }

    if ((millis() - ultimoTempo) > tempoDebounce) {
      if (leitura != estadoValidado) {
        estadoValidado = leitura;
        if (estadoValidado == LOW) ocorreuClique = true;
      }
    }
    estadoAnterior = leitura;
    return ocorreuClique;
  }
};

// Instâncias globais – definidas em menu.cpp
extern Botao botaoTravar;
extern Botao botaoMenu;
extern Botao botaoCima;
extern Botao botaoBaixo;
extern Botao botaoLerSd;
