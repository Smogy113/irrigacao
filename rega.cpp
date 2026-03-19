#include "rega.h"
#include "config.h"
#include "sdcard.h"
#include "menu.h"

void executarRega(int segundos) {
  if (estadoAtual != OCIOSO) return;

  duracaoRega = (unsigned long)segundos * 1000UL;

  if (estadoSolenoide == DESLIGADO) {
    Serial.println("SOLENOIDE LIBERADO");
    digitalWrite(PIN_SOLENOIDE, HIGH);
    estadoSolenoide = LIGADO;
    estadoAtual     = ABRINDO_SOLENOIDE;
    tempoInicio     = millis();
  } else {
    Serial.println("BOMBA LIGADA");
    digitalWrite(PIN_BOMBA, HIGH);
    estadoBomba  = LIGADO;
    tempoInicio  = millis();
    inicioDaRega = rtc.now();
    estadoAtual  = REGANDO;
    strcpy(ultimoBuffer, "1900/01/01");
  }
}

void gerenciarRega() {
  alternarBotao();   // leitura dos botões vem do módulo menu

  if (interfaceAtual == MENU) return;

  // Desativa travas se virou o dia
  if (strcmp(bufferParado, dataHoje) != 0 && estadoBotao) {
    estadoBotao = false;
    digitalWrite(PIN_LED_TRAVADO,    LOW);
  }

  if (!estadoBotao && estadoAtual == OCIOSO) return;

  unsigned long agora_ms = millis();

  switch (estadoAtual) {
    case ABRINDO_SOLENOIDE:
      if (estadoBotao) {
        estadoAtual = ESPERANDO_SOLENOIDE;
        tempoInicio = agora_ms;
        return;
      }
      if (agora_ms - tempoInicio >= ATRASO_SOLENOIDE) {
        Serial.println("BOMBA LIGADA");
        digitalWrite(PIN_BOMBA, HIGH);
        estadoBomba  = LIGADO;
        tempoInicio  = agora_ms;
        inicioDaRega = rtc.now();
        estadoAtual  = REGANDO;
      }
      break;

    case REGANDO:
      if (agora_ms - tempoInicio >= duracaoRega || estadoBotao) {
        digitalWrite(PIN_BOMBA, LOW);
        Serial.println("BOMBA DESLIGADA");
        strcpy(ultimoBuffer, "1900/01/01");
        registrarLogRegaConcluida();
        if (regaForcadaAtiva) {
          atualizarProximaIrrigacaoForcada();
        } else {
          strcpy(buffer,   "1900/01/01");
          strcpy(horaInicio, "25:00:00");
        }
        tempoInicio = agora_ms;
        estadoBomba = DESLIGADO;
        estadoAtual = ESPERANDO_SOLENOIDE;
      }
      break;

    case ESPERANDO_SOLENOIDE:
      if (agora_ms - tempoInicio >= ATRASO_SOLENOIDE) {
        if (estadoBomba == DESLIGADO) {
          digitalWrite(PIN_SOLENOIDE, LOW);
          Serial.println("SOLENOIDE FECHADO");
          estadoSolenoide = DESLIGADO;
        }
        estadoAtual = OCIOSO;
        strcpy(ultimoBuffer, "1900/01/01");
      }
      break;

    default:
      break;
  }
}

void atualizarProximaIrrigacaoForcada() {
  Serial.println("atualizarProximaIrrigacaoForcada()");

  if (interfaceAtual != IRRIGACAO_FORCADA && !regaForcadaAtiva) return;

  duracaoEmSegundos   = duracaoRegaForcado * 60;
  intervaloEmSegundos = (uint32_t)horaRegaForcado * 3600;
  
  agora = rtc.now();
  DateTime tempoFuturo = agora + TimeSpan((int32_t)intervaloEmSegundos);

  char novaHora[9], novaData[11];
  snprintf(novaHora, sizeof(novaHora), "%02d:%02d:%02d",
  tempoFuturo.hour(), tempoFuturo.minute(), tempoFuturo.second());
  snprintf(novaData, sizeof(novaData), "%04d/%02d/%02d",
  tempoFuturo.year(), tempoFuturo.month(), tempoFuturo.day());
  
  strcpy(buffer,    novaData);
  strcpy(horaInicio, novaHora);
  snprintf(duracaoStr, sizeof(duracaoStr), "%d", duracaoEmSegundos);
  strcpy(ultimoBuffer, "1900/01/01");

  Serial.print("Proxima rega forcada: ");
  Serial.print(buffer);
  Serial.print(" ");
  Serial.println(horaInicio);
}
