//TODO Get rid of Arduino Library #defines
#define PIN_LED_RED 10
#define PIN_LED_YELLOW 9
#define PIN_LED_GREEN 6
#define PIN_LED_BLUE 5

#define PIN_PUSHBUTTON_RED 3     // INT1
#define PIN_PUSHBUTTON_YELLOW 2  // INT0
#define PIN_PUSHBUTTON_GREEN 1
#define PIN_PUSHBUTTON_BLUE 0

// Output PORTs and BITs
#define REG_PORT_LED_RED PORTB
#define BIT_LED_RED 2
#define REG_PORT_LED_YELLOW PORTB
#define BIT_LED_YELLOW 1
#define REG_PORT_LED_GREEN PORTD
#define BIT_LED_GREEN 6
#define REG_PORT_LED_BLUE PORTD
#define BIT_LED_BLUE 5

// Input PORTs, PIN reg, and Bits Bits
#define REG_PORT_PUSHBUTTON_RED PORTD
#define REG_PIN_PUSHBUTTON_RED PIND
#define BIT_PUSHBUTTON_RED 3
#define REG_PORT_PUSHBUTTON_YELLOW PORTD
#define REG_PIN_PUSHBUTTON_YELLOW PIND
#define BIT_PUSHBUTTON_YELLOW 2
#define REG_PORT_PUSHBUTTON_GREEN PORTD
#define REG_PIN_PUSHBUTTON_GREEN PIND
#define BIT_PUSHBUTTON_GREEN 1
#define REG_PORT_PUSHBUTTON_BLUE PORTD
#define REG_PIN_PUSHBUTTON_BLUE PIND
#define BIT_PUSHBUTTON_BLUE 0

uint8_t blueState = HIGH;
uint8_t lastBlueState = HIGH;
uint8_t greenState = HIGH;
uint8_t lastGreenState = HIGH;

volatile uint8_t mainEventFlags = 0;
#define FLAG_YELLOW_PUSHBUTTON 0x01
#define FLAG_RED_PUSHBUTTON 0x02

void setup() {
  Serial.begin(9600);
  DDRB = _BV(BIT_LED_RED) | _BV(BIT_LED_YELLOW);  // Set RED and YELLOW LEDs as Output
  DDRD = _BV(BIT_LED_GREEN) | _BV(BIT_LED_BLUE);  // Set GREEN and BLUE LEDs as Output

  REG_PORT_PUSHBUTTON_RED |= _BV(BIT_PUSHBUTTON_RED);
  REG_PORT_PUSHBUTTON_YELLOW |= _BV(BIT_PUSHBUTTON_YELLOW);
  REG_PORT_PUSHBUTTON_GREEN |= _BV(BIT_PUSHBUTTON_GREEN);
  REG_PORT_PUSHBUTTON_BLUE |= _BV(BIT_PUSHBUTTON_BLUE);

  pinMode(PIN_PUSHBUTTON_RED, INPUT_PULLUP);
  pinMode(PIN_PUSHBUTTON_YELLOW, INPUT_PULLUP);
  pinMode(PIN_PUSHBUTTON_GREEN, INPUT_PULLUP);
  pinMode(PIN_PUSHBUTTON_BLUE, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(PIN_PUSHBUTTON_YELLOW), yellow_pushbutton_isr, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_PUSHBUTTON_RED), red_pushbutton_isr, FALLING);
}

void loopxx() {
  if (bit_is_clear(REG_PIN_PUSHBUTTON_RED, BIT_PUSHBUTTON_RED)) {
    REG_PORT_LED_RED |= _BV(BIT_LED_RED);
  } else {
    REG_PORT_LED_RED &= ~_BV(BIT_LED_RED);
  }
  
  if (bit_is_clear(REG_PIN_PUSHBUTTON_YELLOW, BIT_PUSHBUTTON_YELLOW)) {
    REG_PORT_LED_YELLOW |= _BV(BIT_LED_YELLOW);
  } else {
    REG_PORT_LED_YELLOW &= ~_BV(BIT_LED_YELLOW);
  }
  
  if (bit_is_clear(REG_PIN_PUSHBUTTON_GREEN, BIT_PUSHBUTTON_GREEN)) {
    REG_PORT_LED_GREEN |= _BV(BIT_LED_GREEN);
  } else {
    REG_PORT_LED_GREEN &= ~_BV(BIT_LED_GREEN);
  }
  
  if (bit_is_clear(REG_PIN_PUSHBUTTON_BLUE, BIT_PUSHBUTTON_BLUE)) {
    REG_PORT_LED_BLUE |= _BV(BIT_LED_BLUE);
  } else {
    REG_PORT_LED_BLUE &= ~_BV(BIT_LED_BLUE);
  }
}

void oldTestCode() {
  REG_PORT_LED_RED |= _BV(BIT_LED_RED);
  REG_PORT_LED_YELLOW |= _BV(BIT_LED_YELLOW);
  REG_PORT_LED_GREEN |= _BV(BIT_LED_GREEN);
  REG_PORT_LED_BLUE |= _BV(BIT_LED_BLUE);
  delay(1000);

  REG_PORT_LED_RED &= ~_BV(BIT_LED_RED);
  REG_PORT_LED_YELLOW &= ~_BV(BIT_LED_YELLOW);
  REG_PORT_LED_GREEN &= ~_BV(BIT_LED_GREEN);
  REG_PORT_LED_BLUE &= ~_BV(BIT_LED_BLUE);
  delay(1000);
}

void loop2() {
  // Keep as a reference for the Sequencing HW
  //digitalWrite(PIN_LED_RED,!digitalRead(PIN_PUSHBUTTON_RED));
  //digitalWrite(PIN_LED_YELLOW,!digitalRead(PIN_PUSHBUTTON_YELLOW));
  //digitalWrite(PIN_LED_GREEN,!digitalRead(PIN_PUSHBUTTON_GREEN));
  //digitalWrite(PIN_LED_BLUE,!digitalRead(PIN_PUSHBUTTON_BLUE));

  if (mainEventFlags & FLAG_YELLOW_PUSHBUTTON) {
    delay(30);
    mainEventFlags &= ~FLAG_YELLOW_PUSHBUTTON;
    if (!digitalRead(PIN_PUSHBUTTON_YELLOW)) {
      // Do the action!
      digitalWrite(PIN_LED_YELLOW, !digitalRead(PIN_LED_YELLOW));
    }
  }

  if (mainEventFlags & FLAG_RED_PUSHBUTTON) {
    delay(30);
    mainEventFlags &= ~FLAG_RED_PUSHBUTTON;
    if (!digitalRead(PIN_PUSHBUTTON_RED)) {
      // Do the action!
      digitalWrite(PIN_LED_RED, !digitalRead(PIN_LED_RED));
    }
  }

  greenState = digitalRead(PIN_PUSHBUTTON_GREEN);
  if (greenState != lastGreenState) {
    if (!greenState) {
      digitalWrite(PIN_LED_GREEN, !digitalRead(PIN_LED_GREEN));
    }
    delay(50);
  }
  lastGreenState = greenState;

  blueState = digitalRead(PIN_PUSHBUTTON_BLUE);
  if (blueState != lastBlueState) {
    if (!blueState) {
      digitalWrite(PIN_LED_BLUE, !digitalRead(PIN_LED_BLUE));
    }
    delay(50);
  }
  lastBlueState = blueState;
}

void yellow_pushbutton_isr() {
  mainEventFlags |= FLAG_YELLOW_PUSHBUTTON;
}

void red_pushbutton_isr() {
  mainEventFlags |= FLAG_RED_PUSHBUTTON;
}
