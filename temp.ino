#include <WiFi.h>    // Enables the ESP8266 to connect to the local network (via WiFi)
#include <PubSubClient.h>  // Allows us to connect to, and publish to the MQTT broker
#include <DHTesp.h>

//WiFi variables
const char* ssid = "SmartHuawei";
const char* password =  "smartstudent";
//MQTT variables
const char* mqttServer = "192.168.43.181";
const int mqttPort = 1883;

//Sending control variables
int raspReady = 1;
int switcher = 5;
 
WiFiClient espClient;             //Initialise the WiFi and MQTT Client objects
PubSubClient client(espClient);
DHTesp dht;

char temperatureChar[10];         

void callback(char* topic, byte* payload, unsigned int length) {
  if(strcmp(topic,"rasp/feedback")==0){           //confirmation that next data can be sent
    payload[length]= '\0';
    raspReady = atoi((char *)payload);
  }

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}
 
void setup() {
  Serial.begin(9600);
 
  WiFi.begin(ssid, password);                      //Connecting to WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");      //Connecting to MQTT
 
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(1000);
    }   
  }
  
  dht.setup(26, DHTesp::DHT22);               //creatig an object for temperature and humidity sensor

  client.subscribe("home/room1/temp");
  client.subscribe("home/room1/hum");
  client.subscribe("rasp/feedback");

}
 
void loop() {

  while (WiFi.status() != WL_CONNECTED) {       //WiFi connection interrupted
    Serial.println("Disconnected from WiFi!");
    WiFi.begin(ssid, password);                 //trying to reconnect to WiFi
    Serial.println("Reconnecting to WiFi..");
  }

  while (!client.connected()) {                 //MQTT connection interrupted
    Serial.println("Reconnecting to MQTT...");  //trying to reconnect to MQTT
 
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");  
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(1000);
    }
  }
  client.loop();

 if(raspReady==1){
  
   switch(switcher){
     case 5:{
          TempAndHumidity measurement = dht.getTempAndHumidity();
          dtostrf(measurement.temperature,5,2,temperatureChar);
          dtostrf(measurement.humidity,5,2,humidityChar);
          switcher++;  
     }
     
     case 6: {
             char charBuffTemp[50];
             dtostrf(randBroj,5,2,randChar);
             randBroj--;
             
             //String message1 =  String("{temp: ") + temperatureCharDS + String(", topic: home/room1/temp}"); 
             String message1 =  String("{temp: ") + randChar + String(", topic: home/room1/temp}"); 
             message1.toCharArray(charBuffTemp, 50);
             client.publish("home/room1/temp", charBuffTemp);
             raspReady=0;
             switcher++;
             break;
     }
     
   }


 }

 
}
