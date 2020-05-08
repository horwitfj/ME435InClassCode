#define PIN_LED_RED 10
#define PIN_LED_YELLOW 9
#define PIN_LED_GREEN 6
#define PIN_LED_BLUE 5

#define PIN_PUSHBUTTON_RED 3
#define PIN_PUSHBUTTON_YELLOW 2
#define PIN_PUSHBUTTON_GREEN 1
#define PIN_PUSHBUTTON_BLUE 0

uint8_t blueState = HIGH;
uint8_t lastBlueState = HIGH;
uint8_t greenState = HIGH;
uint8_t lastGreenState = HIGH;

volatile uint8_t mainEventFlags = 0;
#define FLAG_YELLOW_PUSHBUTTON 0x01
#define FLAG_RED_PUSHBUTTON 0x02

int ledSequence[10] = {PIN_LED_BLUE,
                       PIN_LED_BLUE,
                       PIN_LED_BLUE,
                       PIN_LED_BLUE,
                       PIN_LED_BLUE,
                       PIN_LED_BLUE,
                       PIN_LED_BLUE,
                       PIN_LED_BLUE,
                       PIN_LED_BLUE,
                       PIN_LED_BLUE};

int index = 0;

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

  attachInterrupt(digitalPinToInterrupt(PIN_PUSHBUTTON_RED), red_pushbutton_isr, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_PUSHBUTTON_YELLOW), yellow_pushbutton_isr, FALLING);
}

void loop() {
  // Give feedback when a button is pressed.
  digitalWrite(PIN_LED_RED, !digitalRead(PIN_PUSHBUTTON_RED));
  digitalWrite(PIN_LED_YELLOW, !digitalRead(PIN_PUSHBUTTON_YELLOW));
  digitalWrite(PIN_LED_GREEN, !digitalRead(PIN_PUSHBUTTON_GREEN));

  if (mainEventFlags & FLAG_RED_PUSHBUTTON) {
    delay(30);
    mainEventFlags &= ~FLAG_RED_PUSHBUTTON;
    if (!digitalRead(PIN_PUSHBUTTON_RED)) {
      addLed(PIN_LED_RED);
    }
  }

  if (mainEventFlags & FLAG_YELLOW_PUSHBUTTON) {
    delay(30);
    mainEventFlags &= ~FLAG_YELLOW_PUSHBUTTON;
    if (!digitalRead(PIN_PUSHBUTTON_YELLOW)) {
      addLed(PIN_LED_YELLOW);
    }
  }

  greenState = digitalRead(PIN_PUSHBUTTON_GREEN);
  if (greenState != lastGreenState) {
    if (!greenState) {
      addLed(PIN_LED_GREEN);
    }
  }
  lastGreenState = greenState;

  blueState = digitalRead(PIN_PUSHBUTTON_BLUE);
  if (blueState != lastBlueState) {
    if (!blueState) {
      runSequence();
      for (int i = 0; i < 10; i++) {
        ledSequence[i] = PIN_LED_BLUE;
      }
      index = 0;
    }
    delay(50);
  }
  lastBlueState = blueState;
}

void addLed(uint8_t ledToAdd) {
  if (index < 10) {
    ledSequence[index] = ledToAdd;
    index++;
  }
}

void runSequence() {
  for (int i = 0; i < 10; i++) {
    digitalWrite(ledSequence[i], HIGH);
    delay(1000);
    digitalWrite(ledSequence[i], LOW);
    delay(100);
  }
}

void red_pushbutton_isr() {
  mainEventFlags |= FLAG_RED_PUSHBUTTON;
}

void yellow_pushbutton_isr() {
  mainEventFlags |= FLAG_YELLOW_PUSHBUTTON;
}

// Use PCINT
