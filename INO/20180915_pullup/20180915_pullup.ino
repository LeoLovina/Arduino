/*
  d1 min board
  pull-up: only D3 and D4 support pull up
  by 陳建榮

  This example code is in the public domain.
*/
#define READ_PIN 0    // GPIO: 0,  d1 min:D3
#define D1 5          // GPIO: 5,  d1 min D1

unsigned long previousTime = 0;
bool g_switchValue = false;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //  pinMode(READ_PIN, INPUT_PULLUP) ;
  pinMode(READ_PIN, INPUT);           // set pin to input
  digitalWrite(READ_PIN, HIGH);       // turn on pullup resistors

  pinMode(D1, OUTPUT) ;

}

void loop() {
  bool switchValue = !digitalRead(READ_PIN);
  if (g_switchValue != switchValue) {
    g_switchValue = switchValue;
    Serial.println("change g_switchValue --------------------------->");
  }
  Serial.println(switchValue);
  unsigned long currentTime = millis();
  if (abs(currentTime - previousTime) > 1000) {
    previousTime = currentTime;
    controlLED(switchValue);
  }
}

void controlLED(bool switchValue) {
  if (switchValue)
    digitalWrite(D1, HIGH);
  else
    digitalWrite(D1, LOW);
}
