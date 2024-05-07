
/*
    Initialize the lists with the pins and buttons positions
    And get the number of buttons
    
*/
const int ledPins[] = {4, 6, 7, 8};
const int buttonPins[] = {10, 11, 12, 13};
const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

/*
    Setup will initialize the pins and set their states.
*/
void setup() {
  for (int i = 0; i < numButtons; i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT);
  }
}

/*
    Main loop will verify if the button is pressed or not.
    If the button is pressed, the index of the button will be given as atribute to the function activateLed.
    and the delay will be applied to prevent deboucing.
*/
void loop() {
  for (int i = 0; i < numButtons; i++) {
    if (digitalRead(buttonPins[i]) == HIGH) {
      activateLed(i);
      delay(100);
    }
  }
}

/*
    Activate the Leds based on the button and LED index
    If the index are not the same LedPins[i] = 2 and buttonIndex[i] = 3, the Led will be turned off.   
*/
void activateLed(int buttonIndex) {
  for (int i = 0; i < numButtons; i++) {
    digitalWrite(ledPins[i], i == buttonIndex ? HIGH : LOW);
  }
}
