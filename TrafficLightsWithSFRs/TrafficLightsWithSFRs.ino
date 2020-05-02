#define PIN_LED_RED  			10 
#define PIN_LED_YELLOW  		9
#define PIN_LED_GREEN  			6
#define PIN_LED_BLUE  			5

#define PRESSED  	0
#define UNPRESSED  	1

#define PIN_PUSHBUTTON_BLUE  	0


int pushed = 0;

void setup()
{

  Serial.begin(9600);
  DDRD = B01000000;
  DDRB = B00000110;
  
  PORTD= B00000001;
}

void loop()
{
  if (digitalRead(PIN_PUSHBUTTON_BLUE) == LOW) {
      
  	digitalWrite(PIN_LED_GREEN, HIGH);
    pushed = 1;
  	delay(1000);
  } 
  if ((pushed == 1) && (digitalRead(PIN_PUSHBUTTON_BLUE) == HIGH)){
        // PORTD = ~_BV(PIN_LED_GREEN)
      digitalWrite(PIN_LED_GREEN, LOW);
      digitalWrite(PIN_LED_YELLOW, HIGH);
      delay(2000);
      digitalWrite(PIN_LED_RED, HIGH);
      digitalWrite(PIN_LED_YELLOW, LOW);
    pushed = 0;
  }
  
   
}