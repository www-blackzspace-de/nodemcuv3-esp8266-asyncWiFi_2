// NodeMCUv3-ESP8266 Async WebServer (c) by BlackLeakz
// Author: BlackLeakz
// Name: NodeMCUv3-ESP8266 Async WebServer
// Version: 0.0.0.1a.2
// Date/Time: 25.07.2023 17:09Uhr (Europe/Berlin) //update 19:42

#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <LittleFS.h>

#include <ESP8266mDNS.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LiquidCrystal_I2C.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include <AsyncElegantOTA.h>

// LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address, if it's not working try 0x27.
LiquidCrystal_I2C lcd(0x26, 16, 2);
LiquidCrystal_I2C lcd2(0x27, 16, 2);

// Adafruit OLED SSD1306 Parameters (WIDTH, HEIGHT)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// AP Data
const char *ssid = "BlackLeakz";
const char *password = "123456789";

// mDNS Name
const char *dns_name = "blackleakz";

const char *http_username = "admin";
const char *http_password = "admin";

// WiFi Scan Function
int numberOfNetworks;
unsigned long lastAusgabe;
const unsigned long intervall = 5000;

String msg;
String networks[40];

AsyncWebServer server(80);

// blackzspace.de BITMAP_LOGO
#define LOGO_HEIGHT 32
#define LOGO_WIDTH 32
const unsigned char NaN[] PROGMEM = {
    // 'favicon-32x32, 32x32px
    0xc0, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x30, 0x00, 0x0f, 0xf8,
    0x30, 0x00, 0x1f, 0xfc, 0x30, 0x00, 0x38, 0x08, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00,
    0x37, 0xf0, 0x30, 0x00, 0x3e, 0x78, 0x38, 0x00, 0x38, 0x1c, 0x1e, 0x00, 0x30, 0x1c, 0x0f, 0xc0,
    0x30, 0x0c, 0x03, 0xf0, 0x30, 0x0c, 0x00, 0xf8, 0x30, 0x0e, 0x00, 0x3c, 0x30, 0x0e, 0x00, 0x1c,
    0x30, 0x0c, 0x00, 0x0c, 0x30, 0x0c, 0x00, 0x0c, 0x30, 0x1c, 0x00, 0x1c, 0x38, 0x38, 0x70, 0x38,
    0x3f, 0xf0, 0x7f, 0xf0, 0x37, 0xe0, 0x1f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x03};

///// START  Adafruit DISPLAY DRAWER SECTIONS   ///// START  Adafruit DISPLAY DRAWER SECTIONS  ///// START  Adafruit DISPLAY DRAWER SECTIONS


void testscrolltext(void)
{
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.display(); // Show initial text
  delay(100);
  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}


void testdrawbitmap(void)
{
  display.clearDisplay();

  display.drawBitmap(
      (display.width() - LOGO_WIDTH) / 2,
      (display.height() - LOGO_HEIGHT) / 2,
      NaN, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(4500);
}
///// END  Adafruit DISPLAY DRAWER SECTIONS   ///// END  Adafruit DISPLAY DRAWER SECTIONS  ///// END  Adafruit DISPLAY DRAWER SECTIONS

// WiFi-Scan |-> Prints RESULT TO OLED-DISPLAY!!
void wifi_oledscan()
{
  if (numberOfNetworks <= 0)
  {
    numberOfNetworks = WiFi.scanNetworks();
    Serial.print(numberOfNetworks);
    Serial.println(F(" gefundene Netzwerke"));
  }
  else if (millis() - lastAusgabe > intervall)
  {
    numberOfNetworks--;
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Network-name: ");
    display.setCursor(0, 10);
    display.print(WiFi.SSID(numberOfNetworks));
    display.setCursor(0, 20);
    display.print("Signal strength: ");
    display.setCursor(0, 30);
    display.print(WiFi.RSSI(numberOfNetworks));
    display.display();
    Serial.print("Network name: ");
    Serial.println(WiFi.SSID(numberOfNetworks));
    Serial.print("Signal strength: ");
    Serial.println(WiFi.RSSI(numberOfNetworks));
    Serial.println("-----------------------");
    lastAusgabe = millis();
  }
}


////  START  WEBSERVER FUNCTIONS  ////   START  WEBSERVER FUNCTIONS
void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

////   END  WEBSERVER FUNCTIONS   ////   END  WEBSERVER FUNCTIONS     ////

// WebServers Website URL-Handler
void website()
{

  // Route to Styles.css, stoed in LittleFS! -

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/style/style.css", "text/css"); });

  server.on("/wifi.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/styles/wifi.css", "text/css"); });




  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", "text/html"); });




  // WebPage Router to Sites - 

  //    3xx  /scan path's, 1st = Func with PROGMEM, 2nd Func with Route to LittleFS! 3rd = Added Route to /scan, Using ArduinoJson and a simple request-send method to print page.
  //   Only the 3rd Method is currently working with a wifi-scan result. Requesting the path/Route wont be show anything if only 1x Time Requested!! Please refresh the tab, to the exact smae route, to get a JSON-String with the WiFi-Scan Results!!!
  // NOTE: Im currently learning by doing how a AsyncWebServer acts, and what NAT Routing is about. So beware of shit-code! This isnt a release-file.

  // server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request)
  //           { request->send_P(200, "text/html", wifiscan); });

  // server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request)
  //           { request->send(LittleFS, "/scan.html", "text/html"); });

  server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String json = "[";
    int n = WiFi.scanComplete();
    if(n == -2){
      WiFi.scanNetworks(true);
    } else if(n){
      for (int i = 0; i < n; ++i){
        if(i) json += ",";
        json += "{";
        json += "\"rssi\":"+String(WiFi.RSSI(i));
        json += ",\"ssid\":\""+WiFi.SSID(i);
        json += ",\"bssid\":\""+WiFi.BSSIDstr(i);
        json += ",\"channel\":"+String(WiFi.channel(i));
        json += ",\"secure\":"+String(WiFi.encryptionType(i));
        json += ",\"hidden\":"+String(WiFi.isHidden(i)?"true":"false");
        json += "}";
      }
      WiFi.scanDelete();
      if(WiFi.scanComplete() == -2){
        WiFi.scanNetworks(true);
      }
    }
    json += "]";
    request->send(200, "text/json", json);
    json = String();
    wifi_oledscan(); });



  server.on("/wifimanager", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/wifimanager.html", "text/html"); });

  server.onNotFound(notFound);

  // Start OTA Update Server
  AsyncElegantOTA.begin(&server);

  // Start server
  server.begin();
}

