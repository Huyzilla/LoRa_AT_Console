#include <U8g2lib.h>

#define LORA_TX 27     // ESP32 TX → RYLR998 RX
#define LORA_RX 14     // ESP32 RX ← RYLR998 TX
#define LORA_RST 26    // Reset module

  #define TRANSMITTER 1
  // #define RECEIVER 0

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void resetLoRa() {
  pinMode(LORA_RST, OUTPUT);
  digitalWrite(LORA_RST, LOW);
  delay(100);
  digitalWrite(LORA_RST, HIGH);
  delay(1000);
}

void setupLoRa() {
  Serial2.begin(115200, SERIAL_8N1, LORA_RX, LORA_TX);
  resetLoRa();

  // Thiết lập cấu hình cơ bản
  Serial2.println("AT+ADDRESS=2");           // mặc định cho transmitter
  delay(200);
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

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("RYLR998 Sender");

  setupLoRa();
}

void loop() {
  String message = String(counter);
  Serial.println("Sending: " + message);

  Serial2.print("AT+SEND=1,");
  Serial2.print(message.length());
  Serial2.print(",");
  Serial2.println(message);

  counter++;
  if(counter == 255) counter = 0;
  delay(1000);
}

#else

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("RYLR998 Receiver");

  u8g2.begin(); 
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.clearBuffer();
  u8g2.drawStr(2, 12, "Init success");
  u8g2.sendBuffer();

  setupLoRa();

  // Đặt lại địa chỉ là 1 nếu là receiver
  Serial2.println("AT+ADDRESS=1");
  delay(200);
}

#endif
