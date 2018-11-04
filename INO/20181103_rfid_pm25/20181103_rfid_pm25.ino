/*
    1. using ESP8266 to connect to RFID reader (RC522)
    2. read RFID tag
    3. put the data to Firebase
    4. show it on LCD
    5. read value from pm2.5 sensor

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
// PM2.5
#include <SoftwareSerial.h>

// Set connection information.
#define FIREBASE_HOST "arduino-ff0bc.firebaseio.com"
#define FIREBASE_AUTH "qGXsCir7KVVbAe2H4DVwWJBgulLThlfHBiA1inqZ"
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
#define FirebaseDustPM25 "iot0901/DustPM25"
#define FirebaseDustPM25S "iot0901/DustPM25S"

unsigned long previousTime = 0;
bool currentLedValue = false;

// RFID setting
#define SS_PIN  D8
#define RST_PIN D0
MFRC522 mfrc522(SS_PIN, RST_PIN);  // 建立MFRC522物件

// LCD
#define D1 5
#define D2 4
LiquidCrystal_I2C lcd(0x27, D1, D2);

// PM2.5
#define rxPin 10
#define txPin 0 //11
// Use software serial port for communicating with GP2Y1026
SoftwareSerial mySerial(rxPin, txPin);
// Helper function to read data from software serial port
int getSerial(){
  while (!mySerial.available()){}
  int value = mySerial.read();
  Serial.println(String("getSerial value=") + value);
  return mySerial.read();
}


StaticJsonBuffer<200> jsonBuffer;

void setup() {
  initLCD();
  
  Serial.begin(9600);
  Serial.println();
  initialWifi();
  initialFirebase();
  initRFID();
  
  // initialize pin
  //pinMode(READ_PIND3, INPUT);
  pinMode(PIND4, OUTPUT);

  // set up PM2.5
  mySerial.begin(2400);
  // Wait a second for startup   
  delay(1000);
  Serial.println("PM2.5 is ready!!");
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
  unsigned long currentTime = millis();
  if (abs(currentTime - previousTime) > 1000) {
    previousTime = currentTime;
   // DustPM25();
  }
}

void DustPM25(){
  static int frame[7];
  static int dataStart, VoutH, VoutL, VrefH, VrefL, checksum, dataEnd;
  // Look for end of frame
  if (getSerial() != 0xff){
    return;
  }
  //read in next frame
  for (int i=0; i<7; i++){
    frame[i] = getSerial();
  }

  // Assgn values
  dataStart = frame[0];
  VoutH = frame[1];
  VoutL = frame[2];
  VrefH = frame[3];
  VrefL = frame[4];
  checksum = frame[5];
  dataEnd = frame[6];

  // Check the start and end frame
  if(dataStart != 0xaa || dataEnd != 0xff){
    Serial.println("Check the start and end frame is failed");
    return ;
  }
        
  // Verify the checksum 
  int testSum = VoutH + VoutL + VrefH + VrefL;
  if (testSum != checksum){
    Serial.println("Verify the checksum  is failed");
    return;
  }
  else 
    Serial.println(String("Verify the checksum") + testSum);
  
  return;
  
  //print the data values
  Serial.print("dataStart=");
  Serial.print(dataStart);
  Serial.print(",");
  Serial.print("VoutH=");
  Serial.print(VoutH);
  Serial.print(",");
  Serial.print("VoutL=");
  Serial.print(VoutL);
  Serial.print(",");
  Serial.print("VrefH=");
  Serial.print(VrefH);
  Serial.print(",");
  Serial.print("VrefL=");
  Serial.print(VrefL);
  Serial.print(",");
  Serial.print("Checksum=");
  Serial.print(checksum);
  Serial.print(",");
  Serial.print("dataEnd=");
  Serial.print(dataEnd);

  // Calculate Vout
  float Vout = (VoutH * 256 + VoutL) / 1024.0 * 5.0;
  Serial.print("Vout=");
  Serial.print(Vout * 1000.0);
  Serial.print("mV,");
  // Calculate "a" coefficient before hand by using a reference sucr monitor
  float a = 100/0.14;
  // Calculate dust density 
  float dustDensity = a * Vout;
  Serial.print("PM2.5=");
  Serial.print(dustDensity);
  Serial.print("ug/m3");
  Serial.println("");
  
 


  
  

  
//  int samplingTime = 280;
//  int deltaTime = 40;
//  int sleepTime = 9680;
//  // Connect dust sensor, read pm2.5
//  float voMeasured = 0;
//  float calcVoltage = 0;
//  float dustDensity = 0;
//
// // digitalWrite(PIND4,LOW); // power on the LED
//  delayMicroseconds(samplingTime);
//  
//  voMeasured = analogRead(A0); // read the dust value
//  
//  delayMicroseconds(deltaTime);
// // digitalWrite(PIND4,HIGH); // turn the LED off
//  //delayMicroseconds(sleepTime);
//  
//  // 0 - 5V mapped to 0 - 1023 integer values
//  // recover voltage
//  calcVoltage = voMeasured * (5.0 / 1024.0);
//  
//  // linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
//  // Chris Nafis (c) 2012
//  dustDensity = 0.17 * calcVoltage - 0.1;
//  
//  Serial.print("Raw Signal Value (0-1023): ");
//  Serial.print(voMeasured);
//  
//  Serial.print(" - Voltage: ");
//  Serial.print(calcVoltage);
//  
//  Serial.print(" - Dust Density: ");
//  Serial.print(dustDensity * 1000); // 這裡將數值呈現改成較常用的單位( ug/m3 )
//  Serial.println(" ug/m3 ");
//
//  float value = dustDensity * 1000;
//  // send to firebase 
//  Firebase.setFloat(FirebaseDustPM25, value);
//
//  jsonBuffer.clear();
//  // prepare send data to firebase
//  JsonObject& cardObject = jsonBuffer.createObject();
//  cardObject["value"] = value;
//  JsonObject& timeStampObject = jsonBuffer.createObject();
//  timeStampObject[".sv"] = "timestamp";
//  cardObject["timeStamp"] = timeStampObject;
//  Firebase.push(FirebaseDustPM25S, cardObject);  
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
    
 //   processTag(id, idSize);
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
    //create inRecords
    jsonBuffer.clear(); //need clear butter
    JsonObject& timeObject = jsonBuffer.createObject();
    timeObject[".sv"] = "timestamp";
    Firebase.push(String("")+FirebaseInRecords+"/"+cardID,timeObject);
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
