int btnPin = A2;
int fetPin = 0;
int fetPin2 = 1;

    
void setup() {      

  pinMode(fetPin, OUTPUT); 
  pinMode(fetPin2, OUTPUT);  
}


void loop() {

  int a= analogRead(btnPin);
  analogWrite(fetPin,a/4);
  analogWrite(fetPin2,a/4);
}
