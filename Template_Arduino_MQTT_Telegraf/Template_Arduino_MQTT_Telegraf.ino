#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>

// MQTT details
const char* broker = "electsut.ddns.net";
const char* topicOut = "b6106164/sensor";
const char* topicIn = "b6106164/in";

//Define Wifi data for connect
#define WIFI_STA_NAME "MSI GL63-ANR"
#define WIFI_STA_PASS "anrpnd310363"

StaticJsonDocument<200> doc;
//String message="";
char message[256];

WiFiClient client;
PubSubClient mqtt(client);

float temp = 25;
float humid = 75;

void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(WIFI_STA_NAME);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_STA_NAME, WIFI_STA_PASS);
   
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  mqtt.setServer(broker, 8883);
  mqtt.setCallback(mqttCallback);
  mqtt.setKeepAlive(1500);
  mqtt.setSocketTimeout (1500);
  Serial.println("Connecting to MQTT Broker: " + String(broker));
  while(mqttConnect()==false) continue;
  Serial.println();

}

void loop() {
   if(mqtt.connected())
  {
    mqtt.loop();
  }
  if(!mqtt.connected())
  {    
    Serial.println("Retry");
    while(mqttConnect()==false) continue;
  }
  doc["temp"] = random(3000, 4000) / 100.0;
  doc["humid"] = random(6000, 9000) / 100.0;
  doc["ADC"] = random(0, 4095);
  doc["lat"] = 14.880135;
  doc["long"] = 102.016438;
  
  serializeJson(doc, message);
  delay(5000);
  Serial.print("MQTT pub = ");
  Serial.println(mqtt.publish(topicOut, message));

}

boolean mqttConnect()
{
  if(!mqtt.connect("device01"))
  {
    Serial.print(".");
    return false;
  }
  Serial.println("Connected to broker.");
  mqtt.subscribe(topicIn);
  return mqtt.connected();
}

void mqttCallback(char* topic, byte* payload, unsigned int len)
{
  Serial.print("Message receive: ");
  Serial.write(payload, len);
  Serial.println();
}
