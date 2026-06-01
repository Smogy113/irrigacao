#include "sdcard.h"
#include "config.h"

// Tenta (re)inicializar o SD se necessário
static bool garantirSD() {
  File root = SD.open("/");
  if (root) { root.close(); return true; }

  Serial.println("SD não montado, tentando reinicializar...");
  SD.end();
  SPI.begin(18, 17, 5, 19);   // ← precisa reconfigurar após SD.end()
  if (!SD.begin(19)) {
    Serial.println("Falha ao reinicializar SD");
    return false;
  }
  Serial.println("SD reinicializado com sucesso");
  return true;
}

void lerArquivo() {
  if (interfaceAtual != PROGRAMADA) return;

  // 1. Gatilho de Meia-Noite: Libera nova busca no início de um novo dia
  static int ultimoDiaLido = -1;
  if (agora.day() != ultimoDiaLido) {
    ultimoDiaLido = agora.day();
    estadoDeLeitura = true; // Força uma nova varredura pelo SD
  }

  // 2. Trava de Leitura (Early-Return)
  if (!estadoDeLeitura) {
    bool dataFutura   = strcmp(buffer, dataHoje) > 0;
    bool mesmoHoje    = strcmp(buffer, dataHoje) == 0 && !indicadorDiaTerminado;
    bool fimDeArquivo = (buffer[0] == '\0'); // Impede loops infinitos se o SD não tiver mais regas futuras

    // Se já temos a rega de amanhã, ou ainda temos regas hoje, ou o arquivo simplesmente acabou: PAUSA O SD.
    if (dataFutura || mesmoHoje || fimDeArquivo) return;
  } else {
    estadoDeLeitura = false;
  }

  digitalWrite(PIN_LED, HIGH);
  if (!garantirSD()) { digitalWrite(PIN_LED, LOW); return; }
  File f = SD.open("/data.txt");
  if (!f) { Serial.println("Erro ao abrir data.txt"); digitalWrite(PIN_LED, LOW); return; }

  bool pulou = false;

  // Varredura do arquivo usando C-strings (zero alocação dinâmica)
  while (f.available()) {
    // Lê os dados do arquivo diretamente para o vetor estático
    size_t bytesLidos = f.readBytesUntil('\n', conteudoDaLinha, sizeof(conteudoDaLinha) - 1);
    conteudoDaLinha[bytesLidos] = '\0'; // Finaliza a string com caractere nulo

    // Remove o '\r' (retorno de carro) caso exista
    char* cr = strchr(conteudoDaLinha, '\r');
    if (cr) *cr = '\0';
    
    // Ignora linhas vazias
    if (bytesLidos == 0 || conteudoDaLinha[0] == '\0') continue;

    // Busca o primeiro dado (Data) até a primeira vírgula
    char* primeiraVirgula = strchr(conteudoDaLinha, ',');
    if (primeiraVirgula) {
      size_t tamDado = primeiraVirgula - conteudoDaLinha;
      if (tamDado >= sizeof(buffer)) tamDado = sizeof(buffer) - 1;
      strncpy(buffer, conteudoDaLinha, tamDado);
      buffer[tamDado] = '\0';
    } else {
      // Se não houver vírgula, a linha inteira é a data
      strncpy(buffer, conteudoDaLinha, sizeof(buffer) - 1);
      buffer[sizeof(buffer) - 1] = '\0';
    }

    if (strcmp(buffer, dataHoje) == 0 && indicadorDiaTerminado) {
      indicadorDiaTerminado = false;
      pulou = true;
      continue; 
    }
    if (strcmp(buffer, dataHoje) == 0) break;
    if (strcmp(buffer, dataHoje) > 0)  break;
  }

  // 3. Tratamento blindado de Fim de Arquivo (EOF)
  if (!f.available() && strcmp(buffer, dataHoje) < 0) {
    // Arquivo inteiro lido e só tem datas antigas
    conteudoDaLinha[0] = '\0';
    buffer[0]     = '\0';
    horaInicio[0] = '\0';
    duracaoStr[0] = '\0';
    Serial.println("[SD] Sem novas irrigacoes para o futuro. Leitura bloqueada.");
  } else if (pulou && strcmp(buffer, dataHoje) <= 0) {
    // Pulou as de hoje e não achou mais nada
    conteudoDaLinha[0] = '\0';
    buffer[0]     = '\0';
    horaInicio[0] = '\0';
    duracaoStr[0] = '\0';
    Serial.println("[SD] Irrigacoes do dia concluidas. Leitura pausada ate amanha.");
  } else if (pulou) {
    // Achou o dia seguinte com sucesso
    Serial.print("[SD] Proximo dia carregado para memoria: ");
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

  File logFile = SD.open("/logdata.txt", FILE_APPEND);
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

void salvarRegaAtiva() {
  if (!garantirSD()) return;

  DateTime agora_rtc = rtc.now();
  TimeSpan decorrido = agora_rtc - inicioDaRega;

  SD.remove("/rega_ativa.txt");
  File f = SD.open("/rega_ativa.txt", FILE_WRITE);
  if (!f) return;

  char linha[40];
  sprintf(linha, "%04d/%02d/%02d,%02d:%02d:%02d,%ld",
    inicioDaRega.year(), inicioDaRega.month(), inicioDaRega.day(),
    inicioDaRega.hour(), inicioDaRega.minute(), inicioDaRega.second(),
    decorrido.totalseconds());
  f.print(linha);
  f.close();
}

void finalizarRegaAtiva() {
  if (!garantirSD()) return;
  SD.remove("/rega_ativa.txt");
}

void verificarRegaInterrompida() {
  if (!garantirSD()) return;
  if (!SD.exists("/rega_ativa.txt")) return;

  File f = SD.open("/rega_ativa.txt");
  if (!f) return;
  String linha = f.readStringUntil('\n');
  f.close();

  // Calcula duração até o momento do boot como estimativa
  DateTime inicio;
  int y, mo, d, h, mi, s;
  sscanf(linha.c_str(), "%d/%d/%d,%d:%d:%d", &y, &mo, &d, &h, &mi, &s);
  inicio = DateTime(y, mo, d, h, mi, s);

  DateTime agora_rtc = rtc.now();
  TimeSpan duracao = agora_rtc - inicio;

  File logFile = SD.open("/logdata.txt", FILE_APPEND);
  if (logFile) {
    char log[50];
    sprintf(log, "##%04d/%02d/%02d,%02d:%02d:%02d,%ld\n",
      inicio.year(), inicio.month(), inicio.day(),
      inicio.hour(), inicio.minute(), inicio.second(),
      duracao.totalseconds());
    logFile.print(log);
    logFile.close();
    Serial.print("[BOOT] Rega interrompida registrada: ");
    Serial.print(log);
  }

  SD.remove("/rega_ativa.txt");
}

void monitorarSD() {
  unsigned long agora_ms = millis();
  static unsigned long ultimoPing = 0;
  static bool sdPresente = true; // Assume true no boot, pois o setup já inicia o SD

  // Executa o ping leve a cada 3 segundos (3000 ms)
  if (agora_ms - ultimoPing < 3000) return;
  ultimoPing = agora_ms;

  if (sdPresente) {
    // Ping leve: tenta abrir a raiz para confirmar se o cartão físico continua lá
    File root = SD.open("/");
    if (!root) {
      Serial.println("[SD MONITOR] Cartao removido!");
      sdPresente = false;
    } else {
      root.close();
    }
  } else {
    // Se o cartão estava fora, limpa a memória VFS e tenta remontar
    SD.end();
    SPI.begin(18, 17, 5, 19);
    
    if (SD.begin(19)) {
      Serial.println("[SD MONITOR] Cartao reinserido! Atualizando agenda automaticamente...");
      sdPresente = true;
      
      // Essa é a mágica: levanta as flags exatas que a sua função lerArquivo() 
      // já usa para forçar o carregamento de uma nova programação!
      estadoDeLeitura = true;         
      indicadorDiaTerminado = false;  
    }
  }
}