#include <HardwareSerial.h>

#define RX        16
#define TX        17

HardwareSerial Serial_2(2);

void setup() {
  Serial_2.begin(115200, SERIAL_8N1, RX, TX);
}

void loop() {
  Serial_2.println("ciao");
  delay(2000);
}
