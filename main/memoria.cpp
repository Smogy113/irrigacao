#include <Preferences.h>
#include "config.h"

void carregarConfiguracoesForcidas() {
  Preferences preferences;
  
  // Abre o namespace "rega_forcada" em modo apenas leitura (true)
  preferences.begin("rega_forcada", true);
  
  // Recupera os valores salvos. Se a chave não existir (primeiro boot), 
  // assume automaticamente os valores padrão de 30 minutos e 1 hora.
  duracaoRegaForcado = preferences.getUInt("dur_forcado", 30);
  horaRegaForcado    = preferences.getUInt("hora_forcado", 1);
  
  preferences.end();
  Serial.printf("[NVS] Configurações carregadas: %u min, %u h\n", duracaoRegaForcado, horaRegaForcado);
}

void salvarConfiguracoesForcidas() {
  Preferences preferences;
  
  // Abre o namespace em modo de leitura e escrita (false)
  preferences.begin("rega_forcada", false);
  
  preferences.putUInt("dur_forcado", duracaoRegaForcado);
  preferences.putUInt("hora_forcado", horaRegaForcado);
  
  preferences.end();
  Serial.println("[NVS] Novas configurações forçadas salvas na Flash!");
}