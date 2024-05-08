/*
    Initialize the lists with the pins and buttons positions
    And get the number of buttons
*/
const int ledPins[] = {4, 6, 8, 10};
const int buttonPins[] = {5, 7, 9, 11};
const int startButtonPin = 12;
const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

// Global variable to store the order of LED blinking
int orderLedBlink[4]; // Array to store the order of LED blinking

// Global variable to store the order of next 4 buttons pressed
int nextButtonOrder[4]; // Array to store the order of the next 4 buttons pressed

// Flag to indicate whether to count the next four button presses after button 12
bool countingButtons = false;

/*
    Setup will initialize the pins and set their states.
*/

void setup() {
    // Set up the serial monitor
    Serial.begin(9600);
    // Initialize LED and button pins
    for (int i = 0; i < numButtons; i++) {
        pinMode(ledPins[i], OUTPUT);
        pinMode(buttonPins[i], INPUT);
    }

    // Initialize Random Sequence Blink Button
    pinMode(startButtonPin, INPUT);
  
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
          
            // Store the order of the next 4 buttons pressed after pressing the start button
            if (countingButtons) {
                storeNextButtonOrder(buttonPins[i]);
            }
        }
    }

    // Check if the select button is pressed
    if (digitalRead(startButtonPin) == HIGH) {
        selectRandomLedToBlink();
        countingButtons = true; // Start counting buttons after LED blinking
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
            digitalWrite(ledPins[i], HIGH); 
            delay(150);
            digitalWrite(ledPins[i], LOW); 
            delay(150);
        }
    }

    // Turn on all LEDs for 3 seconds
    turnOnAllLeds();
    delay(1000);
    turnOffAllLeds(); // Turn off all LEDs
}

/*
    Turn On all LEDs
*/
void turnOnAllLeds() {
    for (int i = 0; i < numButtons; i++) {
        digitalWrite(ledPins[i], HIGH);
    }
}

/*
    Turn off all LEDs
*/
void turnOffAllLeds() {
    for (int i = 0; i < numButtons; i++) {
        digitalWrite(ledPins[i], LOW);
    }
}

/*
    Select random LEDs to blink and store their order
    each LED will blink for 0.5 seconds and will not repeat
*/
void selectRandomLedToBlink() {
    turnOffAllLeds();
    delay(1000);

    int randomIndices[4]; // Array to store three random indices
    int numSelected = 0; // Counter for how many LEDs are selected

    while (numSelected < 4) {
        int randomIndex = random(numButtons); // Generate a random index
        bool alreadySelected = false;
        for (int i = 0; i < numSelected; i++) {
            if (randomIndices[i] == randomIndex) {
                alreadySelected = true;
                break;
            }
        }
        if (!alreadySelected) {
            randomIndices[numSelected] = randomIndex;
            orderLedBlink[numSelected] = ledPins[randomIndex]; // Store the LED pin number in orderLedBlink
            numSelected++;
        }
    }

    // Blink the selected random LEDs
    for (int i = 0; i < 4; i++) {
        digitalWrite(ledPins[randomIndices[i]], HIGH);
        delay(500); 
        digitalWrite(ledPins[randomIndices[i]], LOW); 
        delay(500); 
    }

    // Print the order of LED blinking
    for (int i = 0; i < 4; i++) {
        Serial.print(orderLedBlink[i]);
    }

    // Start counting buttons after LED blinking
    countingButtons = true;

    // Clean nextButtonOrder array
    for (int i = 0; i < 4; i++) {
        nextButtonOrder[i] = 0;
    }
}

/*
    Store the order of the next 4 buttons pressed based on their pin numbers
*/

void storeNextButtonOrder(int buttonPin) { // Change the parameter to button pin number
    static int nextButtonIndex = 0; // Static variable to keep track of the next button order index
    bool alreadyStored = false;

    for (int i = 0; i < 4; i++) {
        if (nextButtonOrder[i] == buttonPin) {
            alreadyStored = true;
            break;
        }
    }

    if (!alreadyStored) {
        nextButtonOrder[nextButtonIndex++] = buttonPin; // Store button pin number instead of index
        if (nextButtonIndex == 4) {
            // Print the order of the next 4 buttons pressed
            Serial.print("Next 4 Button Order: ");
            for (int i = 0; i < 4; i++) {
                Serial.print(nextButtonOrder[i]);
                if (i < 3) {
                    Serial.print(", ");
                }
            }
            Serial.println();
            countingButtons = false; // Stop counting after storing the next 4 button orders
            nextButtonIndex = 0; // Reset the next button order index
        }
    }
}
