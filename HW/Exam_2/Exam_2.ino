#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

#define LINE_0 0
#define LINE_1 1
#define PIN_POT 0

#define REG_PORT_LED_YELLOW PORTB
#define BIT_LED_YELLOW 1
#define REG_PORT_LED_GREEN PORTD
#define BIT_LED_GREEN 6

#define REG_PORT_PUSHBUTTON_YELLOW PORTD
#define REG_PIN_PUSHBUTTON_YELLOW PIND
#define BIT_PUSHBUTTON_YELLOW 2
#define REG_PORT_PUSHBUTTON_GREEN PORTD
#define REG_PIN_PUSHBUTTON_GREEN PIND
#define BIT_PUSHBUTTON_GREEN 1
#define REG_PORT_PUSHBUTTON_BLUE PORTD
#define REG_PIN_PUSHBUTTON_BLUE PIND
#define BIT_PUSHBUTTON_BLUE 0

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
unsigned long priorTimeMs = 0;

void setup() {
  lcd.begin(16, 2);

  // pinMode(PIN_PUSHBUTTON_YELLOW, INPUT_PULLUP);
  // pinMode(PIN_PUSHBUTTON_GREEN, INPUT_PULLUP);
  // pinMode(PIN_PUSHBUTTON_BLUE, INPUT_PULLUP);
  DDRB = _BV(BIT_LED_YELLOW);  // Set RED and YELLOW LEDs as Output
  DDRD = _BV(BIT_LED_GREEN);   // Set GREEN and BLUE LEDs as Output
  REG_PORT_PUSHBUTTON_YELLOW |= _BV(BIT_PUSHBUTTON_YELLOW);
  REG_PORT_PUSHBUTTON_GREEN |= _BV(BIT_PUSHBUTTON_GREEN);
  REG_PORT_PUSHBUTTON_BLUE |= _BV(BIT_PUSHBUTTON_BLUE);

  attachInterrupt(digitalPinToInterrupt(PIN_PUSHBUTTON_YELLOW), yellow_pushbutton_isr, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_PUSHBUTTON_GREEN), green_pushbutton_isr, FALLING);
  // // EICRA = 0x0A;  // Set INT0 and INT1 to falling edge
  // EICRA = _BV(ISC11) | _BV(ISC01);
  // // EIMSK = 0x03;  // Turns on both INT0 and INT1
  // EIMSK = _BV(INT0) | _BV(INT1);

  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  digitalWrite(PIN_LED_YELLOW, LOW);
  digitalWrite(PIN_LED_GREEN, LOW);

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
    if (!digitalRead(PIN_PUSHBUTTON_GREEN)) {
      // Do the action!
      isGreenTimerRunning = !isGreenTimerRunning;

      digitalWrite(PIN_LED_GREEN, isGreenTimerRunning);
    }
  }

  if (mainEventFlags & FLAG_YELLOW_PUSHBUTTON) {
    delay(20);
    mainEventFlags &= ~FLAG_YELLOW_PUSHBUTTON;
    if (!digitalRead(PIN_PUSHBUTTON_YELLOW)) {
      // Do the action!
      isYellowTimerRunning = !isYellowTimerRunning;

      digitalWrite(PIN_LED_YELLOW, isYellowTimerRunning);
    }
  }

  if (!digitalRead(PIN_PUSHBUTTON_BLUE)) {
    // Do the action!
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_YELLOW, LOW);
    lcd.clear();
    isYellowTimerRunning = 0;
    yellowTimerTenthsSecond = 0;
    isGreenTimerRunning = 0;
    greenTimerThousandthsSecond = 0;
  }
  // unsigned long currentTimeMs = millis();
  // unsigned long elapsedTimeMs = currentTimeMs - priorTimeMs;
  // priorTimeMs = currentTimeMs;

  // if (isYellowTimerRunning) {
  //   yellowTimerTenthsSecond = yellowTimerTenthsSecond + elapsedTimeMs;
  // }
  // if (isGreenTimerRunning) {
  //   greenTimerThousandthsSecond = greenTimerThousandthsSecond + elapsedTimeMs;
  // }

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
void yellow_pushbutton_isr() {
  mainEventFlags |= FLAG_YELLOW_PUSHBUTTON;
}

void green_pushbutton_isr() {
  mainEventFlags |= FLAG_GREEN_PUSHBUTTON;
}

// ISR(INT0_vect) {
//   mainEventFlags |= FLAG_YELLOW_PUSHBUTTON;
// }

// ISR(INT1_vect) {
//   mainEventFlags |= FLAG_GREEN_PUSHBUTTON;
// }

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