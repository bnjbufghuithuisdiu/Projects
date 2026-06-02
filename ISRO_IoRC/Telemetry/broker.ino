#include <WiFi.h>
#include <sMQTTBroker.h>

//MQTT credential
const char* MQTT_CLIENT_USER ="user";
const char* MQTT_CLIENT_PASSWORD ="password";

//AP credential
const char* ap_ssid ="ESP32_Broker";
const char* ap_password ="12345678";

class MyBroker: public sMQTTBroker
{
public:
    bool onEvent(sMQTTEvent *event) override
    {
        switch(event->Type())
        {
        //New Client Connected
        case NewClient_sMQTTEventType:
        {
            sMQTTNewClientEvent *e=(sMQTTNewClientEvent*)event;

            Serial.print("New client:");
            Serial.println(e->Client()->getClientId().c_str());

            Serial.print("Username:");
            Serial.println(e->Login().c_str());

            Serial.print("Password:");
            Serial.println(e->Password().c_str());

            // Authentication
            if ((e->Login()!=MQTT_CLIENT_USER) ||
                (e->Password()!=MQTT_CLIENT_PASSWORD)) {
                Serial.println("invalid credentials");
                return false;
            }

            Serial.println("client authenticated");
        }
        break;

        //client disconnected normally
        case RemoveClient_sMQTTEventType:
        {
            sMQTTRemoveClientEvent*e=sMQTTRemoveClientEvent*)event;

            // Serial.print("Client disconnected: ");
            // Serial.println(e->Client()->getClientId().c_str());
        }
        break;

        // lost connection
        case LostConnect_sMQTTEventType:
            // Serial.println("Broker lost connection!");
        break;

        //Client Published Message
        case Public_sMQTTEventType:
        {
            sMQTTPublicClientEvent *e=(sMQTTPublicClientEvent*)event;

            
            std::string payload=e->Payload();
            // Serial.print("Topic: ");
            // Serial.println(e->Topic().c_str());
            uint8_t value[200];
            memcpy(value,payload.data(),sizeof(value));
            // if(e->Topic()=="clientUAV/altitude"){
            //   Serial.println(payload.data());
            // }
            // else{
            //   Serial.print("Decoded INT:");
            //   for(int i=0;i<200;i++){
            //     Serial.print(value[i]);
            //     Serial.print(" ,");
            //   }
            // }
            

            
            Serial.println();
          
        }
        break;

        case Subscribe_sMQTTEventType:
        {
            sMQTTSubUnSubClientEvent *e=(sMQTTSubUnSubClientEvent*)event;
            
            // Serial.print("Client");
            // Serial.print(e->Client()->getClientId().c_str());
            // Serial.print(" subscribed to: ");
            // Serial.println(e->Topic().c_str());
        }
        break;

        //  Unsubscribe
        case UnSubscribe_sMQTTEventType:
        {
            sMQTTSubUnSubClientEvent *e=(sMQTTSubUnSubClientEvent*)event;

            //Serial.print("Client");
            //Serial.print(e->Client()->getClientId().c_str());
            //Serial.print(" unsubscribed from:");
            //Serial.println(e->Topic().c_str());
        }
        break;
        }

        return true;
    }
};

MyBroker broker;
unsigned long lastPublish=0;

void setup()
{
    Serial.begin(115200);

    // Start Access Point
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid,ap_password);

    Serial.println("ESP32 AP Started");
    Serial.print("SSID:");
    Serial.println(ap_ssid);

    Serial.print("IP Address:");
    Serial.println(WiFi.softAPIP()); // usually 192.168.4.1-let's see

    //start MQTT Broker
    broker.init(1883);
    Serial.println("MQTT Broker started on port 1883");
}

void loop()
{
    broker.update();

    //Broker publishes sensor data every 2 sec
    if (millis()-lastPublish >2000)
    {
        lastPublish=millis();

        int sensor=random(300);  // example sensor pin

        String payload="{\"sensor\":"+String(sensor) + "}";

        broker.publish("sensor/data",payload.c_str());

        //Serial.print("Broker Published: ");
        //Serial.println(payload);
    }
    if(Serial.available()){
      String cmd=Serial.readStringUntil('\n');
      Serial.println(cmd=="ON ");
      cmd.trim();
      if(cmd=="ON"){
        
        broker.publish("clientUAV/charging","ON");
      }
    }
}