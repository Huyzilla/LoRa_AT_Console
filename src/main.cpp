#include <Arduino.h>
#include "LoRaDriver.h"
#include "tx_node.h"
#include "rx_node.h"

#define LORA_RX 14
#define LORA_TX 27
#define LORA_RST 26

// Tạo instance dùng chung
HardwareSerial LoRaSerial(1);
LoRaDriver lora(LoRaSerial, LORA_RST);

#define ROLE_TX
// #define ROLE_RX

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("===== LoRa Node Start =====");

  // Khởi tạo UART1 với chân RX/TX
  LoRaSerial.begin(115200, SERIAL_8N1, LORA_RX, LORA_TX);

  // Khởi động driver + reset module
  lora.begin();

#ifdef ROLE_TX
  setupTx();
#elif defined(ROLE_RX)
  setupRx();
#else
  #error "Chua chon ROLE_TX hay ROLE_RX trong main.cpp"
#endif
}

void loop() {
#ifdef ROLE_TX
  loopTx();
#elif defined(ROLE_RX)
  loopRx();
#endif
}
