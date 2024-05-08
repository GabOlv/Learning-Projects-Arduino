/*
    Initialize the lists with the pins and buttons positions
    And get the number of buttons
*/
const int ledPins[] = {4, 6, 8, 10};
const int buttonPins[] = {5, 7, 9, 11};
const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

/*
    Setup will initialize the pins and set their states.
*/
void setup() {
  // Initialize LED and button pins
  for (int i = 0; i < numButtons; i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT);
  }

  // Sequentially turn on and off LEDs
  sequentialLeds();
}

/*
    Main loop will verify if the button is pressed or not.
    If the button is pressed, the index of the button will be given as an attribute to the function activateLed
    and the delay will be applied to prevent debouncing.
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
    Activate the LEDs based on the button and LED index
    If the indices are not the same (LedPins[i] != buttonIndex), the LED will be turned off.
*/
void activateLed(int buttonIndex) {
  for (int i = 0; i < numButtons; i++) {
    digitalWrite(ledPins[i], i == buttonIndex ? HIGH : LOW);
  }
}

/*
    Sequentially turn on and off the LEDs
*/
void sequentialLeds() {
    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < numButtons; i++) {
            digitalWrite(ledPins[i], HIGH); // Turn on current LED
            delay(150); // Wait 100ms
            digitalWrite(ledPins[i], LOW); // Turn off current LED
            delay(150); // Wait 100ms before turning on next LED
        }
    }

    // Turn on all LEDs for 3 seconds
    for (int i = 0; i < numButtons; i++) {
        digitalWrite(ledPins[i], HIGH);
    }
    delay(2000); // Wait 2 seconds
    turnOffAllLeds(); // Turn off all LEDs
}

/*
    Turn off all LEDs
*/
void turnOffAllLeds() {
    for (int i = 0; i < numButtons; i++) {
        digitalWrite(ledPins[i], LOW);
    }
}