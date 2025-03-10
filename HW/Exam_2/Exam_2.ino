#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

#define LINE_0 0
#define LINE_1 1
#define PIN_POT 0

#define REG_PORT_LED_YELLOW PORTD
#define BIT_LED_YELLOW 7
#define REG_PORT_LED_GREEN PORTD
#define BIT_LED_GREEN 6

#define REG_PORT_PUSHBUTTON_YELLOW PORTD
#define REG_PIN_PUSHBUTTON_YELLOW PIND
#define BIT_PUSHBUTTON_YELLOW 3
#define REG_PORT_PUSHBUTTON_GREEN PORTD
#define REG_PIN_PUSHBUTTON_GREEN PIND
#define BIT_PUSHBUTTON_GREEN 2
#define REG_PORT_PUSHBUTTON_BLUE PORTD
#define REG_PIN_PUSHBUTTON_BLUE PIND
#define BIT_PUSHBUTTON_BLUE 4

// Using a 16 MHz clock, we want 0.1 second interrupts();
// Timer 1 used for the Yellow Timer
// Legal values are 0 to 65536
// Prescalar 1 --> 1,600,00
// Prescalar 8 --> 200,000
// Prescalar 64 --> 25,000      // Highest possible legal value
// Prescalar 256 --> 6250
// Prescalar 1024 --> 1562.5    // Try to avoid non-integers
// Tiemr 2 used for the Green Timer (use 0.001 second interrupts)
// Legal values are 0 to 256
// Prescalar 1 --> 16,000
// Prescalar 8 --> 2000
// Prescalar 64 --> 250      // Highest possible legal value

#define TIMER_1_START 0
#define TIMER_1_COMPARE 25000
#define TIMER_2_START 0
#define TIMER_2_COMPARE 250

#define PIN_PUSHBUTTON_GREEN 2
#define PIN_PUSHBUTTON_YELLOW 3
#define PIN_PUSHBUTTON_BLUE 4
#define PIN_LED_GREEN 6
#define PIN_LED_YELLOW 7

volatile int mainEventFlags = 0;
#define FLAG_YELLOW_PUSHBUTTON 0x01
#define FLAG_GREEN_PUSHBUTTON 0x02

unsigned long yellowTimerTenthsSecond;
uint8_t isYellowTimerRunning = 0;
unsigned long greenTimerThousandthsSecond;
uint8_t isGreenTimerRunning = 0;

int yellowDebounceMs = 0;
int greenDebounceMs = 0;

volatile uint8_t portdhistory = 0xFF;

void setup() {
  lcd.begin(16, 2);

  REG_PORT_PUSHBUTTON_YELLOW |= _BV(BIT_PUSHBUTTON_YELLOW);
  REG_PORT_PUSHBUTTON_GREEN |= _BV(BIT_PUSHBUTTON_GREEN);
  REG_PORT_PUSHBUTTON_BLUE |= _BV(BIT_PUSHBUTTON_BLUE);

  PCICR = _BV(PCIE2);
  PCMSK2 = _BV(PCINT19) | _BV(PCINT18);

  DDRD = _BV(BIT_LED_GREEN) | _BV(BIT_LED_YELLOW);

  REG_PORT_LED_YELLOW &= ~_BV(BIT_LED_YELLOW);
  REG_PORT_LED_GREEN &= ~_BV(BIT_LED_GREEN);

  // Timer 1 setup
  TCCR1A = 0;  // Reset Timer1 Control Reg A
  TCCR2A = 0;  // Reset Timer1 Control Reg A
  // Set to prescaler of 64
  TCCR1B &= ~_BV(CS12);
  TCCR1B |= _BV(CS11);
  TCCR1B |= _BV(CS10);

  TCCR2B &= ~_BV(CS22);
  TCCR2B |= _BV(CS21);
  TCCR2B |= _BV(CS20);
  //TCCR1B = 0x04;  // Same code but shorter
  // Reset Timer1 and set compare value
  TCNT1 = TIMER_1_START;
  OCR1A = TIMER_1_COMPARE;  // Reset Timer1 and set compare value
  TIMSK1 = _BV(OCIE1A);     // Enable Timer1 overflow interrupt

  TCNT2 = TIMER_2_START;
  OCR2A = TIMER_2_COMPARE;  // Reset Timer1 and set compare value
  TIMSK2 = _BV(OCIE1A);     // Enable Timer1 overflow interrupt
  sei();                    // Enable global interrupts
}

