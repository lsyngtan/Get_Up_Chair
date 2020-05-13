#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include <EasyBuzzer.h>

const char* ESP_ssid = "ESP32-Access-Point";
const char* ESP_password = "123456789";

const char* url_switch = "http://192.168.4.1/On_Off";
int buzzer = 15;

String switcher;

int interval = 3600;
int period_rest = 1800;

void setup(){
  Serial.begin(115200);
  WiFi.begin(ESP_ssid, ESP_password);
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());

  EasyBuzzer.setPin(buzzer);
}

void loop(){
  int temp_rest = 0;
  int counter = 0;
  for(int i = 0; i < 86400; i++){    
    EasyBuzzer.update();
    if(WiFi.status()== WL_CONNECTED){ 
      switcher = httpGETRequest(url_switch);
    }else{
      Serial.println("Error in WiFi connection");    
    }
    Serial.println(counter);
    if((counter % interval) > period_rest){
      if(switcher == "1"){
        Serial.println("NOT RESTING");
        EasyBuzzer.singleBeep(2000, 1000);
      }else{
        Serial.println("RESTING");
        EasyBuzzer.stopBeep();
        counter = counter + 1;
      }
    }else if(switcher == "1"){
      Serial.println("SITTING");
      counter = counter + 1;
      temp_rest = 0;
    }else{
      Serial.println("NOT SITTING");
      temp_rest = temp_rest + 1;
      if(temp_rest > period_rest){
        counter = 0;
      }
    }
    delay(1000);
  }
}

String httpGETRequest(const char* serverName) {
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "--"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
