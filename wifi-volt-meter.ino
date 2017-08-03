#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <stdio.h>

WebSocketsServer webSocket = WebSocketsServer(81);

const char *ssid = "YoshiLinksys";
const char *password = "18092006shubi";

uint8_t lastConnection;
//char *readChar;
char readStringa[] = "1";

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  switch(type) {
    case WStype_DISCONNECTED:
      {
        Serial.printf("[%u] Disconnected!\n", num);
        break;
      }
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        lastConnection = num;
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        webSocket.sendTXT(num, "Connected");
        break;
      }
    case WStype_TEXT:
      {
        String text = String((char *) &payload[0]);
        delay(10);         
        webSocket.sendTXT(num, text);	
        Serial.print(text);
        break;
      }  

    case WStype_BIN:    
      hexdump(payload, lenght);
      // echo data back to browser
      webSocket.sendBIN(num, payload, lenght);
      break;
  }
}

void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.begin(ssid,password);
  Serial.begin(115200);
  Serial.setTimeout(0.05);
  Serial.println("hello world");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
  String textToSend = String(millis());
  webSocket.sendTXT(lastConnection, textToSend);
}
