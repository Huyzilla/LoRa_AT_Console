//Gọi thư viện để điều khiển các cổng giao tiếp nối tiếp (Serial) phần cứng của ESP32.
#include <HardwareSerial.h>


// UART (Universal Asynchronous Receiver-Transmitter)
// Đây là giao thức truyền thông dùng 2 dây: 1 dây để gửi (TX - Transmit) và 1 dây để nhận (RX - Receive).
// Nguyên tắc sống còn: Chân TX của thiết bị này phải nối vào chân RX của thiết bị kia (Nói thì phải có người Nghe).
// ESP32 thường có 3 bộ UART phần cứng (UART0, UART1, UART2).



// Định nghĩa tên cho các chân (Pin) của ESP32 để dễ nhớ.
#define LORA_TX 27     // ESP32 TX → RYLR998 RX
#define LORA_RX 14     // ESP32 RX ← RYLR998 TX
#define LORA_RST 26    // Reset module

// Tạo một đối tượng (biến) tên là LoRaSerial để quản lý giao tiếp với module LoRa.
// Chúng ta chọn sử dụng bộ UART 1 của ESP32.
// Trong ESP32, bộ UART 0 (Serial) thường dùng để nạp code và in ra màn hình máy tính
//  qua cổng USB. Nên ta dùng UART 1 cho thiết bị ngoại vi (LoRa).
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
