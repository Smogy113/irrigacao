#include "config.h"
#include "display.h"
#include "agenda.h"
#include "rega.h"
#include "menu.h"
#include "sdcard.h"
#include <esp_task_wdt.h>

// Tempo em segundos para o WDT estourar e reiniciar o ESP32
#define WDT_TIMEOUT 15

void setup() {
  Serial.begin(115200);

  // Inicializa o WDT com o timeout definido e habilita o panic (reboot automático)
  esp_task_wdt_init(WDT_TIMEOUT, true); 
  esp_task_wdt_add(NULL); // Adiciona a thread atual (loop principal) ao WDT

  pinMode(PIN_BOMBA,         OUTPUT); digitalWrite(PIN_BOMBA,         LOW);
  pinMode(PIN_SOLENOIDE,     OUTPUT); digitalWrite(PIN_SOLENOIDE,     LOW);
  pinMode(PIN_LED,           OUTPUT); digitalWrite(PIN_LED,           LOW);
  pinMode(PIN_LED_TRAVADO,   OUTPUT); digitalWrite(PIN_LED_TRAVADO,   LOW);
  pinMode(PIN_LED_IRRIGANDO, OUTPUT); digitalWrite(PIN_LED_IRRIGANDO, LOW);
  pinMode(PIN_LED_FORCADA,   OUTPUT); digitalWrite(PIN_LED_FORCADA,   LOW);

  pinMode(PIN_BTN_TRAVAR, INPUT_PULLUP);
  pinMode(PIN_BTN_MENU,   INPUT_PULLUP);
  pinMode(PIN_BTN_CIMA,   INPUT_PULLUP);
  pinMode(PIN_BTN_BAIXO,  INPUT_PULLUP);
  pinMode(PIN_BTN_LER_SD, INPUT_PULLUP);

  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("INICIANDO...");

  if (!rtc.begin()) {
    Serial.println("Erro: RTC nao encontrado! Reiniciando em instantes...");
    delay(WDT_TIMEOUT * 1000 + 1000); // Espera o WDT resetar a placa
  }
  Serial.println("RTC iniciado");
  
  
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(__DATE__, __TIME__));
    //rtc.adjust(DateTime(2026, 3, 20, 23, 58, 50)); // (Ano, Mês, Dia, Hora, Minuto, Segundo)
    //rtc.adjust(DateTime(__DATE__, "23:58:50"));
  }

  SPI.begin(18, 17, 5, 19);   // SCK, MISO, MOSI, CS
  
  if (!SD.begin(PIN_CHIP_SELECT)) {
    Serial.println("Falha no SD!");
  }
  Serial.println("SD iniciado");
  verificarRegaInterrompida();


  lcd.clear();
  agora = rtc.now();
}

void loop() {
  // Alimenta o cão de guarda no início ou fim do loop
  esp_task_wdt_reset();

  formatarLcd();
  atualizarAgenda();
  gerenciarRega();
  verificarAgenda();
  monitorarSD();
}
