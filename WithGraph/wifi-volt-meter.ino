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
char splitSign = 127;       // Split between samples

char* decimalToArrayChar(int value){
  //The Maximum Size is 4 Byte
  //The Websocket can't get chars that there index is greater then 127
  //When I have a character that the index is equle to 0 the string will be cut
  //To fix that i am using the number 1 to 127 and not 0 to 127
  //The charater 127 will sign new sample

  //then the maximum size of the data can be lower or equle to 5

  // Example, The number 30287 will be send by 3 chars:
  //    array[0] = '>', array[1] = 'p' and array[2] = '\x02' 
  static char array[5] = {0,0,0,0,0} ;
  for(int i = 0 ;  value != 0 ; i++){
    array[i] = (value % 126)+1;
    //Serial.print(array[i]);
    value = (value / 126);
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

/*        for(int j = 0 ; j < 10 ; j++){
          String textToSend = "";
          for(int i = 0 ; i < 50 ; i++){
            //textToSend += String(decimalToArrayChar(30000+i));
            //textToSend += splitSign;
            textToSend += String(i);
            textToSend += ",";
          }
          webSocket.sendTXT(lastConnection, textToSend);
          Serial.println(textToSend);
        }*/
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
  String textToSend = "";
  for(int i = 0 ; i < 50 ; i++){
    textToSend += String(decimalToArrayChar(millis()));
    textToSend += splitSign;
  }
  webSocket.sendTXT(lastConnection, textToSend);
}
