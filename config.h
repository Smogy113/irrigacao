#pragma once

#include <Arduino.h>
#include <RTClib.h>
#include <SD.h>
#include <LiquidCrystal_I2C.h>
#include <string>

// ─── Pinos ────────────────────────────────────────────────────────────────────
const int PIN_BOMBA      = 26;
const int PIN_SOLENOIDE  = 25;
const int PIN_CHIP_SELECT = 5;
const int PIN_LED        = 33;

const int PIN_BTN_TRAVAR  = 14;
const int PIN_BTN_MENU    = 13;
const int PIN_BTN_CIMA    = 32;
const int PIN_BTN_BAIXO   = 27;
const int PIN_BTN_LER_SD  = 12;

// ─── Enums ────────────────────────────────────────────────────────────────────
enum Estado       { OCIOSO, ABRINDO_SOLENOIDE, REGANDO, ESPERANDO_SOLENOIDE };
enum LigadoDesligado { LIGADO, DESLIGADO };
enum Interface    { PROGRAMADA, MENU, IRRIGACAO_FORCADA };

// ─── Timing ───────────────────────────────────────────────────────────────────
const unsigned long ATRASO_SOLENOIDE        = 2000;
const unsigned long INTERVALO_AGENDA        = 500;
const unsigned long INTERVALO_ATUALIZAR_AGENDA = 500;

// ─── Conteúdo padrão do SD ───────────────────────────────────────────────────
extern const char LINHAS_PADRAO[];

// ─── Estado global ────────────────────────────────────────────────────────────
extern Interface     interfaceAtual;
extern Interface     ultimaInterfaceAtual;
extern Estado        estadoAtual;
extern LigadoDesligado estadoSolenoide;
extern LigadoDesligado estadoBomba;

extern DateTime agora;
extern DateTime inicioDaRega;
extern RTC_DS1307 rtc;
extern LiquidCrystal_I2C lcd;

// Buffers de tempo/data
extern char horaInicio[9];
extern char duracaoStr[20];
extern char dataP[11];
extern char buffer[11];
extern char dataHoje[11];
extern char bufferParado[11];
extern char ultimoBufferParado[11];
extern char ultimaData[11];
extern char ultimoBuffer[11];
extern char ultimaHora[9];
extern char ultimaDuracao[20];
extern char horaAgora[9];
extern char resposta[9];

extern std::string conteudoDaLinha;

// Flags
extern bool indicadorDiaTerminado;
extern bool estadoBotao;
extern bool ultimoEstadoBotao;
extern bool estadoDeLeitura;
extern bool duracaoJaSelecionada;
extern bool regaForcadaAtiva;
extern bool voltarParaProgramada;

// Timers
extern unsigned long tempoInicio;
extern unsigned long duracaoRega;
extern unsigned long ultimoTempoAgenda;
extern unsigned long ultimoTempoFormatar;
extern unsigned long tempoAnteriorAtualizarAgenda;
extern unsigned long tempoAnteriorVoltarProgramada;
extern unsigned long tempoVoltarProgramada;

// Menu / rega forçada
extern int  menuSelecionado;
extern int  duracaoEmSegundos;
extern unsigned int duracaoRegaForcado;
extern unsigned int horaRegaForcado;
extern uint32_t ultimoTempoRegaForcada;
extern uint32_t intervaloEmSegundos;
