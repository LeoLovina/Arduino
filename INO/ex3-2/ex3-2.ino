/*
 將Arduino Serial串列所接收到的 R/L控制四顆LED的位移方向 與 0~9控制位移速度
 
 陳建榮 
 2018/08/18
*/
const byte g_outputPinNumber[]= {10,11,12,13};
bool g_direction = false;
byte g_ledIndex = 0;
char g_val=0;
int g_speed = 1000;

void setup() 
{  
  // initialize pin mode
  for (int i =0; i < 4; i++)
  pinMode(g_outputPinNumber[i], OUTPUT);
                
  Serial.begin(9600);
  Serial.println("--- Start Serial Monitor SEND_RCVE ---");
}

void loop() 
{
  if (Serial.available()){
    g_val = Serial.read();
    Serial.print(String("Received char: ")+ g_val +" (") ;
    Serial.print(g_val, DEC) ;
    Serial.println(")");
  }
  switch(g_val){
    case 'l':
    case 'L':
      g_direction = true;
      break;
    case 'r':
    case 'R':
      g_direction = false;
      break;
    case '1' ... '9':
        g_speed = (g_val - '0') * 100;
        break;
    case '0':
        g_speed  = 1000;
        break;  
  }
  on_off_led(g_ledIndex, g_direction, g_speed);
  g_ledIndex ++;
  if (g_ledIndex>3)
    g_ledIndex = 0;
}

// on/off LED
void on_off_led(int index, bool direct, int speed){
  index = abs(3*direct-index);
  Serial.println(String("== index:")+ index + " direct:"+direct + " speed: "+ speed);
  for (int i =0; i< 4 ; i++){
    bool on = (i == index);
    Serial.println(String("pin ") + i + ":"+ on );
    digitalWrite(g_outputPinNumber[i],on);  
  }
  delay(speed);   
}



