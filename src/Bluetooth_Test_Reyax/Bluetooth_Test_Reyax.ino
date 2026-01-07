#include "BluetoothSerial.h"

#define TRANSMITTER

#define NO_AMPLIFIER

#define LORA_TX 27
#define LORA_RX 14
#define LORA_RST 26

BluetoothSerial SerialBT;
const char *pin = "1234"; // Mật khẩu ghép đôi bluetooth

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

  // Cấu hình các thông số lora quan trọng
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

int counter = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("--- RYLR998 Transmitter ---");

  SerialBT.setPin(pin, 4);

  // Đặt tên bluetooth dựa trên cấu hình
#ifdef AMPLIFIER
  SerialBT.begin("Transmitter_Amplifier");
#else
  SerialBT.begin("Transmitter_No_Amplifier");
#endif

  Serial.println("Bluetooth Transmitter ready. PIN: 1234");

  setupLoRa();
  Serial2.println("AT+ADDRESS=2"); // Địa chỉ máy phát là 2
  delay(200);
}

unsigned long lastSendTime = 0;

void loop()
{
  // Gửi tin nhắn mỗi 3 giây
  if (millis() - lastSendTime >= 3000)
  {
    lastSendTime = millis();
    String message = "Packet " + String(counter);

    // Gửi lệnh AT để bắn tin nhắn đi
    Serial2.print("AT+SEND=1,"); // Gửi tới địa chỉ 1
    Serial2.print(message.length());
    Serial2.print(",");
    Serial2.println(message);

    // In ra Serial Monitor (cáp USB)
    Serial.println("Sending: " + message);

    // Gửi thông báo về điện thoại qua bluetooth (nếu đang kết nối)
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

#else

void setup()
{
  Serial.begin(115200);
  Serial.println("--- RYLR998 Receiver ---");

  SerialBT.setPin(pin, 4);

  // Đặt tên Bluetooth dựa trên cấu hình
#ifdef AMPLIFIER
  SerialBT.begin("Receiver_Amplifier");
#else
  SerialBT.begin("Receiver_No_Amplifier");
#endif

  Serial.println("Bluetooth Receiver ready. PIN: 1234");

  setupLoRa();
  Serial2.println("AT+ADDRESS=1"); // Địa chỉ máy thu là 1
  delay(200);
}

void loop()
{
  // Kiểm tra xem LoRa có nhận được gì không
  if (Serial2.available())
  {
    String response = Serial2.readStringUntil('\n');
    response.trim();

    if (response.startsWith("+RCV="))
    {
      // Bóc tách dữ liệu (Parsing)
      int idx1 = response.indexOf(',');
      int idx2 = response.indexOf(',', idx1 + 1);
      int idx3 = response.indexOf(',', idx2 + 1);
      int idx4 = response.indexOf(',', idx3 + 1);

      String sender = response.substring(5, idx1);
      String message = response.substring(idx2 + 1, idx3);
      String rssi = response.substring(idx3 + 1, idx4);
      String snr = response.substring(idx4 + 1);

      // In ra Serial Monitor (cáp USB)
      Serial.println("--- New LoRa Message ---");
      Serial.println("From: " + sender);
      Serial.println("Data: " + message);
      Serial.println("RSSI: " + rssi + " dBm");
      Serial.println("SNR: " + snr + " dB");

      // Gửi dữ liệu về điện thoại qua Bluetooth
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