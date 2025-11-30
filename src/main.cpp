#include <Arduino.h>
#include "LoRaDriver.h"

#define LORA_RX 14
#define LORA_TX 27
#define LORA_RST 26

HardwareSerial LoRaSerial(1);
LoRaDriver lora(LoRaSerial, LORA_RST);

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("=== Test Config LoRa ===");

    LoRaSerial.begin(115200, SERIAL_8N1, LORA_RX, LORA_TX);
    lora.begin();

    Serial.println("Setting address...");
    Serial.println(lora.setAddress(1) ? "OK" : "FAIL");

    Serial.println("Setting network...");
    Serial.println(lora.setNetwork(5) ? "OK" : "FAIL");

    Serial.println("Setting band...");
    Serial.println(lora.setBand(915000000) ? "OK" : "FAIL");

    Serial.println("Setting parameter...");
    Serial.println(lora.setParameter(8, 7, 1, 4) ? "OK" : "FAIL");

    Serial.println("Setting power...");
    Serial.println(lora.setPower(22) ? "OK" : "FAIL");

    Serial.println("--- SEND TEST ---");
    lora.send("HELLO_LORA");
}

void loop() {
    while (LoRaSerial.available()) {
        Serial.println("> " + lora.readLine());
    }
}
