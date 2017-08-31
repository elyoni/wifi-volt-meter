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

//char ssid[32];
//char password[32];

const char newDeviceSign = 42 ;

const int maxConnectionTries = 40;
int connectionTries = 0;
ESP8266WebServer webServer(80);

bool isConnected = false;

//User Can change
const int PCF8591 = 0x48; //#TODO will be pull from the eeprom I2C address
const byte numberOfAnalog = 4;  //#TODO Will pull from the EEPROM 
char deviceSign = '1' ;  //#TODO  Change the sign to IP address


class SmartWifi{
  //I am using the EEPROM to save the data off the access point and the password
  static const int SSID_SIZE = 16;
  static const int PASSWORD_SIZE = 20;
  private:
    int _startAddress; // Save the point where to start save the AP and the Pass
    int _eepromSize;
    char _ssid[SSID_SIZE];
    char _password[PASSWORD_SIZE];
    int _maxNumberOfTries;
    bool _useSerial;
    ESP8266WebServer& _webServer;
    void loadCredentiasFromEEPROM(void);
    void saveCredentialsToEEPROM(void);
    void clearCredentiasFromEEPROM(void);
    // Site handler functions 
    void accessPointAndPasswordForm(void);
    void resetESPForm(void);
    void resetESPHandler(void);
  public:
    SmartWifi(int startAddress,int eepromSize,bool useSerial,ESP8266WebServer& webServer);
    SmartWifi(bool useSerial, int eepromSize,ESP8266WebServer& webServer);
    void setMaxNumberOfConnectionTries(int numberOfTries);
    void connectToWifi();
    IPAddress getIPAddress(void);

};
SmartWifi::SmartWifi(bool useSerial, int eepromSize, ESP8266WebServer& webServer):_useSerial(useSerial),_eepromSize(eepromSize),_webServer(webServer){
  _maxNumberOfTries = 20;
  _startAddress = 0;
  loadCredentiasFromEEPROM();
}
SmartWifi::SmartWifi(int startAddress,int eepromSize, bool useSerial, ESP8266WebServer& webServer):_useSerial(useSerial), _startAddress(startAddress), _eepromSize(eepromSize),_webServer(webServer){
  _maxNumberOfTries = 20;
  loadCredentiasFromEEPROM();
}

void SmartWifi::loadCredentiasFromEEPROM(void){
/** Load WLAN credentials from EEPROM */
  char ok[3];
  EEPROM.begin(_eepromSize);
  EEPROM.get(_startAddress, _ssid);
  EEPROM.get(_startAddress+sizeof(_ssid), _password);
  EEPROM.get(_startAddress+sizeof(_ssid)+sizeof(_password), ok);
  EEPROM.end();
  if (String(ok) != String("OK")) {
    _ssid[0] = 0;
    _password[0] = 0;
  }
  if (_useSerial){
    Serial.println("Recovered credentials:");
    Serial.println(_ssid);
    Serial.println(strlen(_password)>0?"********":"<no password>");
  }
}

void SmartWifi::saveCredentialsToEEPROM(void){
  char ok[3] = "OK";
  EEPROM.begin(_eepromSize);
  EEPROM.put(_startAddress, _ssid);
  EEPROM.put(_startAddress+sizeof(_ssid), _password);
  EEPROM.put(_startAddress+sizeof(_ssid)+sizeof(_password), ok);
  EEPROM.commit(); //After using put we need to use commit
  EEPROM.end();
}

void SmartWifi::clearCredentiasFromEEPROM(void){
  String empty = "0000"; //Enter junk data
  empty.toCharArray(_ssid,SSID_SIZE);
  empty.toCharArray(_password,PASSWORD_SIZE);

  char no[2+1] = "NO";
  EEPROM.begin(_eepromSize);
  EEPROM.put(_startAddress, _ssid);
  EEPROM.put(_startAddress+sizeof(_ssid), _password);
  EEPROM.put(_startAddress+sizeof(_ssid)+sizeof(_password), no);
  EEPROM.commit();
  EEPROM.end();
}

