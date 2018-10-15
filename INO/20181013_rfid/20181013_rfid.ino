/*   
    1. using ESP8266 to connect to RFID reader (RC522)
    2. read RFID tag
    3. put the data to Firebase
  
  Depends on the following Arduino libraries:
      FirebaseArduino: https://github.com/FirebaseExtended/firebase-arduino
      MFRC522 library

  Note:       
    how does it work
    step 1: make a sound that valumne is greater then 250 
    step 2: change the LED, read Humidity, Temperature and MagneticSring
*/
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
// RFID library
#include <SPI.h>
#include <MFRC522.h>

// Set connection information.
#define FIREBASE_HOST "arduino-ff0bc.firebaseio.com"
#define FIREBASE_AUTH "qGXsCir7KVVbAe2H4DVwWJBgulLThlfHBiA1inqZ"
#define WIFI_SSID "admin"
#define WIFI_PASSWORD "leochenviewaa"
//#define WIFI_SSID "IOT505"
//#define WIFI_PASSWORD "12345678"
//#define WIFI_SSID "505-AP"
//#define WIFI_PASSWORD "mis505505"

// Set pin value
#define READ_PIND3  0   // GPIO: 0,  d1 min:D3
#define READ_PIND4  2   // GPIO: 2,  d1 min:D4
#define LedD1       5   // GPIO: 5,  d1 min D1
#define RelayD2     4   // GPIO: 4,  d1 min D2

#define FirebaseRFID "iot0901/RFID"
#define FirebaseWindow "iot0901/Window"

unsigned long previousTime = 0;
bool currentLedValue = false;

// RFID setting
#define SS_PIN  D8   
#define RST_PIN D0  
#define MAX_PICC_Count 2
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key; 
// Init array that will store new NUID 
byte nuidPICC[MAX_PICC_Count][4];            // define a 4 X MAX_PICC_Count (XxY) array
byte nuidPICCIndex =0;
//char users[MAX_PICC_Count][6]={"Peter","Amy","Kelly"};
String users[MAX_PICC_Count]={"Peter","Amy"};

void setup() {
  Serial.begin(9600);
  Serial.println();
  initialWifi();
  initialFirebase();

  // initialize pin
  pinMode(READ_PIND3, INPUT_PULLUP); 
  pinMode(READ_PIND4, INPUT_PULLUP); 
  pinMode(LedD1, OUTPUT) ;
  pinMode(RelayD2, OUTPUT) ;

  // RDIF
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void initialWifi(){
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
}

void initialFirebase(){
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
//Firebase.begin(FIREBASE_HOST);
}

void loop() {
  readRFIDTag();
  
//  readMagneticSring();
//  readSwitch();
    
//  int soundValue = analogRead(A0);
//  if (soundValue >250){
//    Serial.print("Sound:");
//    Serial.println(soundValue);
//    unsigned long currentTime = millis();
//    if (abs(currentTime - previousTime) > 1000) {
//      previousTime = currentTime;
//      setLEDStatus();
//     // readMagneticSring();
//    }
//  }  
}

void readRFIDTag(){
  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }
  
  // Check if the tag is existed
  byte found = 0;
  for(int i=0; i<MAX_PICC_Count; i++){
    if (rfid.uid.uidByte[0] == nuidPICC[i][0] && rfid.uid.uidByte[1] == nuidPICC[i][1] && rfid.uid.uidByte[2] == nuidPICC[i][2] && rfid.uid.uidByte[3] == nuidPICC[i][3]){
      found = 1;
      Serial.println(String("Tag ") + users[i] + " present");
      Firebase.setString(FirebaseRFID, users[i]);
      if (Firebase.failed()) {
        Serial.print("readMagneticSring setting /number failed:");
        Serial.println(Firebase.error());  
      }  
    }
  }

  if (!found){
    if (nuidPICCIndex == MAX_PICC_Count) {
      Serial.println(F("nuidPICC is full"));  
      Firebase.setString(FirebaseRFID, "nuidPICC is full");
    }
    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[nuidPICCIndex][i] = rfid.uid.uidByte[i];
    } 

    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println(String("--- assigned to ") + users[nuidPICCIndex]);
    
    nuidPICCIndex ++;   
  }

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();  
}

/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

void readMagneticSring(){
  bool switchValue = !digitalRead(READ_PIND4);
  if (!switchValue){
    Serial.println("************Magnetic spring value is opened");  
    Firebase.setString(FirebaseWindow, "Open");
  }
  else {
    Serial.println("************Magnetic spring value is closed");  
    Firebase.setString(FirebaseWindow, "Close");
  }
  if (Firebase.failed()) {
    Serial.print("readMagneticSring setting /number failed:");
    Serial.println(Firebase.error());  
  }  
}

// setup pullup on READ_PIND3 
void readSwitch(){
  bool switchValue = !digitalRead(READ_PIND3);
  if (switchValue){
    Serial.println("************Switch is pressed");  
//    Firebase.setString(FirebaseWindow, "Open");
  }
  else {
//    Firebase.setString(FirebaseWindow, "Close");
  }
}
