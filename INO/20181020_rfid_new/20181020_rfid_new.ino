/*
    1. using ESP8266 to connect to RFID reader (RC522)
    2. read RFID tag
    3. put the data to Firebase
    4. show it on LCD

  Depends on the following Arduino libraries:
      1. FirebaseArduino: https://github.com/FirebaseExtended/firebase-arduino
      2. MFRC522 library
      3. LiquidCrystal-I2C.zip
  Reference
    https://swf.com.tw/?p=930
*/
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
// RFID library
#include <SPI.h>
#include <MFRC522.h>
// LCD library
#include <LiquidCrystal_I2C.h>

// Set connection information.
#define FIREBASE_HOST "arduino-ff0bc.firebaseio.com"
#define FIREBASE_AUTH "qGXsCir7KVVbAe2H4DVwWJBgulLThlfHBiA1inqZ"
//#define WIFI_SSID "admin"
//#define WIFI_PASSWORD "leochenviewaa"
//#define WIFI_SSID "IOT505"
//#define WIFI_PASSWORD "12345678"
#define WIFI_SSID "505-AP"
#define WIFI_PASSWORD "mis505505"

// Set pin value
#define READ_PIND3  0   // GPIO: 0,  d1 min:D3
#define READ_PIND4  2   // GPIO: 2,  d1 min:D4

#define PIND4       2   // GPIO: 2,  d1 min:D4

#define FirebaseRFID "iot0901/RFID"
#define FirebaseRecord "iot0901/Records"
#define FirebaseInRecords "iot0901/InRecords"
#define FirebaseWindow "iot0901/Window"

unsigned long previousTime = 0;
bool currentLedValue = false;

// RFID setting
#define SS_PIN  D8
#define RST_PIN D0
#define MAX_PICC_Count 2
MFRC522 mfrc522(SS_PIN, RST_PIN);  // 建立MFRC522物件
// Init array that will store new NUID
byte nuidPICC[MAX_PICC_Count][4];            // define a 4 X MAX_PICC_Count (XxY) array
byte nuidPICCIndex = 0;
//char users[MAX_PICC_Count][6]={"Peter","Amy","Kelly"};
String users[MAX_PICC_Count] = {"Peter", "Amy"};

// LCD
#define D1 5
#define D2 4
LiquidCrystal_I2C lcd(0x27, D1, D2);

StaticJsonBuffer<200> jsonBuffer;

void setup() {
  initLCD();
  
  Serial.begin(9600);
  Serial.println();
  initialWifi();
  initialFirebase();
  initRFID();

  // initialize pin
  //pinMode(READ_PIND3, INPUT_PULLUP);
  //  pinMode(READ_PIND4, INPUT_PULLUP);
  pinMode(PIND4, OUTPUT);
//  pinMode(LedD1, OUTPUT) ;
//  pinMode(PWMD2, OUTPUT) ;
}

void initLCD(){
  //LCD
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(5, 0);
  lcd.print("HELLO");
  lcd.setCursor(5, 1);
  lcd.print("WORLD");
}

void initRFID() {
  SPI.begin();
  mfrc522.PCD_Init();   // 初始化MFRC522讀卡機模組
}

