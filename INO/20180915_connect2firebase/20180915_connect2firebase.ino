// Using ESP8266 to connect to Firebase
//
// Library: FirebaseArduino

#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Set connection information.
#define FIREBASE_HOST "arduino-ff0bc.firebaseio.com"
#define FIREBASE_AUTH "qGXsCir7KVVbAe2H4DVwWJBgulLThlfHBiA1inqZ"
#define WIFI_SSID "505-AP"
#define WIFI_PASSWORD "mis505505"

// Set pin value
#define READ_PIN 0    // GPIO: 0,  d1 min:D3
#define D1 5          // GPIO: 5,  d1 min D1
#define D0 16         // GPIO: 16, d1 min D0

unsigned long previousTime = 0;

void setup() {
  Serial.begin(9600);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //Firebase.begin(FIREBASE_HOST);

  pinMode(READ_PIN, INPUT_PULLUP) ;

  pinMode(D1, OUTPUT) ;
  pinMode(D0, OUTPUT) ;

}

void loop() {
  //    bool switchValue = !digitalRead(READ_PIN);
  //  Serial.println(switchValue);
  setFirebaseData();
  unsigned long currentTime = millis();
  if (abs(currentTime - previousTime) > 1000) {
    previousTime = currentTime;
    getFirebaseData();
  }
}

void setFirebaseData() {
  bool switchValue = !digitalRead(READ_PIN);
  if (switchValue){
    bool replay = Firebase.getBool("iot0901/relay");
    if (Firebase.success()) {
      Firebase.setBool("iot0901/relay", !replay);
    }
  }
}

void getFirebaseData() {
  bool replay = Firebase.getBool("iot0901/relay");
  if (Firebase.success()) {
    Serial.println(replay);
    if (replay) {
      digitalWrite(D0, HIGH);
      digitalWrite(D1, HIGH);
    }
    else {
      digitalWrite(D0, LOW);
      digitalWrite(D1, LOW);
    }
  }
}
