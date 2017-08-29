#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <Hash.h>
#include <stdio.h>
#include <EEPROM.h> // For Saveing the ssid and the password
#include <Wire.h> // For the i2c

WebSocketsServer webSocket = WebSocketsServer(81);

//Don't change the flowing value
//The data will be loaded from the EEPROM
char ssid[32];
char password[32];

const char newDeviceSign = 42 ;

const int maxConnectionTries = 40;
int connectionTries = 0;
ESP8266WebServer server(80);

bool isConnected = false;

//User Can change
const int PCF8591 = 0x48; //I2C address
const byte numberOfAnalog = 4; 
char deviceSign = '1' ;




/** Load WLAN credentials from EEPROM */
void loadCredentials() {
  EEPROM.begin(512);
  EEPROM.get(0, ssid);
  EEPROM.get(0+sizeof(ssid), password);
  char ok[2+1];
  EEPROM.get(0+sizeof(ssid)+sizeof(password), ok);
  EEPROM.end();
  if (String(ok) != String("OK")) {
    ssid[0] = 0;
    password[0] = 0;
  }
  Serial.println("Recovered credentials:");
  Serial.println(ssid);
  Serial.println(strlen(password)>0?"********":"<no password>");
}

/** Store WLAN credentials to EEPROM */
void saveCredentials() {
  EEPROM.begin(512);
  EEPROM.put(0, ssid);
  EEPROM.put(0+sizeof(ssid), password);
  char ok[2+1] = "OK";
  EEPROM.put(0+sizeof(ssid)+sizeof(password), ok);
  EEPROM.commit();
  EEPROM.end();
}

void clearCredentials(){
  String empty = "123123"; //Enter junk data
  empty.toCharArray(ssid,32);
  empty.toCharArray(password,16);
  EEPROM.begin(512);
  EEPROM.put(0, ssid);
  EEPROM.put(0+sizeof(ssid), password);
  char no[2+1] = "OK";
  EEPROM.put(0+sizeof(ssid)+sizeof(password), no);
  EEPROM.commit();
  EEPROM.end();
}

uint8_t lastConnection;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  switch(type) {
    case WStype_DISCONNECTED:
      {
        Serial.printf("[%u] Disconnected!\n", num);
        isConnected = false;
        break;
      }
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        lastConnection = num;
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        //webSocket.sendTXT(num, "Connected To ESP01");
        // On new connection the esp will send his name(one letter) and the number of analog
        Serial.println(String(newDeviceSign) +  String(deviceSign) + String(numberOfAnalog));
        webSocket.sendTXT(num, String(newDeviceSign) +  String(deviceSign) + String(numberOfAnalog));
        isConnected = true;
        break;
      }
    case WStype_TEXT:
      {
        String text = String((char *) &payload[0]);
        Serial.print(text);
        break;
      }  

    case WStype_BIN:    
      {
        hexdump(payload, lenght);
        // echo data back to browser
        webSocket.sendBIN(num, payload, lenght);
        break;
      }
  }
}

void newAccessPoint(){
  char site[300];
  snprintf(site,300,
      "<html>\
      <body>\
      <h1>Update Access Point Settings</h1>\
      <form action=\"/updatedAccessPoint\">\
      Access Point:<input type=\"text\" name=\"Access Point\">\
      <br>\
      Password:<input type=\"text\" name=\"Password\">\
      <br>\
      <input type=\"submit\" value=\"Update\">\
      </form>\
      </body>\
      </html>"
      );
  server.send(200,"text/html",site);
}

void updateAcessPoint(void){
  Serial.println(server.argName(0) + ": " + server.arg(0));
  Serial.println(server.argName(1) + ": " + server.arg(1));

  server.arg(0).toCharArray(ssid,32);
  server.arg(1).toCharArray(password,16);
  /*
     --- Here Come the code for saving the data into the EEPROM
   */
  saveCredentials();
  char site[500];
  snprintf(site,500,
      "<html>\
      <body>\
      <h1>Reset ESP</h1>\
      <form action=\"/resetESP\">\
      <input type=\"submit\" value=\"RESET\">\
      </form>\
      </body>\
      </html>"
      );
  Serial.println("Finish running updateAccessPoint");
  server.send(200,"text/html",site);
}
void resetESP(void){
  WiFi.disconnect();
  ESP.restart();//After the first flash this command wan't work
  //Need to reset the esp to make the command work
}

int readI2C(int addressBus,byte controlReg){
  Wire.beginTransmission(addressBus); // wake up PCF8591
  Wire.write(controlReg); // control byte: reads ADC0 then auto-increment
  Wire.endTransmission(); // end tranmission
  Wire.requestFrom(addressBus, 2); // When ask for 1 byte I get delay from the chip
  Wire.read(); // The First and the Secound read are the same, free the from read from the buffer
  return Wire.read();
}

void setup() {
  //Configure The Serial Settings
  Serial.begin(115200);
  Serial.setTimeout(0.05);
  //clearCredentials();
  //Configure The Wifi Settings
  loadCredentials();
  WiFi.mode(WIFI_STA);
  Serial.println(ssid);
  WiFi.begin(ssid,password);
  //Tring to connect to the wifi
  while(WiFi.status() != WL_CONNECTED && connectionTries < maxConnectionTries){
    delay(500);
    Serial.print(".");
    connectionTries++;
  }
  if (connectionTries == maxConnectionTries){
    // Could not success to connect to the wifi
    // Switching To hotspot
    Serial.println("");
    Serial.println("Couldn't Connect to the Wifi, changing to Access Point option");
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    WiFi.softAP("esp8266 Access Point"); //For some reasone the password wan't work
    server.on("/", newAccessPoint);
    server.on("/updatedAccessPoint", updateAcessPoint);
    server.on("/resetESP", resetESP);
    server.begin();
    while(true){
      //The user need to reset the ESP to after he update the ssid and the password
      server.handleClient();
    }
  }else{
    // Every things is good, continue with the program
    Serial.println(WiFi.localIP());
    // i2c config
    Wire.begin(0,2);
    // webSocket config
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
  }
}

void loop() {
  webSocket.loop();
  //The full message form is (number Of Analog,Analog Value 0,Analog Value 1,...)
  if (isConnected){
    String textToSend = deviceSign + String(numberOfAnalog);
    for (byte i=1 ; i < numberOfAnalog	; i++){
      textToSend += String(readI2C(PCF8591,i));
      textToSend += ",";
    }
    textToSend += String(readI2C(PCF8591,0));
    //Serial.println(textToSend);
    webSocket.sendTXT(lastConnection, textToSend);
    delay(300);
  }
}
