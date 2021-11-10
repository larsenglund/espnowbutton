#include <ESP8266WiFi.h>
#include <espnow.h>
#include <SoftwareSerial.h>

SoftwareSerial ESPserial(5, 4); // RX | TX

typedef struct struct_message {
  char a[8];
} struct_message;

struct_message myData;
bool gotpacket = false;

int blinks = 1;
unsigned long lastblink = 0;
int blinklen = 1000;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
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
  Serial.println();

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  
  Serial.print("ESP8266 Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  pinMode(LED_BUILTIN, OUTPUT);

  ESPserial.begin(115200);

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
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop(){
  unsigned long now = millis();
  if (blinks > 0 && (now - lastblink > blinklen)) {
    lastblink = now;
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
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
