/*
    Initialize the lists with the pins and buttons positions
    And get the number of buttons
*/

const int ledPins[] = {2, 3, 4};
const int buttonPins[] = {5, 6, 7};
const int startButtonPin = 12;
const int startLedIndicator[] = {9, 10, 11};
const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

// Global variable to store the order of LED blinking
int orderLedBlink[3];

// Global variable to store the order of next 3 buttons pressed
int nextButtonOrder[3];

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
    In Main loop:
    - Read the state of the button
    - If the button is pressed, activate the LED else turn off the LED
    - Call the function to store the order of the next 4 buttons pressed after pressing the start button
*/

void loop() {
    for (int i = 0; i < numButtons; i++) {
        if (digitalRead(buttonPins[i]) == HIGH) {
            activateLed(i);
            delay(100);
          
            // Call the function to store the order of the next 4 buttons pressed after pressing the start button
            if (countingButtons) {
                storeNextButtonOrder(buttonPins[i]);
            }
        }
    }

    // Check if the select button (12) is pressed
    if (digitalRead(startButtonPin) == HIGH) {
      // Activate the Indicator Leds
        for(int i = 0; i < 2; i++){
            digitalWrite(startLedIndicator[i], HIGH);
        }
        // Call the function to select the random Leds
        selectRandomLedToBlink();
        countingButtons = true; // Start counting buttons after LED blinking
    }
  
}

/*
    In activateLed function:
    - Turn on the LED if the index is equal to the button index declared in the buttonPins list.
*/

void activateLed(int buttonIndex) {
    for (int i = 0; i < numButtons; i++) {
        digitalWrite(ledPins[i], i == buttonIndex ? HIGH : LOW);
    }
}

/*
    In sequentialLeds function:
    - Turn on all LEDs repeatedly for 3 seconds then turn them off after some seconds.
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

    turnOnAllLeds();
    delay(1000);
    turnOffAllLeds();
}

/*
    In turnOnAllLeds function:
    - Turn on all LEDs to High state.
*/
void turnOnAllLeds() {
    for (int i = 0; i < numButtons; i++) {
        delay(10);
        digitalWrite(ledPins[i], HIGH);
    }
}

/*
    In turnOffAllLeds function:
    - Turn off all LEDs to Low state.
*/

void turnOffAllLeds() {
    for (int i = 0; i < numButtons; i++) {
        digitalWrite(ledPins[i], LOW);
    }
}

/*
    In selectRandomLedToBlink function:
    - Blink the selected random LEDs based on the order of the LEDs declared in the ledPins list.
    - Start counting buttons pins after the random LED blinking enabling the flag countingButtons.
*/

void selectRandomLedToBlink() {
    turnOffAllLeds();
    delay(800);

    // Array to store random indices and count the number of selected LEDs.
    int randomIndices[3]; 
    int numSelected = 0; 

    while (numSelected < 3) {
        int randomIndex = random(numButtons); // Generate a random index between 0 and numButtons.
        bool alreadySelected = false;
        for (int i = 0; i < numSelected; i++) {
            if (randomIndices[i] == randomIndex) {
                alreadySelected = true; // Prevent repeatedly selecting the same index.
                break;
            }
        }

        if (!alreadySelected) {
            randomIndices[numSelected] = randomIndex; // Store the random index in randomIndices
            orderLedBlink[numSelected] = ledPins[randomIndex]; // Store the LED pin number in orderLedBlink
            numSelected++;
        }
    }

    // Blink the selected random LEDs
    for (int i = 0; i < 3; i++) {
        digitalWrite(ledPins[randomIndices[i]], HIGH);
        delay(500); 
        digitalWrite(ledPins[randomIndices[i]], LOW); 
        delay(500); 
    }

    // Flag to start counting buttons after the random LED blinking
    countingButtons = true;

    // Clean nextButtonOrder array before storing the next 3 button orders
    for (int i = 0; i < 3; i++) {
        nextButtonOrder[i] = 0;
    }
}

/*
    In storeNextButtonOrder function:
    - Store the next 3 button pins pressed after pressing the start button.
    - Call the function to check if the condition to win is met.
*/

void storeNextButtonOrder(int buttonPin) { // Change the parameter to button pin number
    static int nextButtonIndex = 0; // Static variable to keep track of the next button order index
    bool alreadyStored = false;

    for (int i = 0; i < 3; i++) {
        if (nextButtonOrder[i] == buttonPin) {
            alreadyStored = true;
            break;
        }
    }

    if (!alreadyStored) {
        nextButtonOrder[nextButtonIndex++] = buttonPin; // Store button pin number instead of index
        if (nextButtonIndex == 3) {
            countingButtons = false; // Stop counting after storing the next 4 button orders
            nextButtonIndex = 0; // Reset the next button order index
        	finalResult();
        }
    }
  
}

/*	
	Condition to win: Led,Button: {2,5}, {3,6}, {4,7}
    Logic to win LedPin[i] = ButtonPin[i] + 3
*/

void finalResult() {
    bool passed = false;
  	int cont = 0;

    // Check if the next button order is correct based on the logic
    for (int i = 0;i < numButtons; i++){
  	    if(nextButtonOrder[i] == (orderLedBlink[i] + 3)){
     	    cont = cont+1;
        } 
    }

    // Check if the condition to win is met
    if(cont == numButtons){
    	passed = true;
    } else {
     	passed = false;
    }

    // Call the function to blink the LEDs based on the condition to win
    if (passed) {
      // const int startLedIndicator[] = {9, 10, 11};  g/b/r
        // Green Light
        digitalWrite(startLedIndicator[0], HIGH);
        digitalWrite(startLedIndicator[1], LOW);
        digitalWrite(startLedIndicator[2], LOW);
        
      	sequentialLeds();

        // deactivate RGB Led
        for(int i = 0; i < 2; i++){
        	digitalWrite(startLedIndicator[i], LOW);
    	}
      
    } else {
        // Red Light
        digitalWrite(startLedIndicator[0], LOW);
        digitalWrite(startLedIndicator[1], LOW);
        digitalWrite(startLedIndicator[2], HIGH);
	
        turnOnAllLeds();
    	  delay(1000);
      
        digitalWrite(startLedIndicator[0], LOW);
        digitalWrite(startLedIndicator[1], LOW);
        digitalWrite(startLedIndicator[2], LOW);
      

      turnOffAllLeds();
    }
}