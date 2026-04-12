#include "sdcard.h"
#include "config.h"

// Tenta (re)inicializar o SD se necessário
static bool garantirSD() {
  // Testa se o SD já está ok abrindo a raiz
  File root = SD.open("/");
  if (root) {
    root.close();
    return true;
  }
  // SD em estado ruim: reinicializa
  Serial.println("SD não montado, tentando reinicializar...");
  SD.end();
  //delay(200);
  if (!SD.begin(PIN_CHIP_SELECT)) {
    Serial.println("Falha ao reinicializar SD");
    return false;
  }
  Serial.println("SD rei nicializado com sucesso");
  return true;
}

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
  // if (!SD.exists("/data.txt")) {
  //   File f = SD.open("/data.txt", FILE_WRITE);
  //   if (f) { f.print(LINHAS_PADRAO); f.close(); }
  //   else   { Serial.println("Erro ao criar data.txt"); }
  // }

  digitalWrite(PIN_LED, HIGH);
  if (!garantirSD()) { digitalWrite(PIN_LED, LOW); return; }
  File f = SD.open("/data.txt");
  if (!f) { Serial.println("Erro ao abrir data.txt"); digitalWrite(PIN_LED, LOW); return; }


  bool pulou = false;

  while (f.available()) {
    Serial.println("a");
    conteudoDaLinha = f.readStringUntil('\n').c_str();
    conteudoDaLinha.erase(remove(conteudoDaLinha.begin(), conteudoDaLinha.end(), '\r'), conteudoDaLinha.end());
    
    String primeiroDado;
    if (!conteudoDaLinha.empty()) {
      size_t idx = conteudoDaLinha.find(',');
      primeiroDado = conteudoDaLinha.substr(0, idx).c_str();
    }

    strncpy(buffer, primeiroDado.c_str(), sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    if (strcmp(buffer, dataHoje) == 0 && indicadorDiaTerminado) {
      indicadorDiaTerminado = false;
      pulou = true;
      continue; 
    }
    if (strcmp(buffer, dataHoje) == 0) break;
    if (strcmp(buffer, dataHoje) > 0)  break;
  }

  if (pulou && strcmp(buffer, dataHoje) <= 0) {
    conteudoDaLinha = "";
    buffer[0]     = '\0';
    horaInicio[0] = '\0';
    duracaoStr[0] = '\0';
    Serial.println("Sem proximos agendamentos no arquivo");
  } else if (pulou) {
    Serial.print("Proximo dia carregado: ");
    Serial.println(buffer);
  }
  
  digitalWrite(PIN_LED, LOW);
  f.close();
}

void registrarLogRegaConcluida() {
  DateTime fimDaRega  = rtc.now();
  TimeSpan duracaoReal = fimDaRega - inicioDaRega;

  if (!garantirSD()) {
    Serial.println("ERRO: SD não disponível");
    return;
 }

  File logFile = SD.open("/logdata.txt", FILE_WRITE);
  if (!logFile) {
    Serial.println("ERRO: Falha ao abrir /logdata.txt");
    return;
  }

  char linha[50];
  if (interfaceAtual == PROGRAMADA){
  sprintf(linha,
    "%04d/%02d/%02d,%02d:%02d:%02d,%ld\n",
    inicioDaRega.year(),  inicioDaRega.month(),  inicioDaRega.day(),
    inicioDaRega.hour(),  inicioDaRega.minute(), inicioDaRega.second(),
    duracaoReal.totalseconds());
  }
  else if (interfaceAtual == IRRIGACAO_FORCADA) {
  sprintf(linha,
    "#%04d/%02d/%02d,%02d:%02d:%02d,%ld\n",
    inicioDaRega.year(),  inicioDaRega.month(),  inicioDaRega.day(),
    inicioDaRega.hour(),  inicioDaRega.minute(), inicioDaRega.second(),
    duracaoReal.totalseconds());
  }

  logFile.print(linha);
  logFile.close();

  Serial.print("Log registrado: ");
  Serial.print(linha);
}
