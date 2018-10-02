/*   
  Using ESP8266 to connect to Firebase
  Step 1: read value of switch, 
          if the switch is pressed, then change the value in Firebase
  Step 2: read the Firebase value to set relay (LED) 
  
  Library: FirebaseArduino
      1. download: https://github.com/FirebaseExtended/firebase-arduino
      2. import library 
*/

#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Set connection information.
#define FIREBASE_HOST "arduino-ff0bc.firebaseio.com"
#define FIREBASE_AUTH "qGXsCir7KVVbAe2H4DVwWJBgulLThlfHBiA1inqZ"
#define WIFI_SSID "admin"
#define WIFI_PASSWORD "leochenviewaa"
//#define WIFI_SSID "Xiaomi_55-ST"
//#define WIFI_PASSWORD "12345678"

// Set pin value
#define READ_PIN 0    // GPIO: 0,  d1 min:D3
#define LedD1 5       // GPIO: 5,  d1 min D1
#define RelayD0 16    // GPIO: 16, d1 min D0
#define RelayD2 4     // GPIO: 4,  d1 min D2

#define FirebaseRelayNodePath1 "iot0901/RelayBulb"
#define FirebaseRelayNodePath2 "iot0901/RelayFan"

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
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize firebase 
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //Firebase.begin(FIREBASE_HOST);

  pinMode(READ_PIN, INPUT_PULLUP) ;
 // pinMode(LedD1, OUTPUT) ;
  pinMode(RelayD0, OUTPUT) ;
  pinMode(RelayD2, OUTPUT) ;
}

void loop() {
  setFirebaseData();
  setRelayByFirebase(FirebaseRelayNodePath1, RelayD0);
  setRelayByFirebase(FirebaseRelayNodePath2, RelayD2); 
}

// read value of switch, 
// if the switch is pressed, then change the value in Firebase
void setFirebaseData() {
  bool switchValue = !digitalRead(READ_PIN);
  if (switchValue){
    Serial.println("Switch is pressed");
    bool replay = Firebase.getBool(FirebaseRelayNodePath1);
    if (Firebase.success()) {
      Serial.println("Change relay success");
      Firebase.setBool(FirebaseRelayNodePath1, !replay);
    }
    else 
      Serial.println("Change relay failed");
  }
}

//  1. get boolean value from refNodePath
//  2. according to returned value to trun on/off the relay
void setRelayByFirebase(String refNodePath, int relayPin) {
  bool refNodeValue = Firebase.getBool(refNodePath);
  if (Firebase.success()) {
    Serial.println(refNodePath + String("value:") + refNodeValue);
    if (refNodeValue) {
      digitalWrite(relayPin, HIGH);
    }
    else {
      digitalWrite(relayPin, LOW);
    }
  }
  else {
    Serial.println("Firebase Failed");
  }
}
