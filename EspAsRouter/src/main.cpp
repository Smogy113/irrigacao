#include <Arduino.h>
#include "LittleFS.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>


const char* ssid = "ESP32_lab3i";
const char* key = "123456789";



const int dand = 34;

//especificações do EcoMAtik
const float maxDiameter = 50.8;
const int adc = 4095;
float deslocamento = 0;


//cria o objeto AsyncWebServer na porta 80
AsyncWebServer server(80);

//cria o objeto WebSocket
AsyncWebSocket ws("/ws");

//variável em json para manter a leitura dos sensores
JSONVar readings;

// variáveis de tempo
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;



// Get Sensor Readings and return JSON object
String getSensorReadings(){
  //colocar o sensor que vai ser lido
  readings["Diâmetro"] = String(deslocamento);
  //readings["humidity"] =  String(bme.readHumidity());
  //readings["pressure"] = String(bme.readPressure()/100.0F);
  String jsonString = JSON.stringify(readings);
  return jsonString;
}


void notifyClients(String sensorReadings) {
ws.textAll(sensorReadings);
}


void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    String message = (char*)data;
    //Check if the message is "getReadings"
    if (strcmp((char*)data, "getReadings") == 0) {
      //if it is, send current sensor readings
      String sensorReadings = getSensorReadings();
      Serial.print(sensorReadings);
      notifyClients(sensorReadings);
    }
  }
}


void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}


void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}


void setup(){
  Serial.begin(115200);

  pinMode(maxDiameter, INPUT);


  //initWiFi();
  //initLittleFS();
  initWebSocket();


  //Configurando como roteador
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, key);


  //ID do ESP
  Serial.print("Rede Wi-Fi criada! Conecte-se nela e acesse o IP: ");
  Serial.println(WiFi.softAPIP());


  //Checa se o LittleFS está rodando
  if (!LittleFS.begin(true)){
    Serial.println("Erro ao rodar LittleFS.");
    return;
  }


  //Checa se o arquivo existe
  File arquivo = LittleFS.open("/menssage.txt");
  if(!arquivo){
    Serial.println("Erro ao abrir arquivo para leitura.");
    return;
  }


  //Lê e escreve o arquivo no monitor serial
  Serial.println("Conteúdo do arquivo: ");
  while(arquivo.available()){
    Serial.write(arquivo.read());
  }
  arquivo.close();


  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
  request->send(LittleFS, "/index.html", "text/html");
});


server.serveStatic("/", LittleFS, "/");


// Start server
server.begin();


}


void loop(){
  long somaLeituras = 0;
  const int numeroAmostras = 50;

  if ((millis() - lastTime) > timerDelay) {
    String sensorReadings = getSensorReadings();
    Serial.print(sensorReadings);
    notifyClients(sensorReadings);
    lastTime = millis();

    for (int i = 0; i < numeroAmostras; i++){
      somaLeituras += analogRead(dand);
    }
    
    float mediaAdc = (float)somaLeituras/numeroAmostras;
    deslocamento = (mediaAdc/adc) * maxDiameter;


  }
  ws.cleanupClients();


  unsigned long tempoAtual = millis();


}

