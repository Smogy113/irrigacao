#include "menu.h"
#include "config.h"
#include "Botao.h"

// ─── Instâncias dos botões ────────────────────────────────────────────────────
Botao botaoTravar(PIN_BTN_TRAVAR);
Botao botaoMenu  (PIN_BTN_MENU);
Botao botaoCima  (PIN_BTN_CIMA);
Botao botaoBaixo (PIN_BTN_BAIXO);
Botao botaoLerSd (PIN_BTN_LER_SD);

// ─── Helpers internos ─────────────────────────────────────────────────────────
static void desenharCursor() {
  const char* linhas[4] = {" ", " ", " ", " "};
  if (menuSelecionado >= 0 && menuSelecionado <= 3)
    linhas[menuSelecionado] = ">";

  for (int i = 0; i < 4; i++) {
    lcd.setCursor(0, i);
    lcd.print(linhas[i]);
  }
}

static void exibirDuracaoPrincipal() {
  lcd.setCursor(1, 2);
  lcd.print(duracaoRegaForcado);
  lcd.print("m de irrigacao   ");
}

static void exibirIntervalo() {
  lcd.setCursor(6, 2);
  lcd.print("a cada ");
  lcd.print(horaRegaForcado);
  lcd.print("h  ");
}

// ─── Implementações públicas ──────────────────────────────────────────────────
void menu() {
  interfaceAtual        = MENU;
  menuSelecionado       = 0;
  duracaoRegaForcado    = 30;
  horaRegaForcado       = 1;
  duracaoJaSelecionada  = false;

  lcd.clear();
  lcd.setCursor(7, 0); lcd.print("Voltar");
  lcd.setCursor(2, 1); lcd.print("^");
  lcd.setCursor(1, 2); lcd.print("30m de irrigacao");
  lcd.setCursor(2, 3); lcd.print("v");
}

void gerenciarMenu() {
  // ── Confirmação para voltar de IRRIGACAO_FORCADA ──────────────────────────
  if (voltarParaProgramada) {
    desenharCursor();
  }

  if (interfaceAtual != MENU) return;

  desenharCursor();

  if (!botaoMenu.foiClicado()) return;

  // ── Processar seleção ─────────────────────────────────────────────────────
  if (menuSelecionado == 0 && !duracaoJaSelecionada) {
    // Voltar para a tela programada
    lcd.clear();
    interfaceAtual = PROGRAMADA;
    digitalWrite(PIN_LED_FORCADA, LOW);
    strcpy(ultimoBuffer, "1900/01/01");
    estadoDeLeitura = !estadoDeLeitura;
    return;
  }

  if (menuSelecionado == 0 && duracaoJaSelecionada) {
    // Voltar ao ajuste de duração
    lcd.clear();
    lcd.setCursor(7, 0); lcd.print("Voltar");
    lcd.setCursor(2, 1); lcd.print("^");
    lcd.setCursor(2, 3); lcd.print("v");
    lcd.setCursor(13, 1); lcd.print(" ");
    lcd.setCursor(13, 3); lcd.print(" ");
    exibirDuracaoPrincipal();
    duracaoJaSelecionada = false;
    return;
  }

  if (!duracaoJaSelecionada) {
    if (menuSelecionado == 1) { duracaoRegaForcado += 15; exibirDuracaoPrincipal(); }
    if (menuSelecionado == 3 && duracaoRegaForcado > 15) { duracaoRegaForcado -= 15; exibirDuracaoPrincipal(); }

    if (menuSelecionado == 2) {
      // Avançar para ajuste de intervalo
      duracaoJaSelecionada = true;
      lcd.setCursor(2, 1);  lcd.print(" ");
      lcd.setCursor(2, 3);  lcd.print(" ");
      lcd.setCursor(13, 1); lcd.print("^");
      lcd.setCursor(13, 3); lcd.print("v");
      lcd.setCursor(1, 2);  lcd.print("                   ");
      exibirIntervalo();
    }
  } else {
    // Fase 2 – ajuste de intervalo
    if (menuSelecionado == 1) { horaRegaForcado += 1; exibirIntervalo(); }
    if (menuSelecionado == 3 && horaRegaForcado > 0) { horaRegaForcado -= 1; exibirIntervalo(); }

    if (menuSelecionado == 2) {
      // Confirmar e ativar irrigação forçada
      interfaceAtual = IRRIGACAO_FORCADA;
      estadoAtual    = OCIOSO;
      digitalWrite(PIN_LED_FORCADA, HIGH);
      lcd.clear();

      duracaoEmSegundos = duracaoRegaForcado * 60;
      duracaoRega       = duracaoRegaForcado * 60;

      agora = rtc.now();
      DateTime tempoFuturo = agora + TimeSpan(0, 0, 0, 20); // comeca dps de 20seg

      char novaHora[9], novaData[11];
      snprintf(novaHora, sizeof(novaHora), "%02d:%02d:%02d",
               tempoFuturo.hour(), tempoFuturo.minute(), tempoFuturo.second());
      snprintf(novaData, sizeof(novaData), "%04d/%02d/%02d",
               tempoFuturo.year(), tempoFuturo.month(), tempoFuturo.day());

      strcpy(buffer,    novaData);
      strcpy(horaInicio, novaHora);
      snprintf(duracaoStr, sizeof(duracaoStr), "%d", duracaoEmSegundos);
      //snprintf(duracaoStr, sizeof(duracaoStr), "%d", 5);


      if (horaRegaForcado != 0) {
        agora = rtc.now();
        regaForcadaAtiva         = true;
        ultimoTempoRegaForcada   = agora.unixtime();
      }

      Serial.print("Rega forcada ativa, intervalo: ");
      Serial.print(horaRegaForcado);
      Serial.println("h");
    }
  }
}

