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
#define FLAG_RED_PUSHBUTTON 0x01
#define FLAG_YELLOW_PUSHBUTTON 0x02
#define FLAG_GREEN_PUSHBUTTON 0x04
#define FLAG_BLUE_PUSHBUTTON 0x08

volatile uint8_t portdhistory = 0xFF;

int ledSequence[10] = {BIT_LED_BLUE,
                       BIT_LED_BLUE,
                       BIT_LED_BLUE,
                       BIT_LED_BLUE,
                       BIT_LED_BLUE,
                       BIT_LED_BLUE,
                       BIT_LED_BLUE,
                       BIT_LED_BLUE,
                       BIT_LED_BLUE,
                       BIT_LED_BLUE};

int index = 0;

void setup() {
  Serial.begin(9600);
  DDRB = _BV(BIT_LED_RED) | _BV(BIT_LED_YELLOW);  // Set RED and YELLOW LEDs as Output
  DDRD = _BV(BIT_LED_GREEN) | _BV(BIT_LED_BLUE);  // Set GREEN and BLUE LEDs as Output
  REG_PORT_PUSHBUTTON_RED |= _BV(BIT_PUSHBUTTON_RED);
  REG_PORT_PUSHBUTTON_YELLOW |= _BV(BIT_PUSHBUTTON_YELLOW);
  REG_PORT_PUSHBUTTON_GREEN |= _BV(BIT_PUSHBUTTON_GREEN);
  REG_PORT_PUSHBUTTON_BLUE |= _BV(BIT_PUSHBUTTON_BLUE);

  PCICR = _BV(PCIE2);  // Enable Pin Change Interrupts for Port D pins
  //PCMSK2 = 0x0F;                                                       // Enables RD0 - RD3 as interrupts
  PCMSK2 = _BV(PCINT19) | _BV(PCINT18) | _BV(PCINT17) | _BV(PCINT16);  // Enables RD0 - RD3 as interrupts

  sei();  // Turn on interrupts globally. Not required for us, since Arduino does it.
}

void showFeedbackLeds() { 
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


void loop() {
  showFeedbackLeds();
  
  if (mainEventFlags & FLAG_RED_PUSHBUTTON) {
    delay(30);
    mainEventFlags &= ~FLAG_RED_PUSHBUTTON;
    if (bit_is_clear(REG_PIN_PUSHBUTTON_RED, BIT_PUSHBUTTON_RED)) {
      // Do the action!
      // REG_PORT_LED_RED ^= _BV(BIT_LED_RED);
      addLed(BIT_LED_RED);
    }
  }
  if (mainEventFlags & FLAG_YELLOW_PUSHBUTTON) {
    delay(30);
    mainEventFlags &= ~FLAG_YELLOW_PUSHBUTTON;
    if (bit_is_clear(REG_PIN_PUSHBUTTON_YELLOW, BIT_PUSHBUTTON_YELLOW)) {
      // Do the action!
      addLed(BIT_LED_YELLOW);
    }
  }
  if (mainEventFlags & FLAG_GREEN_PUSHBUTTON) {
    delay(30);
    mainEventFlags &= ~FLAG_GREEN_PUSHBUTTON;
    if (bit_is_clear(REG_PIN_PUSHBUTTON_GREEN, BIT_PUSHBUTTON_GREEN)) {
      // Do the action!
      addLed(BIT_LED_GREEN);
    }
  }
  if (mainEventFlags & FLAG_BLUE_PUSHBUTTON) {
    delay(30);
    mainEventFlags &= ~FLAG_BLUE_PUSHBUTTON;
    if (bit_is_clear(REG_PIN_PUSHBUTTON_BLUE, BIT_PUSHBUTTON_BLUE)) {
      // Do the action!
      runSequence();
      for (int i = 0; i < 10; i++) {
        ledSequence[i] = BIT_LED_BLUE;
      }
      index = 0;
    }
  }
}

void addLed(uint8_t ledToAdd) {
  if (index < 10) {
    ledSequence[index] = ledToAdd;
    index++;
  }
}

void runSequence() {
  REG_PORT_LED_BLUE &= ~_BV(BIT_LED_BLUE);
  for (int i = 0; i < 10; i++) {
    uint8_t activeBit = ledSequence[i];
    // digitalWrite(ledSequence[i], HIGH);
    if (activeBit == BIT_LED_RED || activeBit == BIT_LED_YELLOW) {
      REG_PORT_LED_RED |= _BV(activeBit);
    } else {
      REG_PORT_LED_GREEN |= _BV(activeBit);
    }
    delay(1000);
    // digitalWrite(ledSequence[i], LOW);
    if (activeBit == BIT_LED_RED || activeBit == BIT_LED_YELLOW) {
      REG_PORT_LED_RED ^= _BV(activeBit);
    } else {
      REG_PORT_LED_GREEN ^= _BV(activeBit);
    }
    delay(100);
  }
}


              // if (bit_is_clear(REG_PIN_PUSHBUTTON_RED, BIT_PUSHBUTTON_RED)) {
              //   REG_PORT_LED_RED |= _BV(BIT_LED_RED);
              // } else {
              //   REG_PORT_LED_RED &= ~_BV(BIT_LED_RED);
              // }

ISR(PCINT2_vect) {
  // ISR called means that RD0, RD1, RD2, or RD3 changed.
  uint8_t changedbits;
  changedbits = PIND ^ portdhistory;  // All pins are on port D
  portdhistory = PIND;
  if (changedbits & _BV(BIT_PUSHBUTTON_RED)) {
    if (bit_is_clear(REG_PIN_PUSHBUTTON_RED, BIT_PUSHBUTTON_RED)) {
      mainEventFlags |= FLAG_RED_PUSHBUTTON;
    }
  }
  if (changedbits & _BV(BIT_PUSHBUTTON_YELLOW)) {
    if (bit_is_clear(REG_PIN_PUSHBUTTON_YELLOW, BIT_PUSHBUTTON_YELLOW)) {
      mainEventFlags |= FLAG_YELLOW_PUSHBUTTON;
    }
  }
  if (changedbits & _BV(BIT_PUSHBUTTON_GREEN)) {
    if (bit_is_clear(REG_PIN_PUSHBUTTON_GREEN, BIT_PUSHBUTTON_GREEN)) {
      mainEventFlags |= FLAG_GREEN_PUSHBUTTON;
    }
  }
  if (changedbits & _BV(BIT_PUSHBUTTON_BLUE)) {
    if (bit_is_clear(REG_PIN_PUSHBUTTON_BLUE, BIT_PUSHBUTTON_BLUE)) {
      mainEventFlags |= FLAG_BLUE_PUSHBUTTON;
    }
  }
  delay(30);
}