void loop() {
  if (mainEventFlags & FLAG_GREEN_PUSHBUTTON) {
    delay(20);
    mainEventFlags &= ~FLAG_GREEN_PUSHBUTTON;
    if (bit_is_clear(REG_PIN_PUSHBUTTON_GREEN, BIT_PUSHBUTTON_GREEN)) {
      // Do the action!
      isGreenTimerRunning = !isGreenTimerRunning;

      REG_PORT_LED_GREEN ^= _BV(BIT_LED_GREEN);
    }
  }

  if (mainEventFlags & FLAG_YELLOW_PUSHBUTTON) {
    delay(20);
    mainEventFlags &= ~FLAG_YELLOW_PUSHBUTTON;
    if (bit_is_clear(REG_PIN_PUSHBUTTON_YELLOW, BIT_PUSHBUTTON_YELLOW)) {
      // Do the action!
      isYellowTimerRunning = !isYellowTimerRunning;
      REG_PORT_LED_YELLOW ^= _BV(BIT_LED_YELLOW);
    }
  }

  if (bit_is_clear(REG_PIN_PUSHBUTTON_BLUE, BIT_PUSHBUTTON_BLUE)) {
    // Do the action!
    REG_PORT_LED_YELLOW &= ~_BV(BIT_LED_YELLOW);
    REG_PORT_LED_GREEN &= ~_BV(BIT_LED_GREEN);
    lcd.clear();
    isYellowTimerRunning = 0;
    yellowTimerTenthsSecond = 0;
    isGreenTimerRunning = 0;
    greenTimerThousandthsSecond = 0;
  }
  yellowDebounceMs++;
   greenDebounceMs++;
  updateLcd();
  // delay(100);
}

void updateLcd() {
  lcd.setCursor(0, LINE_1);
  lcd.print(yellowTimerTenthsSecond / 10);
  lcd.print(".");
  lcd.print(yellowTimerTenthsSecond % 10);

  lcd.setCursor(0, LINE_0);
  lcd.print(greenTimerThousandthsSecond / 1998);
  lcd.print(".");
  lcd.print(greenTimerThousandthsSecond * 5 / 999 % 10);
  //lcd.print("    ");
}

ISR(TIMER1_COMPA_vect) {
  TCNT1 = TIMER_1_START;
  if (isYellowTimerRunning) {
    yellowTimerTenthsSecond++;
  }
}

ISR(TIMER2_COMPA_vect) {
  TCNT2 = TIMER_2_START;
  if (isGreenTimerRunning) {
    greenTimerThousandthsSecond++;
  }
}

ISR(PCINT2_vect) {
  // ISR called means that RD0, RD1, RD2, or RD3 changed.
  uint8_t changedbits;
  changedbits = PIND ^ portdhistory;  // All pins are on port D
  portdhistory = PIND;
  if (changedbits & _BV(BIT_PUSHBUTTON_YELLOW)) {
    if ((yellowDebounceMs > 30)) {
      if (bit_is_clear(REG_PIN_PUSHBUTTON_YELLOW, BIT_PUSHBUTTON_YELLOW)) {
        mainEventFlags |= FLAG_YELLOW_PUSHBUTTON;
        greenDebounceMs = 0;
      }
    }
  }
  if (changedbits & _BV(BIT_PUSHBUTTON_GREEN)) {
    if (yellowDebounceMs > 30) {
      if (bit_is_clear(REG_PIN_PUSHBUTTON_GREEN, BIT_PUSHBUTTON_GREEN)) {
        mainEventFlags |= FLAG_GREEN_PUSHBUTTON;
        yellowDebounceMs = 0;
      }
    }
  }
}
