/*
 將Arduino Serial串列所接收到的一組二位數以上的數字(字元)轉換成一個整數
 
 陳建榮 
 2018/08/18
*/
int result = 0;
void setup() 
{                
  Serial.begin(9600);
}

void loop() 
{
  if (Serial.available()){
    char val = Serial.read();
    byte digital = val - '0';
    if (digital>10 || digital <0) {
      result = 0;
      Serial.println("Unrecognized number. reset result to 0");
    }
    else {
      result = result * 10 + val - '0';
      Serial.print(String("Received char: ")+ val +" (") ;
      Serial.print(val, DEC) ;
      Serial.println(")");
      Serial.print("Current result: ");
      Serial.println(result);
      Serial.println("==================");
    }
  }
}
