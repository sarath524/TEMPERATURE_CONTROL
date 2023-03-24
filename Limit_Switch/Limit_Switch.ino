
const int LIMIT_SWITCH = 35;    // the number of the pushbutton pin
// the number of the LED pin

// Variables will change:
// the current state of the output pin
int buzzerPin = 4;

int buttonState;
int lastButtonState = HIGH;   
unsigned long lastDebounceTime = 0;  
unsigned long debounceDelay = 6000;    

void setup() {
  pinMode(LIMIT_SWITCH, INPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
digitalWrite(buzzerPin,LOW);
}

void loop() {
  // read the state of the switch into a local variable:
 int reading = digitalRead(LIMIT_SWITCH);
  if (reading != lastButtonState ) 
  {
    // reset the debouncing 
          digitalWrite(buzzerPin,LOW);
          noTone(buzzerPin);
    Serial.println("limitswitch is stopped");
    
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) 
  {
    if (reading != buttonState)
    {
      buttonState = reading;
     }
      if (buttonState == LOW) {
        Serial.println("limitswitch is pressed");
        digitalWrite(buzzerPin,HIGH);
        tone(buzzerPin,1000);
        buttonState = HIGH;
      }
    
         }
    
    lastButtonState = reading;
  }

  // set the LED:


  // save the reading. Next time through the loop, it'll be the lastButtonState:
  
