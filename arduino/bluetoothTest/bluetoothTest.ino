#include <SoftwareSerial.h>

SoftwareSerial BT(3, 2); // Arduino RX, TX

void setup() {
  Serial.begin(9600);
  BT.begin(9600);
}

void loop() {
  if (BT.available() < 1) return;
  char request = BT.read();  // Read request
  
  //BT.print("REQUEST: "); BT.println(request); // DEBUG

  if (request == 'H') {
    for(int i = 0; i < 10; i++)
      BT.println(i);
  }
  else if (request == 'L') {
      BT.println(4);
  }
  else if (request == 'M') {
      BT.println((unsigned char) 5);
  }
}
