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
        tempoAberturaPreventiva = millis();
      }
    }
  }

  proximaIrrigacao();
}

void proximaIrrigacao() {
  if (interfaceAtual != PROGRAMADA) return;
  if (strcmp(buffer, dataHoje) < 0) return;

  // Encontra a primeira vírgula (que separa a data das horas)
  char* pos = strchr(conteudoDaLinha, ',');
  if (pos == NULL) return;
  pos++; // Avança a posição para logo após a vírgula

  char* posVirgulaDuracao;
  do {
    // Procura a vírgula depois da Hora
    char* posVirgulaHora = strchr(pos, ',');
    if (posVirgulaHora == NULL) break; 

    // Copia a Hora (limitando ao tamanho do vetor de destino)
    size_t tamanhoHora = posVirgulaHora - pos;
    if (tamanhoHora >= sizeof(horaInicio)) tamanhoHora = sizeof(horaInicio) - 1;
    strncpy(horaInicio, pos, tamanhoHora);
    horaInicio[tamanhoHora] = '\0';

    // Pula a vírgula para ler a duração
    pos = posVirgulaHora + 1;
    posVirgulaDuracao = strchr(pos, ',');

    // Copia a Duração (se não houver próxima vírgula, vai até o fim da linha)
    size_t tamanhoDuracao;
    if (posVirgulaDuracao != NULL) {
      tamanhoDuracao = posVirgulaDuracao - pos;
    } else {
      tamanhoDuracao = strlen(pos);
    }
    if (tamanhoDuracao >= sizeof(duracaoStr)) tamanhoDuracao = sizeof(duracaoStr) - 1;
    
    strncpy(duracaoStr, pos, tamanhoDuracao);
    duracaoStr[tamanhoDuracao] = '\0';

    // Avança para a próxima rodada, caso exista mais irrigações na linha
    if (posVirgulaDuracao != NULL) {
      pos = posVirgulaDuracao + 1;
    }

    if (strcmp(buffer, dataHoje) > 0) break;

  } while (posVirgulaDuracao != NULL && strcmp(horaInicio, horaAgora) < 0);

  // Dia encerrado – sem mais horários programados válidos para hoje
  if (horaInicio[0] == '\0' || strcmp(horaInicio, horaAgora) < 0) {
    indicadorDiaTerminado = true;
  }
}
