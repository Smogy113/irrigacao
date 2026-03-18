#include "display.h"
#include "config.h"

// Converte horaInicio + duracaoStr em horário de fim → armazena em resposta[]
void conversorDeHorario() {
  int horaIrrigacao, minutoIrrigacao, segundoIrrigacao;
  sscanf(horaInicio, "%d:%d:%d", &horaIrrigacao, &minutoIrrigacao, &segundoIrrigacao);

  int valorDuracaoStr = atoi(duracaoStr);
  int totalSegundos   = valorDuracaoStr
                      + (horaIrrigacao * 3600 + minutoIrrigacao * 60 + segundoIrrigacao);

  int h = (totalSegundos / 3600) % 24;
  int m = (totalSegundos % 3600) / 60;
  int s = totalSegundos % 60;

  sprintf(resposta, "%02d:%02d:%02d", h, m, s);
}

// Atualiza data/hora no LCD (linha 0) e variáveis dataHoje/horaAgora
void atualizarAgenda() {
  unsigned long agora_ms = millis();
  static unsigned long ultimo = 0;

  if (agora_ms - ultimo < INTERVALO_ATUALIZAR_AGENDA) return;
  ultimo = agora_ms;

  agora = rtc.now();
  sprintf(dataHoje, "%04d/%02d/%02d", agora.year(), agora.month(), agora.day());
  sprintf(horaAgora, "%02d:%02d:%02d", agora.hour(), agora.minute(), agora.second());

  if (interfaceAtual == MENU || voltarParaProgramada) return;

  lcd.setCursor(0, 0);  lcd.print(dataHoje);
  lcd.setCursor(12, 0); lcd.print(horaAgora);
}

// Renderiza linhas 1-3 do LCD (próxima irrigação / status da rega)
void formatarLcd() {
  if (interfaceAtual == MENU || voltarParaProgramada) return;

  unsigned long agora_ms = millis();
  static unsigned long ultimo = 0;

  if (agora_ms - ultimo < INTERVALO_AGENDA) return;
  ultimo = agora_ms;

  int cmpData = strcmp(buffer, dataHoje);

  lcd.setCursor(0, 1);
  lcd.print("-Proxima  irrigacao-");

  // Evita redesenho desnecessário
  if (strcmp(ultimaData,        dataHoje)         == 0 &&
      strcmp(ultimoBuffer,      buffer)           == 0 &&
      strcmp(ultimaHora,        horaInicio)       == 0 &&
      strcmp(ultimaDuracao,     duracaoStr)       == 0 &&
      ultimaInterfaceAtual                        == interfaceAtual &&
      strcmp(ultimoBufferParado, bufferParado)    == 0 &&
      ultimoEstadoBotao                           == estadoBotao) return;

  strcpy(ultimoBufferParado,  bufferParado);
  strcpy(ultimaData,          dataHoje);
  strcpy(ultimoBuffer,        buffer);
  strcpy(ultimaHora,          horaInicio);
  strcpy(ultimaDuracao,       duracaoStr);
  ultimaInterfaceAtual = interfaceAtual;
  ultimoEstadoBotao    = estadoBotao;

  // Data/hora anterior ao dia atual – sem irrigação programada
  if ((cmpData < 0 && interfaceAtual == PROGRAMADA) ||
      (cmpData == 0 && estadoAtual == OCIOSO &&
       strcmp(horaInicio, horaAgora) < 0 && interfaceAtual == PROGRAMADA)) {
    lcd.setCursor(0, 2);  lcd.print("----/--/--");
    lcd.setCursor(12, 2); lcd.print("--:--:--");
    lcd.setCursor(0, 3);  lcd.print("Em espera   --:--:--");
    return;
  }

  // Ocioso com próxima irrigação definida
  if ((cmpData >= 0 && estadoAtual == OCIOSO && interfaceAtual == PROGRAMADA) ||
      ((estadoAtual == OCIOSO || estadoAtual == ESPERANDO_SOLENOIDE) && interfaceAtual == IRRIGACAO_FORCADA)) {
    lcd.setCursor(0, 2);  lcd.print(buffer);
    lcd.setCursor(12, 2); lcd.print(horaInicio);
    lcd.setCursor(0, 3);
    if (estadoBotao) {
      lcd.print("REGA DIA OFF");
    } else {
      lcd.print("Em espera   ");
      conversorDeHorario();
      lcd.print(resposta);
    }
    return;
  }

  // Regando – modo programado
  if (estadoAtual == REGANDO && interfaceAtual == PROGRAMADA) {
    lcd.setCursor(0, 3);
    lcd.print("Regando     ");
    return;
  }

  // Regando – modo forçado
  if (estadoAtual == REGANDO && interfaceAtual == IRRIGACAO_FORCADA) {
    lcd.setCursor(0, 2);  lcd.print(buffer);
    lcd.setCursor(12, 2); lcd.print(horaInicio);
    lcd.setCursor(0, 3);  lcd.print("Regando     ");
    conversorDeHorario();
    lcd.print(resposta);
  }
}
