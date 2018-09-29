/*
  d1 min board
    1. 利用 D3 讀取 switch (設定為 Input_Pullup)
    2. 使用 ESP8266WiFi 程式庫 連線 wifi
  wifi
  by 陳建榮

  This example code is in the public domain.
*/
#include <ESP8266WiFi.h>

#define READ_PIN 0    // GPIO: 0,  d1 min:D3
#define D1 5          // GPIO: 5,  d1 min D1
#define D0 16         // GPIO: 16, d1 min D0

unsigned long previousTime = 0;
void setup() {
  // initialize wifi
  Serial.begin(115200);
  Serial.println();
  WiFi.begin("505-AP", "mis505505");
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(READ_PIN, INPUT_PULLUP) ;
  pinMode(D1, OUTPUT) ;
  pinMode(D0, OUTPUT) ;

}

void loop() {
  bool switchValue = !digitalRead(READ_PIN);
//  Serial.println(switchValue);
  unsigned long currentTime = millis();
  if (abs(currentTime - previousTime) > 1000) {
    previousTime = currentTime;
    controlLED(switchValue);
  }
}

void controlLED(bool switchValue) {
  if (switchValue) {
    digitalWrite(D0, HIGH);
    digitalWrite(D1, HIGH);
  }
  else {
    digitalWrite(D0, LOW);
    digitalWrite(D1, LOW);
  }
}
