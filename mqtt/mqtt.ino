//#include <ESP8266WiFi.h> // ONLY ON ESP8266
//#include <espnow.h> // ONLY ON ESP8266
#include <WiFi.h>
#include <FastLED.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>


#define LED_PIN 18
#define NUM_LEDS 1

CRGB leds[NUM_LEDS];

// Update these with values suitable for your network.
const char* ssid = "Tot..";
const char* password = "u11..";
const char* mqtt_server = "eng..";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

SoftwareSerial ESPserial(5, 4); // RX | TX
char buff[16];
int buff_idx = 0;

int blinks = 1;
unsigned long lastblink = 0;
int blinklen = 1000;
bool blink_status = 0;

unsigned long previousMillis = 0;
unsigned long interval = 10000;


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  /*
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  }
  */
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("bikebuttonlars", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("setup()");

  //pinMode(LED_BUILTIN, OUTPUT); // ONLY ON ESP8266
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 10);
  FastLED.setBrightness(20);
  FastLED.clear();
  leds[0] = CRGB(255, 0, 0 );
  FastLED.show();
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  ESPserial.begin(19200);

  leds[0] = CRGB(0, 255, 0 );
  FastLED.show();
}

void loop() {
  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
  
  if (!client.connected()) {
    Serial.println("Reconnecting to MQTT...");
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 60000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "#%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("bikebuttonlars", msg);

    //ESPserial.println("MQTT");
  }
  while (ESPserial.available()) {
    buff[buff_idx] = ESPserial.read();
    Serial.print("Got ");
    Serial.println(buff[buff_idx]);
    if (buff[buff_idx] == '\n') {
      buff[buff_idx] = '\0';
      Serial.print("Got a whole string, publishing to MQTT: '");
      Serial.print(buff);
      Serial.println("'");
      client.publish("bikebutton", buff);
      ESPserial.flush();
      while (ESPserial.available()) {
        ESPserial.read();
      }
      buff_idx = 0;
      blinks = 6;
      blinklen = 150;
    }
    else {
      buff_idx++;
    }
  }

  if (blinks > 0 && (now - lastblink > blinklen)) {
    lastblink = now;
    blink_status = !blink_status;
    //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // ONLY ON ESP8266
    leds[0] = CRGB(WiFi.status() != WL_CONNECTED ? (blink_status ? 255 : 0) : 0, blink_status ? 255 : 0, blinks > 1 ? (blink_status ? 255 : 0) : 0);
    //if (blinks > 1) {
    //  leds[0] = CRGB(0, 0, blink_status ? 255 : 0);
    //}
    FastLED.show();
    blinks --;
    if (blinks == 0) {
      blinks = 1;
      blinklen = 1000;
    }
  }
}
