#define D1 5

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(D1, OUTPUT) ;
  for(int i =0; i< 3 ;i++){
    digitalWrite(D1, HIGH);       // sets the digital pin 13 on
    delay(1000);                  // waits for a second
    digitalWrite(D1, LOW);        // sets the digital pin 13 off
    delay(1000);                  // waits for a second
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()){
    int incoming = Serial.read();
    Serial.println(incoming);
    if (incoming == '1')
      digitalWrite(D1, HIGH);
    else         
      digitalWrite(D1, LOW);
  }
  
//  Serial.println("Hello World");
//  delay(1000);
}
