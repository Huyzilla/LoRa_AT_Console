#include <HardwareSerial.h>

#define LORA_TX 27     // ESP32 TX → RYLR998 RX
#define LORA_RX 14     // ESP32 RX ← RYLR998 TX
#define LORA_RST 26    // Reset module

HardwareSerial LoRaSerial(1);  // UART1

unsigned long lastSend = 0;
const unsigned long interval = 1000;  // Gửi AT mỗi 1 giây

void resetLoRa() {
  pinMode(LORA_RST, OUTPUT);
  digitalWrite(LORA_RST, LOW);   // Giữ thấp để reset
  delay(100);
  digitalWrite(LORA_RST, HIGH);  // Nhả reset
  delay(500);
}

void setup() {
  Serial.begin(115200);  // Cho Serial Monitor
  delay(1000);

  LoRaSerial.begin(115200, SERIAL_8N1, LORA_RX, LORA_TX);  // UART1 cho RYLR998
  resetLoRa();

  Serial.println("🔁 ESP32 bắt đầu gửi 'AT' mỗi 1 giây...");
}

void loop() {
  unsigned long now = millis();

  // Gửi AT mỗi 1000ms
  if (now - lastSend >= interval) {
    lastSend = now;
    LoRaSerial.print("AT\r\n");  // Gửi AT có kết thúc CR+LF
    Serial.println(">> Gửi: AT");
  }

  // Nếu có phản hồi từ RYLR998
  while (LoRaSerial.available()) {
    String resp = LoRaSerial.readStringUntil('\n');
    resp.trim();  // Xoá \r hoặc khoảng trắng cuối
    if (resp.length() > 0) {
      Serial.println("[LoRa] " + resp);
    }
  }
}
