
#define RED_LED 19
#define GREEN_LED 23
#define BLUE_LED 18

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(RED_LED, HIGH); 
  digitalWrite(GREEN_LED, HIGH);// turn the LED on (HIGH is the voltage level)                    // wait for a second
  digitalWrite(BLUE_LED, LOW); // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH); // turn the LED on (HIGH is the voltage level                    // wait for a second
  digitalWrite(BLUE_LED, HIGH);// turn the LED off by making the voltage LOW
  delay(1000);     
   digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);// turn the LED on (HIGH is the voltage level                    // wait for a second
  digitalWrite(BLUE_LED,HIGH);
  delay(1000);        
   analogWrite(RED_LED,0 ); 
  analogWrite(GREEN_LED,50 );// turn the LED on (HIGH is the voltage level)                    // wait for a second
  analogWrite(BLUE_LED,255 );
  delay(1000);   
  // wait for a second
}
