#include <EnableInterrupt.h>

#define PIN_LED_RED 10
#define PIN_LED_YELLOW 9
#define PIN_LED_GREEN 6
#define PIN_LED_BLUE 5

#define PIN_PUSHBUTTON_RED 3     // Can be used for INT 1
#define PIN_PUSHBUTTON_YELLOW 2  // Can be used for INT 0
#define PIN_PUSHBUTTON_GREEN 1
#define PIN_PUSHBUTTON_BLUE 0
 
volatile uint8_t mainEventFlags = 0;
#define FLAG_YELLOW_PUSHBUTTON 0x01
#define FLAG_RED_PUSHBUTTON 0x02
#define FLAG_GREEN_PUSHBUTTON 0x04
#define FLAG_BLUE_PUSHBUTTON 0x08

void setup() {
  Serial.begin(9600);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_BLUE, OUTPUT);

  pinMode(PIN_PUSHBUTTON_RED, INPUT_PULLUP);
  pinMode(PIN_PUSHBUTTON_YELLOW, INPUT_PULLUP);
  pinMode(PIN_PUSHBUTTON_GREEN, INPUT_PULLUP);
  pinMode(PIN_PUSHBUTTON_BLUE, INPUT_PULLUP);

  // attachInterrupt(digitalPinToInterrupt(PIN_PUSHBUTTON_YELLOW), yellow_pushbutton_isr, FALLING);
  // attachInterrupt(digitalPinToInterrupt(PIN_PUSHBUTTON_RED), red_pushbutton_isr, FALLING);

  // TODO: Use a custom library calles EnableInterrupts by GreyGnome
  enableInterrupt(PIN_PUSHBUTTON_RED,  red_pushbutton_isr, FALLING);
  enableInterrupt(PIN_PUSHBUTTON_YELLOW,  yellow_pushbutton_isr, FALLING);
  enableInterrupt(PIN_PUSHBUTTON_GREEN,  green_pushbutton_isr, FALLING);
  enableInterrupt(PIN_PUSHBUTTON_BLUE,  blue_pushbutton_isr, FALLING);
}

void loop() {
  // Keep as a reference for the Sequencing HW
  //digitalWrite(PIN_LED_RED,!digitalRead(PIN_PUSHBUTTON_RED));
  //digitalWrite(PIN_LED_YELLOW,!digitalRead(PIN_PUSHBUTTON_YELLOW));
  //digitalWrite(PIN_LED_GREEN,!digitalRead(PIN_PUSHBUTTON_GREEN));
  //digitalWrite(PIN_LED_BLUE,!digitalRead(PIN_PUSHBUTTON_BLUE));

  if (mainEventFlags & FLAG_RED_PUSHBUTTON) {
    delay(30);
    mainEventFlags &= ~FLAG_RED_PUSHBUTTON;
    if (!digitalRead(PIN_PUSHBUTTON_RED)) {
      // Do the action!
      digitalWrite(PIN_LED_RED, !digitalRead(PIN_LED_RED));
    }
  }

  if (mainEventFlags & FLAG_YELLOW_PUSHBUTTON) {
    delay(30);
    mainEventFlags &= ~FLAG_YELLOW_PUSHBUTTON;
    if (!digitalRead(PIN_PUSHBUTTON_YELLOW)) {
      // Do the action!
      digitalWrite(PIN_LED_YELLOW, !digitalRead(PIN_LED_YELLOW));
    }
  }

  if (mainEventFlags & FLAG_GREEN_PUSHBUTTON) {
    delay(30);
    mainEventFlags &= ~FLAG_GREEN_PUSHBUTTON;
    if (!digitalRead(PIN_PUSHBUTTON_GREEN)) {
      // Do the action!
      digitalWrite(PIN_LED_GREEN, !digitalRead(PIN_LED_GREEN));
    }
  }

  if (mainEventFlags & FLAG_BLUE_PUSHBUTTON) {
    delay(30);
    mainEventFlags &= ~FLAG_BLUE_PUSHBUTTON;
    if (!digitalRead(PIN_PUSHBUTTON_BLUE)) {
      // Do the action!
      digitalWrite(PIN_LED_BLUE, !digitalRead(PIN_LED_BLUE));
    }
  }
}

void red_pushbutton_isr() {
  mainEventFlags |= FLAG_RED_PUSHBUTTON;
}

void yellow_pushbutton_isr() {
  mainEventFlags |= FLAG_YELLOW_PUSHBUTTON;
}

void green_pushbutton_isr() {
  mainEventFlags |= FLAG_GREEN_PUSHBUTTON;
}

void blue_pushbutton_isr() {
  mainEventFlags |= FLAG_BLUE_PUSHBUTTON;
}
