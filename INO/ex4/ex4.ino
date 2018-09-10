/*
 * 使用一四位七段顯示器與一按鍵開關, 
 * 按一下使其正數，按第二下使其倒數
 * 按第三下停止並閃爍(亮0.5秒、暗0.5秒)
 * 
 * 陳建榮
 * 20180819
 */

int count=0;
unsigned long time=0;
const byte num[10]={
    B11000000,  //0
    B11111001,  //1
    B10100100,  //2
    B10110000,  //3
    B10011001,  //4
    B10010010,  //5
    B10000010,  //6
    B11111000,  //7    
    B10000000,  //8
    B10010000}; //9    
const int seg[]={2,3,4,5,6,7,8,9};  //abcdefgp
const int digit[]={10,11,12,13};    //D0-D3
const int g_swPin=14;          
const byte g_debouncedelay = 150;
int  g_swStatus = 0;                // 0: 正數, 1:倒數, 2:停止並閃爍

void setup() 
{ 
  Serial.begin(9600);
  Serial.println("--- Start Serial Monitor  ---");
    
  // 第14 腳 開啟該數位接腳的內部20k上拉電阻 
  pinMode(g_swPin, INPUT);
  digitalWrite(g_swPin, HIGH);
  for(int i=0;i<8;i++)
    pinMode(seg[i],OUTPUT);
  for(int i=0;i<4;i++)
  {
    pinMode(digit[i],OUTPUT); 
    digitalWrite(digit[i],HIGH);
  }       
  
}
void loop() 
{
    changeStatus();
    int number=count;
    for(int i=3;i>=0;i--)
    {
      showNumber(number);
      if (millis()-time<500 || g_swStatus!=2)
      {
        digitalWrite(digit[i],LOW);      
        delay(5);
        digitalWrite(digit[i],HIGH);          
        number=number/10;
      }
      
      // turn off all LED
      if(millis()-time>=500 && g_swStatus==2)
        digitalWrite(digit[i],HIGH);          
        
      if(millis()-time>=1000)
      {
        Serial.println(String("count: ")+ count);
        time=millis();
        if (g_swStatus==0 || g_swStatus==1)
        {
          count=count+1-g_swStatus*2;
          if(count>9999)
            count=0;
          else if (count<0)
            count =9999;
        }
      }         
    }
}  

void showNumber(int number)
{
  for(int j=0;j<8;j++)
  {  
    if(bitRead(num[number%10],j))
      digitalWrite(seg[j],HIGH);
    else
      digitalWrite(seg[j],LOW);        
  }
}

void changeStatus()
{
  int val = digitalRead(g_swPin);
  if (val == LOW){
    delay (g_debouncedelay);
    if (digitalRead(g_swPin) == LOW);
    g_swStatus ++;
    if (g_swStatus>2)
      g_swStatus = 0;
    Serial.println(String("switch status: ")+ g_swStatus);
  }
}

