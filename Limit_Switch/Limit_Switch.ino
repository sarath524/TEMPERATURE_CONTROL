
const int buttonPin = 4;    // the number of the pushbutton pin
// the number of the LED pin

// Variables will change:
// the current state of the output pin
int buttonState;
int lastButtonState = HIGH;   
unsigned long lastDebounceTime = 0;  
unsigned long debounceDelay = 6000;    

void setup() {
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);

}

void loop() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState ) {
    // reset the debouncing timer
    lastDebounceTime = millis();
    Serial.println("limitswitch is started pressing");
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
      //Serial.println("lastDebounceTime");
    if (reading != buttonState) {
      buttonState = reading;


      if (buttonState == LOW) {
        Serial.println("limitswitch is pressed");
        buttonState = HIGH;
      }
    }
  }

  // set the LED:


  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}
