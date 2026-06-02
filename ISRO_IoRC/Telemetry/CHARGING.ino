#include <WiFi.h>
#include <PubSubClient.h>

#define RELAY_PIN 2

//WiFi credential
const char* ssid="ESP32_Broker";
const char* password="12345678";

//MQTT broker
const char* mqtt_server="192.168.4.1";
const int mqtt_port = 1883;

//MQTT credential
const char* mqtt_user="user";  //i have change this-----
const char* mqtt_password="password";

//topics
const char* pub_topic="clientUAV/charging";

//object
WiFiClient espClient;
PubSubClient client(espClient);

//variable
String inputString="";
int count=0;
bool relayState=false;

//WiFi setup
void setup_wifi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status()!=WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// MQTT callback
void callback(char* topic, byte* payload, unsigned int length) {
  String message="";

  for (int i = 0; i < length; i++) {
    message+=(char)payload[i];
  }
  Serial.println(message=="on");
  if(message=="on"){
    
    processCommand(message);
  }
 

  Serial.print("MQTT Received [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  if (message=="ON") {
    digitalWrite(RELAY_PIN, HIGH);
    relayState=true;
    Serial.println("relay ON via MQTT");
  } 
  else if (message=="OFF") {
    digitalWrite(RELAY_PIN, LOW);
    relayState=false;
    count = 0;
    Serial.println("relay OFF via MQTT");
  }
}

//MQTT reconnect 
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");

    String clientId = "ESP32-" +String(WiFi.macAddress());

    
    if (client.connect(clientId.c_str(),mqtt_user, mqtt_password)) {
      Serial.println("Connected");

      // Subscribe topic
      client.subscribe("clientUAV/charging");

    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" → retrying in 2 sec");
      delay(2000);
    }
  }
}

//setup 
void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  inputString.reserve(50);
}

//Loop
void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  while (Serial.available()) {
    String incomingChar = Serial.readStringUntil('\n');
    processCommand(incomingChar);

    if (incomingChar=="\n") {
      inputString="";
    } else {
      inputString+=incomingChar;
    }
  }

  delay(200);
}

// ----------- Command Logic (UNCHANGED) -----------
void processCommand(String cmd) {
  cmd.trim();

  Serial.print("received:");
  Serial.println(cmd);

  if (cmd=="ON") {
    count++;
    Serial.print("valid count: ");
    Serial.println(count);
  } 
  else if (cmd== "OFF") {
    count= 0;
    digitalWrite(RELAY_PIN, LOW);
    relayState=false;
    Serial.println("Invalid signal → Reset count");
  }

  if (count>= 3 && !relayState) {
    digitalWrite(RELAY_PIN, HIGH);
    relayState=true;

    Serial.println("Relay ON (Latched)");

    String payload= "{\"id\":\"charging\",\"status\":\"ON\"}";
    client.publish(pub_topic, payload.c_str(), true);

    Serial.println("Published:" + payload);
  }
}