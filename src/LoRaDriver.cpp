#include "LoRaDriver.h"

LoRaDriver::LoRaDriver(HardwareSerial &serial, int pinReset)
    : lora(serial), rstPin(pinReset) {}

void LoRaDriver::begin(long baud) {
    lora.begin(baud, SERIAL_8N1);
    pinMode(rstPin, OUTPUT);
    resetModule();
}

void LoRaDriver::resetModule() {
    digitalWrite(rstPin, LOW);
    delay(100);
    digitalWrite(rstPin, HIGH);
    delay(500);
}

bool LoRaDriver::sendCmd(const String &cmd, const String &expect, uint32_t timeout) {
    lora.print(cmd + "\r\n");

    uint32_t t0 = millis();
    while (millis() - t0 < timeout) {
        if (lora.available()) {
            String line = readLine();
            if (line.indexOf(expect) >= 0) return true;
        }
    }
    return false;
}

String LoRaDriver::readLine() {
    return lora.readStringUntil('\n');
}

bool LoRaDriver::setAddress(int addr) {
    return sendCmd("AT+ADDRESS=" + String(addr));
}

bool LoRaDriver::setNetwork(int id) {
    return sendCmd("AT+NETWORKID=" + String(id));
}

bool LoRaDriver::setBand(long freq) {
    return sendCmd("AT+BAND=" + String(freq));
}

// sf = 7–12, bw = 0–9 (7=125kHz), cr 1–4, preamble = 1–65535
bool LoRaDriver::setParameter(int sf, int bw, int cr, int preamble) {
    return sendCmd("AT+PARAMETER=" + String(sf) + "," + String(bw) + "," + String(cr) + "," + String(preamble));
}

bool LoRaDriver::setPower(int power) {
    return sendCmd("AT+CRFOP=" + String(power));
}

// Gửi tin dạng AT+SEND=0,<len>,<msg>
bool LoRaDriver::send(const String &msg) {
    String cmd = "AT+SEND=0," + String(msg.length()) + "," + msg;
    return sendCmd(cmd);
}
