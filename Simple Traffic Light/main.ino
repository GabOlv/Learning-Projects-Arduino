/*
  Traffic light simulation with Arduino, where a button is used to change the light state, 
  simulating pedestrians possibility of changing the light state if needed. 
*/

const int button_pin = 12; // button pin
const int red_led = 10; // red LED pin
const int yellow_led = 9;  // yellow LED pin
const int green_led = 8;  // green LED pin

unsigned long previousMillis = 0; // stores last time LEDs were updated
unsigned long interval_red = 5000; // red light interval in milliseconds
unsigned long interval_green = 5000; // green light interval in milliseconds
unsigned long interval_yellow = 3000; // yellow light interval in milliseconds

int button_anterior = HIGH; // previous button state
int button_now; // current button state

int light_state = 0; // traffic-light state (0 = red, 1 = green, 2 = yellow)

void reset_timer() {
  previousMillis = millis(); // reset timer
}

// setup function
void setup() {
  pinMode(button_pin, INPUT_PULLUP); // configure button pin as pull-up input
  pinMode(red_led, OUTPUT); // set red, yellow, and green LEDs as outputs
  pinMode(yellow_led, OUTPUT);
  pinMode(green_led, OUTPUT);
}

// loop function
void loop() {
  button_now = digitalRead(button_pin); // read button state

  // handle button press and state transitions
  if (button_now == LOW && button_anterior == HIGH && light_state == 1) {
    light_state = 2; // change state to yellow
    reset_timer(); // start timer for yellow
  }

  unsigned long currentMillis = millis(); // get current time
  unsigned long elapsedTime = currentMillis - previousMillis; // calculate elapsed time

  // control the LED based on the traffic light state and elapsed time
  if (light_state == 0) { // red
    digitalWrite(red_led, HIGH);
    digitalWrite(yellow_led, LOW);
    digitalWrite(green_led, LOW);
    if (elapsedTime >= interval_red) {
      light_state = 1; // change to green after red interval
      reset_timer(); // restart timer for green
    }
  } else if (light_state == 1) { // green
    digitalWrite(red_led, LOW);
    digitalWrite(yellow_led, LOW);
    digitalWrite(green_led, HIGH);
    if (elapsedTime >= interval_green) {
      light_state = 2; // change to yellow after green interval
      reset_timer(); // restart timer for yellow
    }
  } else if (light_state == 2) { // yellow
    digitalWrite(red_led, LOW);
    digitalWrite(yellow_led, HIGH);
    digitalWrite(green_led, LOW);
    if (elapsedTime >= interval_yellow) {
      light_state = 0; // change to red after yellow interval
      reset_timer(); // restart timer for red
    }
  }

  button_anterior = button_now; // update previous button state
}
