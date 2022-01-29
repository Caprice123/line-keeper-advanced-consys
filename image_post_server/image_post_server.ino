
#include <esp32cam.h>
#include <WebServer.h>
#include <WiFi.h>

#include <ArduinoJson.h>
#include "soc/soc.h" 
#include "soc/rtc_cntl_reg.h"  

#include <WirePacker.h>
#include <Wire.h>
#define red 12
#define green 13
#define blue 2
#define SDA 14
#define SCL 15
#define I2C_SLAVE_ADDR 0x04

const char* WIFI_SSID = "PELANGI";
const char* WIFI_PASS = "kelvin123";

//const char* WIFI_SSID = "lantai 3";
//const char* WIFI_PASS = "sisca123";

WebServer server(80);

static auto loRes = esp32cam::Resolution::find(320, 240);
// static auto hiRes = esp32cam::Resolution::find(800, 600);

void serveJpg()
{
  auto frame = esp32cam::capture();
  if (frame == nullptr) {
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");
    ESP.restart();
    return;
  }
  Serial.printf("CAPTURE OK %dx%d %db\n", frame->getWidth(), frame->getHeight(),
                static_cast<int>(frame->size()));

  server.setContentLength(frame->size());
  server.send(200, "image/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);
}

void handleJpgLo()
{
  unsigned long x = millis();
//  if (!esp32cam::Camera.changeResolution(loRes)) {
//    Serial.println("SET-LO-RES FAIL");
//  }
  serveJpg();
  Serial.print("Time spent: ");
  Serial.println(millis() - x);
}

// void handleJpgHi()
// {
//   if (!esp32cam::Camera.changeResolution(hiRes)) {
//     Serial.println("SET-HI-RES FAIL");
//   }
//   serveJpg();
// }

// void handleJpg()
// {
//   server.sendHeader("Location", "/cam-hi.jpg");
//   server.send(302, "", "");
// }

void transmitData(int angle){
      WirePacker packer;

      packer.write((byte) angle);
      packer.end();
      
      Wire.beginTransmission(I2C_SLAVE_ADDR);
      while (packer.available()){
          Wire.write(packer.read());
      }
      Wire.endTransmission();
      Serial.println("Transmission done");          
      Serial.println();
}

void handlePost()
{
  digitalWrite(blue, HIGH);
  String postBody = server.arg("plain");
  

  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, postBody);
  if (error){
      Serial.print("Error parsing JSON ");
      Serial.println(error.c_str());

      String msg = error.c_str();
      server.send(400, "text/html", "Error in parsing json body! <br>" + msg);
  }
  else{
      JsonObject postObj = doc.as<JsonObject>();
      
      
      if (server.method() == HTTP_POST){
          int angle = postObj["angle"];
          Serial.println(angle);
          transmitData(angle);
          
          digitalWrite(blue, LOW);
          DynamicJsonDocument doc(512);
          doc["status"] = "OK";

          String buf;
          serializeJson(doc, buf);
          
          server.send(201, "application/json", buf);
      }
  }
}


void setup()
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);

  digitalWrite(red, HIGH);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);
  Serial.begin(115200);
  Serial.println();

  {
    using namespace esp32cam;
    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(loRes);
    cfg.setBufferCount(2);
    cfg.setJpeg(80);

    bool ok = Camera.begin(cfg);
    Serial.println(ok ? "CAMERA OK" : "CAMERA FAIL");
  }

  WiFi.persistent(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    count += 1;
    if (count == 10){
      ESP.restart();
    }
  }
  Serial.println();
  Serial.print("http://");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.RSSI());
  Wire.begin(SDA, SCL);

  auto frame = esp32cam::capture();
  if (frame == nullptr) {
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");
    ESP.restart();
    return;
  }
  digitalWrite(green, HIGH);
  digitalWrite(red, LOW);
  
  server.on("/cam-lo.jpg", handleJpgLo);
  // server.on("/cam-hi.jpg", handleJpgHi);
  // server.on("/cam.jpg", handleJpg);

  server.on("/upload", HTTP_POST, handlePost);
  server.begin();
}

void loop()
{
  server.handleClient();
}
