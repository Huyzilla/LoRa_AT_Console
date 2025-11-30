#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>

class LoRaDriver {
public:
    LoRaDriver(HardwareSerial &serial, int pinReset);

    void begin(long baud = 115200);
    void resetModule();

    bool send(const String &msg);
    bool setAddress(int addr);
    bool setNetwork(int id);
    bool setBand(long freq);
    bool setParameter(int sf, int bw, int cr, int preamble);
    bool setPower(int power);

    String readLine();

private:
    HardwareSerial &lora;
    int rstPin;

    bool sendCmd(const String &cmd, const String &expect = "OK", uint32_t timeout = 1000);
};
