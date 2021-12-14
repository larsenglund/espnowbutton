//#include <ESP8266WiFi.h> // ONLY ON ESP8266
//#include <espnow.h> // ONLY ON ESP8266
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <SoftwareSerial.h>
#include <FastLED.h>

#define LED_PIN 18
#define NUM_LEDS 1

uint8_t newMACAddress[] = {0xB4, 0xE6, 0x2D, 0x45, 0x8F, 0x4B}; // Change MAC to the one hardcoded in the sender

SoftwareSerial ESPserial(5, 4); // RX | TX

CRGB leds[NUM_LEDS];

typedef struct struct_message {
  char a[8];
} struct_message;

struct_message myData;
bool gotpacket = false;

int blinks = 1;
unsigned long lastblink = 0;
int blinklen = 1000;
bool blink_status = 0;


// Callback function that will be executed when data is received
//void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) { // ONLY ON ESP8266
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Char: ");
  Serial.println(myData.a);
  Serial.println();
  ESPserial.println(myData.a);
  gotpacket = true;
  blinks = 6;
  blinklen = 150;
}


void setup(){
  Serial.begin(115200);
  Serial.println("setup()");

  //pinMode(LED_BUILTIN, OUTPUT); // ONLY ON ESP8266
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 10);
  FastLED.setBrightness(20);
  FastLED.clear();
  leds[0] = CRGB(255, 0, 0 );
  FastLED.show();

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  
  Serial.print("Old ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);
  Serial.print("New ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  ESPserial.begin(19200);
 
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  //WiFi.mode(WIFI_AP);
  //wifi_set_macaddr(SOFTAP_IF, &mac[0]); //Note: by setting a specific MAC you can replace this slave ESP8266 device with a new one and the new slave will still pick up the data from controllers which use that MAC
  Serial.print("This node AP mac: "); Serial.println(WiFi.softAPmacAddress());
  Serial.print("This node STA mac: "); Serial.println(WiFi.macAddress());

  // Once ESPNow is successfully Init, we will register for recv CB to get recv packer info
  //esp_now_set_self_role(ESP_NOW_ROLE_SLAVE); // ONLY ON ESP8266
  esp_now_register_recv_cb(OnDataRecv);

  delay(4000);
  ESPserial.println("espnow receiver ready");
  
  leds[0] = CRGB(0, 255, 0 );
  FastLED.show();
}
 
void loop(){
  unsigned long now = millis();
  if (blinks > 0 && (now - lastblink > blinklen)) {
    lastblink = now;
    blink_status = !blink_status;
    //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // ONLY ON ESP8266
    leds[0] = CRGB(0, blink_status ? 255 : 0, 0);
    if (blinks > 1) {
      leds[0] = CRGB(0, 0, blink_status ? 255 : 0);
    }
    FastLED.show();
    blinks --;
    if (blinks == 0) {
      blinks = 1;
      blinklen = 1000;
    }
  }
  /*
  ESPserial.println("Receiver");
  while (ESPserial.available()) {
    Serial.write(ESPserial.read());
  }
  */
}
