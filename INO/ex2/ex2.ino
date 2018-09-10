/*
  按鍵開關改變LED燈(也可以是霹靂燈)左右位移
  by 陳建榮

  This example code is in the public domain.
*/

const byte g_outputPinNumber[]= { 3,5,6,9};
const int g_brightness[7][4]=
  {{250,  0,  0,  0},      
   {100,250,  0,  0},
   { 50,100,250,  0},
   {  5, 50,100,250},
   {  0,  5, 50,100},
   {  0,  0,  5, 50},
   {  0,  0,  0,  5}  };
byte g_brightnessIndex = 0; 
bool g_direction = true;
byte g_debouncedelay = 100;
byte g_inputPin = 2;
    
// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  Serial.println("--- Start Serial Monitor  ---");
    
  // 第2 腳 開啟該數位接腳的內部20k上拉電阻
  pinMode(g_inputPin, INPUT);
  digitalWrite(g_inputPin, HIGH);
  
  // initialize output pins
  for (int i =0; i < 4; i++)
    pinMode(g_outputPinNumber[i], OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  int val = digitalRead(g_inputPin);
  Serial.println(String("value:")+ val);
  if (val == LOW){
      delay (g_debouncedelay);
      if (digitalRead(g_inputPin) == LOW);
         g_direction = !g_direction;
  }
  on_off_led(g_brightnessIndex, g_direction);
  g_brightnessIndex ++;
  if (g_brightnessIndex>=7){
    g_brightnessIndex = 0;  
  }
}

// control brightness of LED
void on_off_led(int brightnessIndex, bool direct){
  for (int i =0; i< 4 ; i++){
    analogWrite(g_outputPinNumber[i],g_brightness[brightnessIndex][abs(3*direct-i)]);  
  }
  delay(100);   
}

