 // NodeMCUv3-ESP8266 Async WebServer (c) by BlackLeakz
// Author: BlackLeakz
// Name: NodeMCUv3-ESP8266 Async WebServer Backup Snippets
// Version: 0.0.0.1a.2
// Date/Time: 25.07.2023 17:09Uhr (Europe/Berlin) //update 19:42

#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <LittleFS.h>
 #include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>


int networks[];
int n;




 void website() {

 server.on("/requ", HTTP_GET, [](AsyncWebServerRequest *request)
 {
  int n = WiFi.scanNetworks();
    
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print("<!DOCTYPE html><html lang='en'><meta name='viewport' content='width=device-width, initial-scale=1.0'>");
    response->print("<head><title>ESP8266 Configuration Page</title></head>");
    response->print("<body><ul><li><a href='/'>Home</a></li><li><a class='active' href='/repeater'>Repeater</a></li><li style='float:right'><a href='/reboot'>Reboot</a></li><li><a href='/wifiscan'>WiFi-Scan</a></li></ul>");
    if (WiFi.status() != WL_CONNECTED) {
      response->print("<center><div>currently not connected</div></center>");
      }
    else {
      response->print("<center><div>connected to: ");
      response->print(WiFi.SSID());
      response->print(" IP: ");
      response->print(WiFi.localIP().toString());
      response->print("</div></center>");

    }
    response->print("<center><div>");
    if (n == 0) {
      response->print("<center><h1>No wireless networks found</h1></center>");
  }
    else {
      response->print("<center><h1>Wireless Station Settings</h1></center>");
      response->print("<form method='post'>");
      int params = request->params();
      for (int i = 0; i < params; ++i) {
        AsyncWebParameter* p = request->getParam(i);
        networks[i] = WiFi.SSID(i);
        response->print("<div>");
        response->print("<center><input type=\"radio\" id=\"");
        response->print(String(i).c_str());
        response->print("\" ");
        response->print("name=\"SSIDs\" value=\"");
        response->print(String(i).c_str());
        response->print("\"></center>");
        response->print("<center><label for=\"");
        response->print(String(i).c_str());
        response->print("\"");
        response->print(">");
        response->print(String(i + 1).c_str());
        response->print(": ");
        response->print(WiFi.SSID(i).c_str());
        response->print(" (");
        response->print(String(WiFi.RSSI(i)).c_str());
        response->print(")");
        response->print(WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
        response->print("</label></center>");
        response->print("</div>");
        delay(10);
    }
      response->print("<center><label>Password:</label></center><br><input type='password' placeholder='********' name='stapsk' minlength=8 maxlength=63><br><center><small>Must be at least 8 characters or blank!</small></center><br><br>");
      response->print("<center><button type='submit' formaction='stasettings'>Save Permanently</button></center><center><button type='submit' formaction='tempstasettings'>Save Temporarily (Until Reboot)</button></center>");
      response->print("</form>");  
      response->print("<center><h1>Wireless Access Point Settings</h1></center>");
      response->print("<form method='post'>");
      response->print("<center><label>SSID:</label></center><br><input name='apssid' placeholder='");
      response->print(WiFi.softAPSSID().c_str());
      response->print("' length=32><br>");
      response->print("<center><label>Password:</label></center><br><input type='password' placeholder='");
      response->print(WiFi.softAPPSK().c_str());
      response->print("' name='appsk' minlength=8 maxlength=63><br><center><small>Must be at least 8 characters or blank!</small></center><br><br>");
      response->print("<center><button type='submit' formaction='apsettings'>Save Permanently</button></center><center><button type='submit' formaction='tempapsettings'>Save Temporarily (Until Reboot)</button></center>");
      response->print("</form>");
  }
    response->print("<center><h1>Miscellaneous</h1></center>");
    response->print("<form method='get' action='reboot'><input type='submit' value='Reboot'></form>");
    response->print("<div>");    

    request->send(response); });
 }





 void setup() {
    website();

 }



 void loop() {

 }