void alternarBotao() {
  gerenciarMenu();

  // ── Botão de trava ────────────────────────────────────────────────────────
  if (botaoTravar.foiClicado()) {
    estadoBotao = !estadoBotao;
    if (estadoBotao) {
      digitalWrite(PIN_LED_TRAVADO,    HIGH);
      strcpy(bufferParado, dataHoje);
    }
    if (!estadoBotao) {
      digitalWrite(PIN_LED_TRAVADO,    LOW);
    }
    Serial.print("Trava: "); Serial.println(estadoBotao);
  }

  // ── Botão Menu ────────────────────────────────────────────────────────────
  if (botaoMenu.foiClicado()) {
    if (interfaceAtual == PROGRAMADA) {
      menu();
    } else if (interfaceAtual == IRRIGACAO_FORCADA) {
      if (!voltarParaProgramada) {
        voltarParaProgramada = true;
        lcd.clear();
        lcd.setCursor(3, 0); lcd.print("Voltar para a");
        lcd.setCursor(1, 1); lcd.print("programacao do SD?");
        lcd.setCursor(4, 2); lcd.print("Sim");
        lcd.setCursor(4, 3); lcd.print("Nao");
        menuSelecionado = 3;
        Serial.println("voltarParaProgramada ativado");
      } else {
        if (menuSelecionado == 2) {
          lcd.clear();
          interfaceAtual = PROGRAMADA;
          digitalWrite(PIN_LED_FORCADA, LOW);
          estadoDeLeitura = !estadoDeLeitura;
          voltarParaProgramada = false;
          regaForcadaAtiva = false;
          strcpy(buffer, "1990/01/01");
          strcpy(horaInicio, "25:00:00");
          strcpy(duracaoStr, "1");
          flagPararIrrigacao = true;
          return;
        }
        if (menuSelecionado == 3) {
          lcd.clear();
          voltarParaProgramada = false;
          strcpy(ultimoBuffer, "1910/01/01");
        }
      }
    }
  }

  // ── Botão Ler SD ──────────────────────────────────────────────────────────
  if (botaoLerSd.foiClicado() && interfaceAtual == PROGRAMADA) {
    estadoDeLeitura = !estadoDeLeitura;
    Serial.print("Leitura SD forçada: "); Serial.println(estadoDeLeitura);
  }

  // ── Navegação no menu ─────────────────────────────────────────────────────
  if (botaoCima.foiClicado()) {
    if (menuSelecionado > 0 && !voltarParaProgramada) {
      menuSelecionado--;
    }
    if (menuSelecionado > 2 && voltarParaProgramada) {
      menuSelecionado--;
    }
  }
  if (botaoBaixo.foiClicado() && menuSelecionado < 3) menuSelecionado++;
}
