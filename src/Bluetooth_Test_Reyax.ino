#include "BluetoothSerial.h"
// Thư viện U8g2lib sẽ được gọi bên dưới, phụ thuộc vào #define

// =================================================
// --- CẤU HÌNH TÍNH NĂNG ---
// #define USE_OLED // Bỏ ghi chú dòng này nếu bạn CÓ sử dụng màn hình OLED

// --- CHỌN CHẾ ĐỘ HOẠT ĐỘNG ---
#define TRANSMITTER
// #define RECEIVER

// --- CHỌN THIẾT BỊ ---
// #define AMPLIFIER
#define NO_AMPLIFIER
// =================================================

// --- CẤU HÌNH PHẦN CỨNG ---
#define LORA_TX 27
#define LORA_RX 14
#define LORA_RST 26

// --- CÁC ĐỐI TƯỢNG TOÀN CỤC ---
BluetoothSerial SerialBT;
const char *pin = "1234";

// --- CÁC ĐỐI TƯỢNG VÀ BIẾN DÀNH RIÊNG CHO OLED ---
#ifdef USE_OLED
#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
bool isOledConnected = false;
#endif

// --- CÁC HÀM CHUNG ---
void resetLoRa()
{
  pinMode(LORA_RST, OUTPUT);
  digitalWrite(LORA_RST, LOW);
  delay(100);
  digitalWrite(LORA_RST, HIGH);
  delay(1000);
}

void setupLoRa()
{
  Serial2.begin(115200, SERIAL_8N1, LORA_RX, LORA_TX);
  resetLoRa();

  Serial2.println("AT+NETWORKID=5");
  delay(200);
  Serial2.println("AT+BAND=915000000");
  delay(200);
  Serial2.println("AT+CRFOP=22");
  delay(200);
  Serial2.println("AT+PARAMETER=12,0,4,8");
  delay(500);
}

#ifdef TRANSMITTER
// **************************************************
// * MÃ DÀNH RIÊNG CHO BỘ PHÁT (TRANSMITTER)      *
// **************************************************
int counter = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("--- RYLR998 Transmitter ---");

#ifdef USE_OLED
  // Kiểm tra khởi tạo OLED
  if (u8g2.begin())
  {
    isOledConnected = true;
    Serial.println("OLED display initialized successfully.");
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setDrawColor(1);
    u8g2.setFontPosTop();
  }
  else
  {
    Serial.println("OLED initialization failed! Check wiring. Disabling display functions.");
  }
#endif

  SerialBT.setPin(pin, 4);

#ifdef AMPLIFIER
  SerialBT.begin("Transmitter_Amplifier");
#endif
#ifdef NO_AMPLIFIER
  SerialBT.begin("Transmitter_No_Amplifier");
#endif

  Serial.println("Bluetooth Transmitter ready. PIN: 1234");

#ifdef USE_OLED
  // Chỉ hiển thị lên màn hình nếu kết nối thành công
  if (isOledConnected)
  {
    u8g2.clearBuffer();
    u8g2.drawStr(2, 12, "LoRa Transmitter");
    u8g2.drawStr(2, 28, "BT Ready, PIN: 1234");
    u8g2.sendBuffer();
  }
#endif

  setupLoRa();
  Serial2.println("AT+ADDRESS=2");
  delay(200);
}

unsigned long lastSendTime = 0;

void loop()
{
  if (millis() - lastSendTime >= 3000)
  {
    lastSendTime = millis();
    String message = "Packet " + String(counter);

    Serial2.print("AT+SEND=1,");
    Serial2.print(message.length());
    Serial2.print(",");
    Serial2.println(message);

    Serial.println("Sending: " + message);

#ifdef USE_OLED
    if (isOledConnected)
    {
      u8g2.clearBuffer();
      u8g2.drawStr(2, 2, "Sending Status:");
      u8g2.drawStr(12, 18, message.c_str());
      u8g2.drawStr(2, 40, "To Address: 1");
      u8g2.sendBuffer();
    }
#endif

    if (SerialBT.hasClient())
    {
      SerialBT.println("--- Transmitter Status ---");
      SerialBT.println("Sending: " + message);
      SerialBT.println("To Address: 1");
      SerialBT.println();
    }
    counter++;
  }
}

#else // RECEIVER
// **************************************************
// * MÃ DÀNH RIÊNG CHO BỘ THU (RECEIVER)            *
// **************************************************

void setup()
{
  Serial.begin(115200);
  Serial.println("--- RYLR998 Receiver ---");

#ifdef USE_OLED
  // Kiểm tra khởi tạo OLED
  if (u8g2.begin())
  {
    isOledConnected = true;
    Serial.println("OLED display initialized successfully.");
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setDrawColor(1);
    u8g2.setFontPosTop();
  }
  else
  {
    Serial.println("OLED initialization failed! Check wiring. Disabling display functions.");
  }
#endif

  SerialBT.setPin(pin, 4);
  SerialBT.begin("ESP32_LoRa_Receiver");

#ifdef AMPLIFIER
  SerialBT.begin("Receiver_Amplifier");
#endif
#ifdef NO_AMPLIFIER
  SerialBT.begin("Receiver_No_Amplifier");
#endif

  Serial.println("Bluetooth Receiver ready. PIN: 1234");

#ifdef USE_OLED
  if (isOledConnected)
  {
    u8g2.clearBuffer();
    u8g2.drawStr(2, 12, "LoRa Receiver");
    u8g2.drawStr(2, 28, "BT Ready, PIN: 1234");
    u8g2.drawStr(2, 44, "Waiting for data...");
    u8g2.sendBuffer();
  }
#endif

  setupLoRa();
  Serial2.println("AT+ADDRESS=1");
  delay(200);
}

void loop()
{
  if (Serial2.available())
  {
    String response = Serial2.readStringUntil('\n');
    response.trim();

    if (response.startsWith("+RCV="))
    {
      int idx1 = response.indexOf(',');
      int idx2 = response.indexOf(',', idx1 + 1);
      int idx3 = response.indexOf(',', idx2 + 1);
      int idx4 = response.indexOf(',', idx3 + 1);

      String sender = response.substring(5, idx1);
      String message = response.substring(idx2 + 1, idx3);
      String rssi = response.substring(idx3 + 1, idx4);
      String snr = response.substring(idx4 + 1);

      Serial.println("--- New LoRa Message ---");
      Serial.println("From: " + sender);
      Serial.println("Data: " + message);
      Serial.println("RSSI: " + rssi + " dBm");
      Serial.println("SNR: " + snr + " dB");

#ifdef USE_OLED
      // Chỉ thực thi các lệnh màn hình nếu OLED đã kết nối thành công
      if (isOledConnected)
      {
        u8g2.clearBuffer();
        u8g2.drawStr(2, 2, "Received:");
        u8g2.drawStr(12, 18, message.c_str());
        char infoLine1[30];
        snprintf(infoLine1, sizeof(infoLine1), "RSSI: %s dBm", rssi.c_str());
        u8g2.drawStr(2, 39, infoLine1);
        char infoLine2[30];
        snprintf(infoLine2, sizeof(infoLine2), "SNR: %s dB", snr.c_str());
        u8g2.drawStr(2, 52, infoLine2);
        u8g2.sendBuffer();
      }
#endif

      if (SerialBT.hasClient())
      {
        SerialBT.println("--- New LoRa Message ---");
        SerialBT.println("From: " + sender);
        SerialBT.println("Data: " + message);
        SerialBT.println("RSSI: " + rssi + " dBm");
        SerialBT.println("SNR: " + snr + " dB");
        SerialBT.println();
      }
    }
  }
}
#endif