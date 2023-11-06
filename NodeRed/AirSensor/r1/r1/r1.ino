//https://docs.arduino.cc/tutorials/projects/control-your-iot-cloud-kit-via-mqtt-and-node-red

 
#define BROKER_IP    "192.168.10.121" // important: you have to change this to your IP brocker Addres
#define DEV_NAME     "AQsensor"
#define MQTT_USER    "mqtt_u"
#define MQTT_PW      "mqtt_p"

const char ssid[] = "emily_2GEXT;
const char pass[] = "qazwsxed";

#include <MQTT.h>
#include <WiFi.h>


int R1 = 1;
int R2 = 2;
int led =  LED_BUILTIN;
uint32_t elaps = 0;

WiFiClient net;
MQTTClient client;
unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect(DEV_NAME, MQTT_USER, MQTT_PW)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nconnected!");
  //client.subscribe("/hello"); //SUBSCRIBE TO TOPIC /hello
  client.subscribe("/relay1"); //SUBSCRIBE TO TOPIC /hello
  client.subscribe("/relay2"); //SUBSCRIBE TO TOPIC /hello


  client.subscribe("/temp"); //SUBSCRIBE TO TOPIC /t
  client.subscribe("/humidity"); //SUBSCRIBE TO TOPIC /h
  client.subscribe("/pressure"); //SUBSCRIBE TO TOPIC /p
  client.subscribe("/light"); //SUBSCRIBE TO TOPIC /l
}
void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  if (topic == "/relay1") {
    if (payload == "true") {
      digitalWrite(R1, HIGH);
      digitalWrite(led, HIGH);

    } else if (payload == "false") {

      digitalWrite(R1, LOW);
      digitalWrite(led, LOW);
    }
  }

  if (topic == "/relay2") {
    if (payload == "true") {
      digitalWrite(R2, HIGH);
      digitalWrite(led, HIGH);

    } else if (payload == "false") {

      digitalWrite(R2, LOW);
      digitalWrite(led, LOW);
    }
  }

}
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported by Arduino.
  // You need to set the IP address directly.
  //
  // MQTT brokers usually use port 8883 for secure connections.
  client.begin(BROKER_IP, 1883, net);
  client.onMessage(messageReceived);
  connect();

  pinMode(led, OUTPUT);
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);

  if (!ENV.begin()) {
    Serial.println("Failed to initialize MKR ENV shield!");
    while (1);
  }

}

void loop() {

  // read enrivornmental data

  float t = ENV.readTemperature();
  float h = ENV.readHumidity();
  float p = ENV.readPressure();
  float l = ENV.readLux();

  client.loop();
  if (!client.connected()) {
    connect();
  }
  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    client.publish("/hello", "world"); //PUBLISH TO TOPIC /hello MSG world
    client.publish("/temp", String(t)); //SUBSCRIBE TO TOPIC /temp, pass the value as a string
    client.publish("/humidity", String(h)); //SUBSCRIBE TO TOPIC /humidity, pass the value as a string
    client.publish("/pressure", String(p)); //SUBSCRIBE TO TOPIC /pressure, pass the value as a string
    client.publish("/light", String(l)); //SUBSCRIBE TO TOPIC /light, pass the value as a string
  }




}
