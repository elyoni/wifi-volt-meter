#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <stdio.h>

WebSocketsServer webSocket = WebSocketsServer(81);

const char *ssid = "YoshiLinksys";
const char *password = "18092006shubi";
const char *ver = "ver08";

uint8_t lastConnection;
//char *readChar;
char readStringa[] = "1";

char* decimalToArrayChar(int value){
  //The Maximum Size is 4 Byte
  //The Websocket can't get chars that there index is greater then 127
  //When I have a character that the index is equle to 0 the string will be cut
  //To fix that i am using the number 1 to 127 and not 0 to 127
  //then the maximum size of the data can be lower or equle to 5

  // Example, The number 30287 will be send by 3 chars:
  //    array[0] = '>', array[1] = 'p' and array[2] = '\x02' 
  static char array[5] = {0,0,0,0,0} ;
  for(int i = 0 ;  value != 0 ; i++){
    array[i] = (value % 127)+1;
    //Serial.print(array[i]);
    value = (value / 127);
  }
  return array;
}


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

        String textToSend = String(decimalToArrayChar(30287));
        Serial.println(textToSend);
        webSocket.sendTXT(lastConnection, textToSend);
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
  Serial.println(ver);
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
  //char* arr = decimalToArrayChar(10,4);
  //  char A = 243;
  //Serial.println(A);
  //  String textToSend = String(A);
  // webSocket.sendTXT(lastConnection, textToSend);
}
