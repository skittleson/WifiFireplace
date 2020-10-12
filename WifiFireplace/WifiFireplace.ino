#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch) 

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h> 

#include <PubSubClient.h>
#include <WS2812FX.h>
#define LED_COUNT 50
#define LED_PIN D5
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
WiFiClient wifiClient;
PubSubClient client(wifiClient);
const char* mqtt_server = "192.168.4.100";

void setup() {
  Serial.begin(115200);  
  setupFirePlace();
  WiFiManager wifiManager;
  //reset saved settings
  //wifiManager.resetSettings();
  
  //set custom ip for portal
  wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
  wifiManager.autoConnect("fireplace");
  randomSeed(micros());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  ws2812fx.service();
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    ws2812fx.setBrightness(100);
    ws2812fx.start();
  } else {
    ws2812fx.setBrightness(0);
    ws2812fx.stop();
  }
}

void setupFirePlace(){
  ws2812fx.init();
  ws2812fx.setBrightness(70);
  int speed = LED_COUNT * 100;
  ws2812fx.setSpeed(speed);
  ws2812fx.setColor(255, 69,3) ;
  ws2812fx.setMode(FX_MODE_FIRE_FLICKER_INTENSE);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("fireplace")) {
      Serial.println("connected");
      client.subscribe("fireplace");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
