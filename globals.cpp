#include "config.h"

const char LINHAS_PADRAO[] =
  "1026/03/04,13:47:50,0005,13:50:50,0005,18:47:50,0005\n"
  "2026/003/06,17:14:50,0005\n"
  "2026/13/08,21:30:00,1501,22:00:00,1601\n"
  "2026/03/35,13:42:20,0010,13:23:40,0010,23:36:20,0010\n"
  "2026/03/17,13:42:20,0010,21:54:05,0010,23:36:20,0010\n";

LiquidCrystal_I2C lcd(0x27, 20, 4);
RTC_DS1307        rtc;

Interface      interfaceAtual       = PROGRAMADA;
Interface      ultimaInterfaceAtual = PROGRAMADA;
Estado         estadoAtual          = OCIOSO;
LigadoDesligado estadoSolenoide     = DESLIGADO;
LigadoDesligado estadoBomba         = DESLIGADO;

DateTime agora;
DateTime inicioDaRega;

char horaInicio[9]          = "";
char duracaoStr[20]         = "";
char dataP[11]              = "";
char buffer[11]             = "";
char dataHoje[11]           = "";
char bufferParado[11]       = "";
char ultimoBufferParado[11] = "";
char ultimaData[11]         = "";
char ultimoBuffer[11]       = "";
char ultimaHora[9]          = "";
char ultimaDuracao[20]      = "";
char horaAgora[9]           = "";
char resposta[9]            = "";

std::string conteudoDaLinha;

bool indicadorDiaTerminado  = false;
bool estadoBotao            = false;
bool ultimoEstadoBotao      = false;
bool estadoDeLeitura        = false;
bool duracaoJaSelecionada   = false;
bool regaForcadaAtiva       = false;
bool voltarParaProgramada   = false;

unsigned long tempoInicio                   = 0;
unsigned long duracaoRega                   = 0;
unsigned long ultimoTempoAgenda             = 0;
unsigned long ultimoTempoFormatar           = 0;
unsigned long tempoAnteriorAtualizarAgenda  = 0;

 
int  menuSelecionado  = 0;
int  duracaoEmSegundos = 0;
unsigned int duracaoRegaForcado = 30;
unsigned int horaRegaForcado    = 1;
uint32_t ultimoTempoRegaForcada = 0;
uint32_t intervaloEmSegundos    = 0;