// Setup Function!
void setup()
{
  // Starts Serial-communication !
  Serial.begin(115200);
  Serial.println("Console > Serial communication started!.");

  // LED-Blinking to sign starting device!
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  delay(300);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);

  // Setup I2C Displays OLED_SSD1306 & LiquidCrystal
  // setupDisplays();
  // Initializes oledssd1306 display, if error, serial.com sents msg
  Serial.println("Console > Initialize displays.\n");
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("Console > SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.display();
  display.clearDisplay();
  testdrawbitmap();
  delay(5000);
  display.setTextColor(WHITE); // set Text-color
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Console >");
  display.display();
  display.setCursor(0, 10);
  display.println("  Started.");
  display.display();
  if (!display2.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  { // SCREEN_ADDRESS maybe needs to set to 0x3D
    Serial.println(F("Console > SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display2.display();
  display2.clearDisplay();
  display2.setTextColor(WHITE); // set Text-color
  display2.setTextSize(1);
  display2.setCursor(0, 0);
  display2.println("Console >");
  display2.display();
  display2.setCursor(0, 10);
  display2.println("  Started.");
  display2.display();
  Serial.println("Console > OLED SSD1306 display started!.\n");
  // Initializes LiquidCrystal Display 1
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Console > ");
  lcd.setCursor(2, 1);
  lcd.print("Started.");
  // Initializes LiquidCrystal Display 2
  lcd2.init();
  lcd2.backlight();
  lcd2.setCursor(0, 0);
  lcd2.print("Console > ");
  lcd2.setCursor(2, 1);
  lcd2.print("Started.");
  Serial.println("Console > I2C LiquidCrystal display started!.\n");


  // Setup WiFi
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Console > AP IP address: ");
  Serial.println(IP);
  Serial.println(WiFi.localIP());
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Console > ");
  display.display();
  display.setCursor(0, 10);
  display.println("AP IP ::: ");
  display.display();
  display.setCursor(0, 20);
  display.println(IP);
  display.display();

  // Setup FileSystem
  // setupFS();
  // Initialize LittleFS (FileSystem) and load file: "text.txt"
  if (!LittleFS.begin())
  {
    Serial.println("Console > An Error has occurred while mounting LittleFS... .. .");
    digitalWrite(LED_BUILTIN, LOW);
    delay(300);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(300);
    digitalWrite(LED_BUILTIN, HIGH);
    return;
  }

  // Start WebServer + Website
  website();

  // // Register the dns_name
  //   mDNS();
  if (!MDNS.begin(dns_name))
  { // Start the mDNS responder for esp8266.local
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);

    Serial.println("Console > Error setting up MDNS responder!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Error mDNS !!!");
    display.display();
  }

  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
  delay(300);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(160);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Console > mDNS responder started");
  Serial.println("Console > DNS gestartet, erreichbar unter: ");
  Serial.println("Console > http://" + String(dns_name) + ".local/\n");
}




void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
  delay(300);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(160);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);

  MDNS.update();
}
