#include <Arduino.h>
#include "LoRaDriver.h"
#include "tx_node.h"

// instance
extern HardwareSerial LoRaSerial;
extern LoRaDriver lora;

// Tham số LoRa cho node TX
const int   LORA_ADDR_TX     = 1;          // địa chỉ node TX
const int   LORA_NET_ID      = 5;          
const long  LORA_FREQ        = 915000000;  // 915 MHz 
const int   LORA_SF          = 8;
const int   LORA_BW          = 7;
const int   LORA_CR          = 1;
const int   LORA_PREAMBLE    = 4;
const int   LORA_POWER       = 22;

static unsigned long lastSend = 0;
static const unsigned long SEND_INTERVAL = 3000; // 3 giây
static uint32_t pktCounter = 0;

static void configLoRaTx() {
  Serial.println("Config LoRa (TX node)");

  Serial.print("Set ADDRESS = ");
  Serial.print(LORA_ADDR_TX);
  Serial.println(lora.setAddress(LORA_ADDR_TX) ? "  [OK]" : "  [FAIL]");

  Serial.print("Set NETWORKID = ");
  Serial.print(LORA_NET_ID);
  Serial.println(lora.setNetwork(LORA_NET_ID) ? "  [OK]" : "  [FAIL]");

  Serial.print("Set BAND = ");
  Serial.print(LORA_FREQ);
  Serial.println(lora.setBand(LORA_FREQ) ? "  [OK]" : "  [FAIL]");

  Serial.print("Set PARAMETER = ");
  Serial.print(LORA_SF);
  Serial.print(",");
  Serial.print(LORA_BW);
  Serial.print(",");
  Serial.print(LORA_CR);
  Serial.print(",");
  Serial.print(LORA_PREAMBLE);
  Serial.println(
    lora.setParameter(LORA_SF, LORA_BW, LORA_CR, LORA_PREAMBLE)
    ? "  [OK]" : "  [FAIL]"
  );

  Serial.print("Set POWER = ");
  Serial.print(LORA_POWER);
  Serial.println(lora.setPower(LORA_POWER) ? "  [OK]" : "  [FAIL]");

  Serial.println("TX Config done");
}

void setupTx() {
  Serial.println("[TX] Node starting...");
  configLoRaTx();
}

void loopTx() {
  unsigned long now = millis();

  // Định kỳ gửi gói
  if (now - lastSend >= SEND_INTERVAL) {
    lastSend = now;

    String msg = "PKT " + String(pktCounter++);
    Serial.print("[TX] Sending: ");
    Serial.println(msg);

    bool ok = lora.send(msg);
    Serial.println(ok ? "[TX] Send command OK" : "[TX] Send command FAIL");
  }

  // Đọc mọi thứ module trả về (OK, lỗi, ...)
  while (LoRaSerial.available()) {
    String line = lora.readLine();
    line.trim();
    if (line.length() > 0) {
      Serial.print("[LoRa TX RAW] ");
      Serial.println(line);
    }
  }
}
