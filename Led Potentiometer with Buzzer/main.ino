/*  
    Project : LED potentiometer meter with buzzer and Leds
    This simple project will either turn on or off the leds depending on the potentiometer value
    and will sound the buzzer when the potentiometer value is over 765
*/

int pot = A0; 
float potMeter;
int buz = 2; 
unsigned cont; 

const int ledPins[] = {7, 6 ,5 ,4};
const int numPins = sizeof(ledPins) / sizeof(ledPins[0]);

/*
    Setup function
    Set the pins as inputs and outputs
*/
void setup(){
  
	pinMode(pot, INPUT);
  	pinMode(buz, OUTPUT);
  
  	for (cont = 0; cont < numPins; cont++) {
        pinMode(ledPins[cont], OUTPUT);
    }
      
 	Serial.begin(9600);
}

void loop(){  
    potMeter = analogRead(pot);
  
	if (potMeter > 0.0 && potMeter <= 255){
      	digitalWrite(ledPins[0], HIGH);	
    	for (cont = 1; cont < numPins; cont++){
  			digitalWrite(ledPins[cont], LOW);
        }
    }

  	if(potMeter > 256.0 && potMeter <= 511.0){
  		digitalWrite(ledPins[1], HIGH);
        for(cont=2; cont< numPins; cont++){
        	digitalWrite(ledPins[cont], LOW);
        }
  	}
    if(potMeter > 512.0 && potMeter <= 765){
    	digitalWrite(ledPins[2], HIGH);
    	for(cont = 3; cont < numPins; cont++){
        	digitalWrite(ledPins[cont], LOW);
        }
    }
    if(potMeter > 765){
    	for(cont = 0; cont < numPins; cont++){
        	digitalWrite(ledPins[cont], HIGH);
        }
      	digitalWrite(buz, HIGH);
        tone(13, 261, 500);     
      	delay(500);
    } else {
    	digitalWrite(buz, LOW); 
    }
}
  
