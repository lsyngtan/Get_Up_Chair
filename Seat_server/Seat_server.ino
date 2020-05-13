#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>

int isObstaclePin = 15;  // This is our input pin
int isObstacle = HIGH;  // HIGH MEANS NO OBSTACLE

const char* ssid     = "Pokemon";              //Main Router      
const char* password = "1945000000";            //Main Router Password
const char* ESP_ssid = "ESP32-Access-Point";
const char* ESP_password = "123456789";
const char* url = "http://myworkspace.hub.ubeac.io/myPC";
String payload_pattern = "[{\"id\": \"MyESP\", \"sensors\": [{\"id\": \"Time Near Sensor\", \"value\": $timer$}, {\"id\": \"Close\", \"value\": $switcher$}]}]";
int dividen = 3600;
int counter = 0;
int switcher = 0;

AsyncWebServer server(80);

void setup() {
  
  //Setup IR Obstacle Sensor
  pinMode(isObstaclePin, INPUT);  
  Serial.begin(115200); // Starts the serial communication

  //Connect to Local WiFi
  delay(4000);   //Delay needed before calling the WiFi.begin
 
  WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");

  //Setup ESP32 WiFi
  WiFi.softAP(ESP_ssid, ESP_password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.on("/On_Off", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(switcher).c_str());
  });

  bool status;

  server.begin();
}

void loop() {
  counter = 0;
  switcher = 0;
  for (int i = 0; i < 86400; i++){
    isObstacle = digitalRead(isObstaclePin);
    if (isObstacle == LOW)
    {
      Serial.println("OBSTACLE!!, OBSTACLE!!");
      counter = counter + 1;
      switcher = 1;
    }
    else
    {
      Serial.println("clear");
      switcher = 0;
    }
    
    float timer;
    timer = (float)counter / (float)dividen;
    String payload = payload_pattern;
    payload.replace("$timer$",String(timer));
    payload.replace("$switcher$",String(switcher));

    if(WiFi.status()== WL_CONNECTED){ 
 
      HTTPClient http;   
  
      http.begin(url);  
      int httpResponseCode = http.POST(payload); 
 
      if(httpResponseCode>0){
        String response = http.getString(); 
        Serial.println(httpResponseCode);
      }
      http.end();
 
    }else{
      Serial.println("Error in WiFi connection");    
    }
    delay(1000);
  }
}
