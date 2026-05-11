#include <esp_now.h>
#include <WiFi.h>
#include <USB.h>

const int MY_ID = 1;      
const int TARGET_ID = (MY_ID == 1) ? 2 : 1; 
const int AUTH_KEY = 1337;

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

struct Message {
  int targetId;   
  int senderId;
  int authCode;
  char text[240]; 
};

esp_now_peer_info_t peerInfo;

void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  if (len < sizeof(Message)) return;
  Message msg;
  memcpy(&msg, incomingData, sizeof(msg));

  if (msg.authCode != AUTH_KEY) return;

  if (msg.targetId == 0 || msg.targetId == MY_ID) {
    String receivedText = String(msg.text);
    if (MY_ID == 1) {
      Serial.println(receivedText); 
    } 
    else if (MY_ID == 2) {
      Serial1.println(receivedText); 
    }
  }
}

void setup() {
  USB.VID(0x0451); 
  USB.PID(0xE008);
  USB.manufacturerName("Texas Instruments");
  USB.productName("TI-Innovator Hub");
  USB.begin();

  Serial.begin(115200); 
  Serial1.begin(115200, SERIAL_8N1, 18, 17);
  
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) return;
  
  esp_now_register_recv_cb(OnDataRecv);
  
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) return;
}

void loop() {
  
  if (Serial.available() > 0 || Serial1.available() > 0) {
    String input = (Serial.available() > 0) ? Serial.readStringUntil('\n') : Serial1.readStringUntil('\n');
    input.trim();
    if (input.length() == 0) return;

    Message out;
    out.senderId = MY_ID;
    out.authCode = AUTH_KEY;
    memset(out.text, 0, 240);

    if (input.startsWith("@all: ")) {
        out.targetId = 0; 
        strncpy(out.text, input.substring(6).c_str(), 239);
    } 
    else if (MY_ID == 1 && input.startsWith("@esp: sudo spm install pkgset ")) {
        out.targetId = 2;
        String pkgName = input.substring(30);
        strncpy(out.text, pkgName.c_str(), 239);
    } 
    else {
        out.targetId = TARGET_ID;
        strncpy(out.text, input.c_str(), 239);
    }

    esp_now_send(broadcastAddress, (uint8_t *) &out, sizeof(out));
  }
}
