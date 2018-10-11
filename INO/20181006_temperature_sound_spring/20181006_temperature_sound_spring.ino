/*   
  Using ESP8266 (Humidity, Temperature, Sound-LED), to connect to Firebase
  read value of Humidity, Temperature and Sound, then send the values to Firebase
  
  Depends on the following Arduino libraries:
      1. FirebaseArduino: https://github.com/FirebaseExtended/firebase-arduino
      2. Adafruit Unified Sensor Library: https://github.com/adafruit/Adafruit_Sensor
      3. DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library      

  Note:       
    how does it work
    step 1: make a sound that valumne is greater then 250 
    step 2: change the LED, read Humidity, Temperature and MagneticSring
*/
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// include DHT11 (Humidity, Temperature)
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Set connection information.
#define FIREBASE_HOST "arduino-ff0bc.firebaseio.com"
#define FIREBASE_AUTH "qGXsCir7KVVbAe2H4DVwWJBgulLThlfHBiA1inqZ"
#define WIFI_SSID "admin"
#define WIFI_PASSWORD "leochenviewaa"
//#define WIFI_SSID "Xiaomi_55-ST"
//#define WIFI_PASSWORD "12345678"

// Set pin value
#define READ_PIND3 0    // GPIO: 0,  d1 min:D3
#define READ_PIND4 2  // GPIO: 2,  d1 min:D4
#define LedD1 5       // GPIO: 5,  d1 min D1
#define RelayD2 4     // GPIO: 4,  d1 min D2

#define DHTPIN            D0        // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT11     // DHT 11 
#define soundSensor       A0

#define FirebaseTemperature "iot0901/Temperature"
#define FirebaseHumidity "iot0901/Humidity"
#define FirebaseSound_LED "iot0901/Sound_LED"
#define FirebaseWindow "iot0901/Window"

// initialize DHT
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
unsigned long previousTime = 0;
bool currentLedValue = false;

void setup() {
  Serial.begin(115200);
  Serial.println();
  initialWifi();
  initialFirebase();
  initialTemperatureHumidity();

  // initialize pin
//  pinMode(READ_PIN, INPUT);           // set pin to input
//  digitalWrite(READ_PIN, HIGH);       // turn on pullup resistors
  pinMode(READ_PIND3, INPUT_PULLUP); 
  pinMode(READ_PIND4, INPUT_PULLUP); 
  pinMode(LedD1, OUTPUT) ;
  pinMode(RelayD2, OUTPUT) ;
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

void initialTemperatureHumidity() {
  // Initialize device.
  dht.begin();
  Serial.println("DHTxx Unified Sensor Example");
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");  
  Serial.println("------------------------------------");
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");  
  Serial.println("------------------------------------");
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
}


void loop() {
//  readSwitch();
    
  int soundValue = analogRead(A0);
  if (soundValue >250){
    Serial.print("Sound:");
    Serial.println(soundValue);
    unsigned long currentTime = millis();
    if (abs(currentTime - previousTime) > 1000) {
      previousTime = currentTime;
      readTemperatureHumidity();
      setLEDStatus();
      readMagneticSring();
    }
  }  
}

void setLEDStatus(){
    currentLedValue = !currentLedValue;
    digitalWrite(RelayD2, currentLedValue);
    Serial.println(String("Relay Change.....") + currentLedValue);
    Firebase.setBool(FirebaseSound_LED, currentLedValue);
}

void readTemperatureHumidity(){  
  // Delay between measurements.
  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    Serial.println(" *C");
    Firebase.setFloat(FirebaseTemperature, event.temperature);
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    Serial.print("Humidity: ");
    Serial.print(event.relative_humidity);
    Serial.println("%");
    Firebase.setFloat(FirebaseHumidity, event.relative_humidity);
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