void SmartWifi::setMaxNumberOfConnectionTries(int maxNumberOfTries){
  _maxNumberOfTries = maxNumberOfTries;
}


void SmartWifi::accessPointAndPasswordForm(void){
  char site[430];
  snprintf(site,430,
      "<html>\
      <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
      <body>\
      <h1>Update Access Point Settings</h1>\
      <form action=\"/resetESPForm\">\
      Access Point:<input type=\"text\" name=\"Access Point\">\
      <br>\
      Password:<input type=\"text\" name=\"Password\">\
      <br>\
      <input type=\"submit\" value=\"Update\">\
      </form>\
      </body>\
      </html>"
      );
  _webServer.send(200,"text/html",site);
}

bool SmartWifi::connectToWifi(void){
  WiFi.mode(WIFI_STA);
  WiFi.begin(_ssid,_password);
  //Tring to connect to the wifi
  int connectionTries = 0;
  while(WiFi.status() != WL_CONNECTED && connectionTries < _maxNumberOfTries){
    delay(500);
    if (_useSerial){
      Serial.print(".");
    }
    connectionTries++;
  }
  if (connectionTries == _maxNumberOfTries){
    // Could not success to connect to the wifi
    // Switching To hotspot
    if (_useSerial){
      Serial.println("");
      Serial.println("Couldn't Connect to the Wifi, changing to Access Point option");
    }
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    WiFi.softAP("esp8266 Access Point"); //For some reasone the password wan't work
    _webServer.on("/",[this](){accessPointAndPasswordForm();});
    _webServer.on("/settingsAPPA",[this](){accessPointAndPasswordForm();});
    _webServer.on("/resetESPForm",[this](){resetESPForm();});
    _webServer.on("/resetESP", [this](){resetESPHandler();});
    _webServer.begin();
    //The user need to reset the ESP to after he update the ssid and the password
    while(true){
      _webServer.handleClient();
    }
  }else{
    // Every things is good, continue with the program
    if (_useSerial){
      Serial.print("Device Ip Address: ");
      Serial.println(WiFi.localIP());
    }
   /*
   // i2c config
    Wire.begin(0,2);
    // webSocket config
    _webSocket.begin();
    _webSocket.onEvent(webSocketEvent);
    */
  }
}


void SmartWifi::resetESPForm(void){
  Serial.println(_webServer.argName(0) + ": " + _webServer.arg(0));
  Serial.println(_webServer.argName(1) + ": " + _webServer.arg(1));

  _webServer.arg(0).toCharArray(_ssid,32);
  _webServer.arg(1).toCharArray(_password,16);
  /*
     --- Here Come the code for saving the data into the EEPROM
   */
  saveCredentialsToEEPROM();
  char site[260];
  snprintf(site,260,
      "<html>\
      <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
      <body>\
      <h1>Reset ESP</h1>\
      <form action=\"/resetESP\">\
      <input type=\"submit\" value=\"RESET\">\
      </form>\
      </body>\
      </html>"
      );
  _webServer.send(200,"text/html",site);
}
void SmartWifi::resetESPHandler(void){
  WiFi.disconnect();
  ESP.restart();//After the first flash this command wan't work
  //Need to reset the esp to make the command work
}

IPAddress SmartWifi::getIPAddress(void){
  return WiFi.localIP();
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


int readI2C(int addressBus,byte controlReg){
  Wire.beginTransmission(addressBus); // wake up PCF8591
  Wire.write(controlReg); // control byte: reads ADC0 then auto-increment
  Wire.endTransmission(); // end tranmission
  Wire.requestFrom(addressBus, 2); // When ask for 1 byte I get delay from the chip
  Wire.read(); // The First and the Secound read are the same, free the from read from the buffer
  return Wire.read();
}

void setup() {
  Serial.begin(115200);
  SmartWifi smartWifi(0, 4096, true, webServer);
  smartWifi.connectToWifi();
  // Every things is good, continue with the program
  // i2c config
  Wire.begin(0,2);
  // webSocket config
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  /*
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
  */
}
