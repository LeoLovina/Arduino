/*
  霹靂燈(左右位移雨滴燈)
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
    
// the setup function runs once when you press reset or power the board
void setup() {
  for (int i =0; i < 4; i++)
    pinMode(g_outputPinNumber[i], OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  on_off_led(g_brightnessIndex, g_direction);
  g_brightnessIndex ++;
  if (g_brightnessIndex>=7){
    g_brightnessIndex = 0;  
    g_direction = !g_direction;
  }
}

// control brightness of LED
void on_off_led(int brightnessIndex, bool direct){
  for (int i =0; i< 4 ; i++){
    analogWrite(g_outputPinNumber[i],g_brightness[brightnessIndex][abs(3*direct-i)]);  
  }
  delay(100);   
}