void initialWifi() {
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

void initialFirebase() {
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //Firebase.begin(FIREBASE_HOST);
}

void loop() {
  readRFIDTag();
//  outputPWM();
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

// https://circuits4you.com/2017/12/21/esp8266-pwm-example/
// ESP8266 can generate PWM on all IO pins. The ESP8266 analogWrite is different than the Arduino Uno. 
// ESP8266 uses 10-bit resolution for PWM generation PWM value varries from 0 to 1023. 
// Arduino Uses 8-Bit Resolution i.e.PWM range is 0-254.
void outputPWM() {
  digitalWrite(PIND4,HIGH);
  delay(250);
  analogWrite(PIND4,LOW);

//  tone(PIND4,1000,1000);
//  delay(1000);   
  
  //PWM Value varries from 0 to 1023  
//  Serial.println("10 % PWM");
//  analogWrite(PIND4,102);
//  delay(2000);
// 
//  Serial.println("20 % PWM");
//  analogWrite(PIND4,205);
//  delay(2000);
// 
//  Serial.println("40 % PWM");
//  analogWrite(PIND4,410);
//  delay(2000);
// 
//  Serial.println("70 % PWM");
//  analogWrite(PIND4,714);
//  delay(1000);
// 
//  Serial.println("100 % PWM");
//  analogWrite(PIND4,1024);
//  delay(2000);
}

void readRFIDTag() {
  // 確認是否有新卡片
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    byte *id = mfrc522.uid.uidByte;   // 取得卡片的UID
    byte idSize = mfrc522.uid.size;   // 取得UID的長度

    Serial.print("PICC type: ");      // 顯示卡片類型
    // 根據卡片回應的SAK值（mfrc522.uid.sak）判斷卡片類型
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    Serial.print("UID Size: ");       // 顯示卡片的UID長度值
    Serial.println(idSize);
    lcd.clear();
    String message1 = "SUCCESS";
    String cardID = "";
    for (byte i = 0; i < idSize; i++) {  // 逐一顯示UID碼
      Serial.print("id[");
      Serial.print(i);
      Serial.print("]: ");
      Serial.println(id[i], HEX);       // 以16進位顯示UID值
      cardID += String(id[i], HEX) + "_";
    }
    Serial.println();

    //lcd.setCursor(0, 0);
    int count = cardID.length();
    lcd.setCursor((16 / 2) - (count / 2), 0);
    lcd.print(message1);

    count = cardID.length();
    lcd.setCursor((16 / 2) - (count / 2), 1);
    lcd.print(cardID);
    
    processTag(id, idSize);

    mfrc522.PICC_HaltA();  // 讓卡片進入停止模式

    outputPWM();
    jsonBuffer.clear();
    // prepare send data to firebase
    JsonObject& cardObject = jsonBuffer.createObject();
    cardObject["casrdID"] = cardID;
    JsonObject& timeStampObject = jsonBuffer.createObject();
    timeStampObject[".sv"] = "timestamp";
    cardObject["timeStamp"] = timeStampObject;
    Firebase.push(FirebaseRecord, cardObject);

//    String rfidCardPath = String("") + FirebaseInRecords + "/" + cardID;
//    Firebase.setString(rfidCardPath, "bee");

    //create inRecords
    jsonBuffer.clear(); //need clear butter
    JsonObject& timeObject = jsonBuffer.createObject();
    timeObject[".sv"] = "timestamp";
    Firebase.push(String("")+FirebaseInRecords+"/"+cardID,timeObject);
  }
}

//
void processTag(byte *uidByte, byte idSize) {
  // Check if the tag is existed
  byte found = 0;
  for (int i = 0; i < MAX_PICC_Count; i++) {
    if (uidByte[0] == nuidPICC[i][0] && uidByte[1] == nuidPICC[i][1] && uidByte[2] == nuidPICC[i][2] && uidByte[3] == nuidPICC[i][3]) {
      found = 1;
      Serial.println(String("Tag ") + users[i] + " present");
      Firebase.setString(FirebaseRFID, users[i]);
    }
  }

  if (!found) {
    if (nuidPICCIndex == MAX_PICC_Count) {
      Serial.println(F("nuidPICC is full"));
      Firebase.setString(FirebaseRFID, "nuidPICC is full");
    }
    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[nuidPICCIndex][i] = uidByte[i];
    }

    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(uidByte, idSize);
    Serial.println(String("--- assigned to ") + users[nuidPICCIndex]);

    nuidPICCIndex ++;
  }
}

/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
   Helper routine to dump a byte array as dec values to Serial.
*/
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

void readMagneticSring() {
  bool switchValue = !digitalRead(READ_PIND4);
  if (!switchValue) {
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
void readSwitch() {
  bool switchValue = !digitalRead(READ_PIND3);
  if (switchValue) {
    Serial.println("************Switch is pressed");
    //    Firebase.setString(FirebaseWindow, "Open");
  }
  else {
    //    Firebase.setString(FirebaseWindow, "Close");
  }
}
