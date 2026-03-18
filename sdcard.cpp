#include "sdcard.h"
#include "config.h"

void lerArquivo() {
  if (interfaceAtual != PROGRAMADA) return;

  // Só relê se necessário
  if (!estadoDeLeitura) {
    bool dataFutura  = strcmp(buffer, dataHoje) > 0;
    bool mesmoHoje   = strcmp(buffer, dataHoje) == 0 && !indicadorDiaTerminado;
    if (dataFutura || mesmoHoje) return;
  } else {
    estadoDeLeitura = false;
  }

  // Cria arquivo com conteúdo padrão se não existir
  if (!SD.exists("/data.txt")) {
    File f = SD.open("/data.txt", FILE_WRITE);
    if (f) { f.print(LINHAS_PADRAO); f.close(); }
    else   { Serial.println("Erro ao criar data.txt"); }
  }

  File f = SD.open("/data.txt");
  if (!f) { Serial.println("Erro ao abrir data.txt"); return; }

  digitalWrite(PIN_LED, HIGH);

  while (f.available()) {
    conteudoDaLinha = f.readStringUntil('\n').c_str();

    String primeiroDado;
    if (!conteudoDaLinha.empty()) {
      size_t idx = conteudoDaLinha.find(',');
      primeiroDado = conteudoDaLinha.substr(0, idx).c_str();
    }

    strncpy(buffer, primeiroDado.c_str(), sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    if (indicadorDiaTerminado) { indicadorDiaTerminado = false; continue; }
    if (strcmp(buffer, dataHoje) == 0) break;
    if (strcmp(buffer, dataHoje) > 0)  break;
  }

  digitalWrite(PIN_LED, LOW);
  f.close();
}

void registrarLogRegaConcluida() {
  DateTime fimDaRega  = rtc.now();
  TimeSpan duracaoReal = fimDaRega - inicioDaRega;

  File logFile = SD.open("/logdata.txt", FILE_WRITE);
  if (!logFile) {
    Serial.println("ERRO: Falha ao abrir /logdata.txt");
    return;
  }

  char linha[100];
  sprintf(linha,
    "%04d/%02d/%02d, Inicio: %02d:%02d:%02d, Fim: %02d:%02d:%02d, Duracao: %ld seg\n",
    inicioDaRega.year(),  inicioDaRega.month(),  inicioDaRega.day(),
    inicioDaRega.hour(),  inicioDaRega.minute(), inicioDaRega.second(),
    fimDaRega.hour(),     fimDaRega.minute(),    fimDaRega.second(),
    duracaoReal.totalseconds());

  logFile.print(linha);
  logFile.close();

  Serial.print("Log registrado: ");
  Serial.print(linha);
}
