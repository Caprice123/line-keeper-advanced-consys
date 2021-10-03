
/*
 Rui Santos
 Complete project details at https: RandomNerdTutorials.com/esp32-cam-post-image-photo-server/
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files.
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
*/

#include <Arduino.h>
#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"

const char* ssid = "lantai 3";
const char* password = "sisca123";

String serverName = "192.168.100.108";   
//String serverName = "example.com";   


const char roomNumber = '1';
String serverPath = "/upload";    // Flask upload route
//String serverPath = "/upload";

const int serverPort = 80;

const int fps = 10;
const int timerInterval = 1000 / fps; //     time (milliseconds) between each HTTP POST image
unsigned long previousMillis = 0;    // last time image was sent


WiFiClient client;

// CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void connecting_wifi(){
 WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 

 WiFi.mode(WIFI_STA);
 Serial.println();
 Serial.print("Connecting to ");
 Serial.println(ssid);
 WiFi.begin(ssid, password);  
 while (WiFi.status() != WL_CONNECTED) {
   Serial.print(".");
   delay(500);
 }
 Serial.println();
 Serial.print("ESP32-CAM IP Address: ");
 Serial.println(WiFi.localIP());
}

void setting_camera_module(){
 camera_config_t config;
 config.ledc_channel = LEDC_CHANNEL_0;
 config.ledc_timer = LEDC_TIMER_0;
 config.pin_d0 = Y2_GPIO_NUM;
 config.pin_d1 = Y3_GPIO_NUM;
 config.pin_d2 = Y4_GPIO_NUM;
 config.pin_d3 = Y5_GPIO_NUM;
 config.pin_d4 = Y6_GPIO_NUM;
 config.pin_d5 = Y7_GPIO_NUM;
 config.pin_d6 = Y8_GPIO_NUM;
 config.pin_d7 = Y9_GPIO_NUM;
 config.pin_xclk = XCLK_GPIO_NUM;
 config.pin_pclk = PCLK_GPIO_NUM;
 config.pin_vsync = VSYNC_GPIO_NUM;
 config.pin_href = HREF_GPIO_NUM;
 config.pin_sscb_sda = SIOD_GPIO_NUM;
 config.pin_sscb_scl = SIOC_GPIO_NUM;
 config.pin_pwdn = PWDN_GPIO_NUM;
 config.pin_reset = RESET_GPIO_NUM;
 config.xclk_freq_hz = 20000000;
 config.pixel_format = PIXFORMAT_JPEG;

   //init with high specs to pre-allocate larger buffers
 if(psramFound()){
   config.frame_size = FRAMESIZE_QVGA;
   config.jpeg_quality = 10;   //0-63 lower number means higher quality
   config.fb_count = 2;
 } else {
   config.frame_size = FRAMESIZE_QVGA;
   config.jpeg_quality = 12;   //0-63 lower number means higher quality
   config.fb_count = 1;
 }

   //camera init
 esp_err_t err = esp_camera_init(&config);
 if (err != ESP_OK) {
   Serial.printf("Camera init failed with error 0x%x", err);
   delay(1000);
   ESP.restart();
 }
 sensor_t * s = esp_camera_sensor_get();
            s->set_brightness(s, 2);     // -2 to 2
            s->set_contrast(s, 0);       // -2 to 2
            s->set_saturation(s, 0);     // -2 to 2
            s->set_special_effect(s, 0); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
            s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
            s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
            s->set_wb_mode(s, 0);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
            s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
            s->set_aec2(s, 0);           // 0 = disable , 1 = enable
            s->set_ae_level(s, 0);       // -2 to 2
            s->set_aec_value(s, 300);    // 0 to 1200
            s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
            s->set_agc_gain(s, 0);       // 0 to 30
            s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
            s->set_bpc(s, 0);            // 0 = disable , 1 = enable
            s->set_wpc(s, 1);            // 0 = disable , 1 = enable
            s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
            s->set_lenc(s, 1);           // 0 = disable , 1 = enable
            s->set_hmirror(s, 0);        // 0 = disable , 1 = enable
            s->set_vflip(s, 0);          // 0 = disable , 1 = enable
            s->set_dcw(s, 1);            // 0 = disable , 1 = enable
            s->set_colorbar(s, 0);       // 0 = disable , 1 = enable
}

void setup() {
 
 Serial.begin(115200);
 pinMode(2, OUTPUT);
 connecting_wifi();
 setting_camera_module();
 sendPhoto(); 
 digitalWrite(2, HIGH);
}

void loop() {
 unsigned long currentMillis = millis();
 if (currentMillis - previousMillis >= timerInterval) {
     Serial.println("Sending Photo");
     sendPhoto();
     previousMillis = currentMillis;
   
   
 }
}

void making_head_http(uint16_t length, String head){
   
   client.println("POST " + serverPath + " HTTP/1.1");
   client.println("Host: " + serverName);
   client.println("Content-Length: " + String(length));
   client.println("Content-Type: multipart/form-data; boundary=ESP32");
   client.println();
   client.print(head);
}

void making_body_http(camera_fb_t * fb){
   uint8_t *fbBuf = fb->buf;
   size_t fbLen = fb->len;
   for (size_t n=0; n<fbLen; n=n+1024) {
     if (n+1024 < fbLen) {
       client.write(fbBuf, 1024);
       fbBuf += 1024;
     }
     else if (fbLen%1024>0) {
       size_t remainder = fbLen%1024;
       client.write(fbBuf, remainder);
     }
   }
}

void making_tail_http(String tail){
   client.print(tail);
}

void posting_image(camera_fb_t * fb){
   String head = "--ESP32\r\nContent-Disposition: form-data; name=\"file\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
   String tail = "\r\n--ESP32--\r\n";
   
   uint16_t imageLen = fb->len;
   uint16_t extraLen = head.length() + tail.length();
   uint16_t totalLen = imageLen + extraLen;

   making_head_http(totalLen, head);
   making_body_http(fb);
   making_tail_http(tail);
   Serial.print(totalLen);
}

String get_response_server(){
   String getAll;
   String getBody;

   int timoutTimer = timerInterval;
   long startTimer = millis();
   boolean state = false;

   
   while ((startTimer + timoutTimer) > 0) {
     Serial.print(".");
     delay(100);      
     while (client.available()) {
       char c = client.read();
       if (c == '\n') {
         if (getAll.length()==0) { state=true; }
         getAll = "";
       }
       else if (c != '\r') { getAll += String(c); }
       if (state==true) { getBody += String(c); }
       startTimer = millis();
     }
     if (getBody.length()>0) { break; }
   }
   
   return getBody;
}

String sendPhoto() {
 String response;

 camera_fb_t * fb = NULL;
 fb = esp_camera_fb_get();
 if(!fb) {
   Serial.println("Camera capture failed");
   delay(1000);
   ESP.restart();
 }
 
 Serial.println("Connecting to server: " + serverName);

 if (client.connect(serverName.c_str(), serverPort)) {
   
   Serial.println("Connection successful!");    
   posting_image(fb);
   esp_camera_fb_return(fb);
   
   response = get_response_server();

   Serial.println();
   client.stop();
   Serial.print(response);
   
   
 }
 else {
   response = "Connection to " + serverName +  " failed.";
   Serial.println(response);
 }
 return response;
}