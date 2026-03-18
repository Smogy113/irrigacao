#include "agenda.h"
#include "config.h"
#include "sdcard.h"
#include "rega.h"

void verificarAgenda() {
  unsigned long agora_ms = millis();
  static unsigned long ultimo = 0;

  if (agora_ms - ultimo < INTERVALO_AGENDA) return;
  ultimo = agora_ms;

  lerArquivo();

  if (buffer[0] == '\0' || strcmp(buffer, dataHoje) != 0) {
    proximaIrrigacao();
    return;
  }

  // Hoje tem irrigação agendada
  if (estadoBotao && interfaceAtual == PROGRAMADA) {
    proximaIrrigacao();
    return;
  }

  if (horaInicio[0] != '\0' && strcmp(horaInicio, horaAgora) <= 0) {
    int tempoRega = atoi(duracaoStr);
    if (tempoRega > 0 && estadoAtual == OCIOSO) {
      executarRega(tempoRega);
      return;
    }
  } else if (horaInicio[0] != '\0') {
    // Pré-abre solenoide até 2 s antes da rega
    char horaCopia[9];
    strcpy(horaCopia, horaInicio);

    char* h = strtok(horaCopia, ":");
    char* m = strtok(NULL,      ":");
    char* s = strtok(NULL,      ":");

    if (h && m && s) {
      long diffSeg = ((long)atoi(h) * 3600L + (long)atoi(m) * 60L + atoi(s))
                   - ((long)agora.hour() * 3600L + (long)agora.minute() * 60L + agora.second());

      if (diffSeg >= 0 && diffSeg <= 2 && estadoSolenoide == DESLIGADO) {
        Serial.println("SOLENOIDE LIBERADO (pre-abertura)");
        digitalWrite(PIN_SOLENOIDE, HIGH);
        estadoSolenoide = LIGADO;
      }
    }
  }

  proximaIrrigacao();
}

void proximaIrrigacao() {
  if (interfaceAtual != PROGRAMADA) return;
  if (strcmp(buffer, dataHoje) < 0)  return;

  size_t pos = conteudoDaLinha.find(',');
  if (pos == std::string::npos) return;
  pos++;

  size_t posVirgulaDuracao;
  do {
    size_t posVirgulaHora  = conteudoDaLinha.find(',', pos);
    size_t tamanhoHora     = posVirgulaHora - pos;
    strncpy(horaInicio, conteudoDaLinha.c_str() + pos, tamanhoHora);
    horaInicio[tamanhoHora] = '\0';

    pos = posVirgulaHora + 1;
    posVirgulaDuracao = conteudoDaLinha.find(',', pos);

    const size_t TAM_DUR = 4;
    strncpy(duracaoStr, conteudoDaLinha.c_str() + pos, TAM_DUR);
    duracaoStr[TAM_DUR] = '\0';

    if (posVirgulaDuracao != std::string::npos) {
      pos = posVirgulaDuracao + 1;
    }

    if (strcmp(buffer, dataHoje) > 0) break;

  } while (posVirgulaDuracao != std::string::npos &&
           strcmp(horaInicio, horaAgora) < 0);

  // Dia encerrado – sem mais horários
  if (horaInicio[0] == '\0' || strcmp(horaInicio, horaAgora) < 0) {
    if (estadoAtual == OCIOSO) indicadorDiaTerminado = true;
  }
}
