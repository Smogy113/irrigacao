#include "config.h"
#include "display.h"
#include "agenda.h"
#include "rega.h"
#include "menu.h"

void setup() {
  Serial.begin(115200);

  pinMode(PIN_BOMBA,         OUTPUT); digitalWrite(PIN_BOMBA,         LOW);
  pinMode(PIN_SOLENOIDE,     OUTPUT); digitalWrite(PIN_SOLENOIDE,     HIGH);
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
    Serial.println("Erro ao iniciar o RTC");
    while (1);
  }
  Serial.println("RTC iniciado");
  if (!rtc.lostPower()) {
    rtc.adjust(DateTime(__DATE__, __TIME__));
    //rtc.adjust(DateTime(2026, 3, 20, 23, 58, 50)); // (Ano, Mês, Dia, Hora, Minuto, Segundo)
    //rtc.adjust(DateTime(__DATE__, "23:58:50"));
  }
  rtc.adjust(DateTime(__DATE__, __TIME__));

  if (!SD.begin(PIN_CHIP_SELECT)) {
    Serial.println("Falha no SD!");
  }
  Serial.println("SD iniciado");

  lcd.clear();
  agora = rtc.now();
}

void loop() {
  formatarLcd();
  atualizarAgenda();
  gerenciarRega();
  verificarAgenda();
}
