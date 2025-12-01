#include <Arduino.h>
#include "LoRaDriver.h"
#include "rx_node.h"

// instance 
extern HardwareSerial LoRaSerial;
extern LoRaDriver lora;

// Tham số LoRa cho node RX
const int   LORA_ADDR_RX     = 2;          // địa chỉ node RX
const int   LORA_NET_ID      = 5;          
const long  LORA_FREQ        = 915000000;  
const int   LORA_SF          = 8;
const int   LORA_BW          = 7;
const int   LORA_CR          = 1;
const int   LORA_PREAMBLE    = 4;
const int   LORA_POWER       = 22;

// Cấu trúc lưu gói tin nhận được
struct LoRaPacket {
  bool   valid;
  int    addr;
  int    len;
  String msg;
  int    rssi;
  int    snr;
};

// Hàm parse chuỗi dạng: +RCV=addr,len,msg,rssi,snr
static bool parseRcvLine(const String &line, LoRaPacket &pkt) {
  pkt.valid = false;

  if (!line.startsWith("+RCV=")) {
    return false;
  }

  String payload = line.substring(5); // sau "+RCV="

  int idx1 = payload.indexOf(',');
  if (idx1 < 0) return false;
  int idx2 = payload.indexOf(',', idx1 + 1);
  if (idx2 < 0) return false;
  int idx3 = payload.indexOf(',', idx2 + 1);
  if (idx3 < 0) return false;
  int idx4 = payload.indexOf(',', idx3 + 1);
  if (idx4 < 0) return false;

  String sAddr = payload.substring(0, idx1);
  String sLen  = payload.substring(idx1 + 1, idx2);
  String sMsg  = payload.substring(idx2 + 1, idx3);
  String sRssi = payload.substring(idx3 + 1, idx4);
  String sSnr  = payload.substring(idx4 + 1);

  sAddr.trim();
  sLen.trim();
  sMsg.trim();
  sRssi.trim();
  sSnr.trim();

  pkt.addr = sAddr.toInt();
  pkt.len  = sLen.toInt();
  pkt.msg  = sMsg;
  pkt.rssi = sRssi.toInt();
  pkt.snr  = sSnr.toInt();
  pkt.valid = true;

  return true;
}

static void configLoRaRx() {
  Serial.println("Config LoRa (RX node)");

  Serial.print("Set ADDRESS = ");
  Serial.print(LORA_ADDR_RX);
  Serial.println(lora.setAddress(LORA_ADDR_RX) ? "  [OK]" : "  [FAIL]");

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

  Serial.println("RX Config done");
}

void setupRx() {
  Serial.println("[RX] Node starting...");
  configLoRaRx();
  Serial.println("Dang cho du lieu +RCV=... tu module LoRa.");
}

void loopRx() {
  while (LoRaSerial.available()) {
    String line = lora.readLine();
    line.trim();
    if (line.length() == 0) continue;

    Serial.print("[LoRa RX RAW] ");
    Serial.println(line);

    LoRaPacket pkt;
    if (parseRcvLine(line, pkt) && pkt.valid) {
      Serial.println("---- Packet Received ----");
      Serial.print("From addr : ");
      Serial.println(pkt.addr);
      Serial.print("Length    : ");
      Serial.println(pkt.len);
      Serial.print("Message   : ");
      Serial.println(pkt.msg);
      Serial.print("RSSI      : ");
      Serial.println(pkt.rssi);
      Serial.print("SNR       : ");
      Serial.println(pkt.snr);
      Serial.println("-------------------------");
    }
  }
}
