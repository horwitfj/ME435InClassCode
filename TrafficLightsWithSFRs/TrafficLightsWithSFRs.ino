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
  int btn = (PIND & _BV(PIN_PUSHBUTTON_BLUE)) >> PIN_PUSHBUTTON_BLUE;


  if (btn == LOW) {
      PORTD = _BV(PIN_LED_GREEN) | PORTD;
    pushed = 1;
  	delay(1000);
  } 
  if ((pushed == 1) && (btn == HIGH)){
      PORTD = ~_BV(PIN_LED_GREEN) | PORTD;
      PORTB =  _BV(PIN_LED_YELLOW) | PORTB;
        
      delay(2000);
      
      PORTB = ~_BV(PIN_LED_YELLOW) | PORTB;
      PORTB =  _BV(PIN_LED_RED) | PORTB;
    pushed = 0;
  }
  
   
}