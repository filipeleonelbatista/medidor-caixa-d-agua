#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Ultrasonic.h>

#define pino_trigger D1
#define pino_echo D2


#define altura_total_cm 160

#include "FS.h"

#define WLAN_SSID "LDSAccess"
#define WLAN_PASS "Pioneer47"

ESP8266WebServer server(80);

Ultrasonic ultrasonic(pino_trigger, pino_echo);

void handleRoot(){
  digitalWrite(2, HIGH);
  
  File file = SPIFFS.open("/index.html", "r");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }

  String htmlCode = file.readString();
  
  file.close();

  float cmMsec;
  long microsec = ultrasonic.timing();
  cmMsec = ((altura_total_cm - ultrasonic.convert(microsec, Ultrasonic::CM)) / altura_total_cm) * 100;
  
  String cmMsecString = String(cmMsec, 0); 
  
  htmlCode.replace("#TEMP#", cmMsecString);
  server.send(200, "text/html", htmlCode);
  
  digitalWrite(2, LOW);
}

void setup()
{  
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  Serial.begin(9600);
  
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_SSID, WLAN_PASS); 
  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }  
  Serial.print("\nConectado | Endereço IP: ");
  Serial.println(WiFi.localIP()); 

  if(MDNS.begin("esp8266")){
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.begin();
  Serial.print("\nHTTP Seerver Started");
}

void loop()
{
  server.handleClient();    
}
