#define PIN_LED_RED 10
#define PIN_LED_YELLOW 9
#define PIN_LED_GREEN 6
#define PIN_LED_BLUE 5

#define PIN_PUSHBUTTON_RED 7
#define PIN_PUSHBUTTON_YELLOW 4
#define PIN_PUSHBUTTON_GREEN 3
#define PIN_PUSHBUTTON_BLUE 2

#define PIN_RGB_RED 11
#define PIN_RGB_GREEN 12
#define PIN_RGB_BLUE 13

#define PIN_ANALOG_POT 0
#define PIN_ANALOG_PHOTOCELL 1

volatile uint8_t mainEventFlags = 0;
#define FLAG_GREEN_PUSHBUTTON 0x01
#define FLAG_BLUE_PUSHBUTTON 0x02

int pot;
int photo;
int rgbCounter = 0;
unsigned long priorTimeMs = 0;

uint8_t redState = HIGH;
uint8_t lastRedState = HIGH;
uint8_t yellowState = HIGH;
uint8_t lastYellowState = HIGH;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_BLUE, OUTPUT);
  digitalWrite(PIN_LED_RED, LOW);
  digitalWrite(PIN_LED_YELLOW, LOW);
  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_BLUE, LOW);

  pinMode(PIN_PUSHBUTTON_RED, INPUT_PULLUP);
  pinMode(PIN_PUSHBUTTON_YELLOW, INPUT_PULLUP);
  pinMode(PIN_PUSHBUTTON_GREEN, INPUT_PULLUP);
  pinMode(PIN_PUSHBUTTON_BLUE, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(PIN_PUSHBUTTON_GREEN), green_pushbutton_isr, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_PUSHBUTTON_BLUE), blue_pushbutton_isr, FALLING);
}

void loop() {
  unsigned long currentTimeMs = millis();
  unsigned long elapsedTimeMs = currentTimeMs - priorTimeMs;

  if (elapsedTimeMs > 2000) {
    pot = analogRead(PIN_ANALOG_POT);
    photo = analogRead(PIN_ANALOG_PHOTOCELL);
    Serial.print("Pot =");
    Serial.println(pot);
    Serial.print("    Photocell =");
    Serial.println(pot);
    Serial.print(" \n");
    priorTimeMs = currentTimeMs;
  }

  redState = digitalRead(PIN_PUSHBUTTON_RED);
  if (redState != lastRedState) {
    if (!redState) {
      if (digitalRead(PIN_LED_RED) == HIGH) {
        digitalWrite(PIN_LED_RED, LOW);
      } else {
        digitalWrite(PIN_LED_RED, HIGH);
      }
    }
    delay(50);
  }
  lastRedState = redState;

  yellowState = digitalRead(PIN_PUSHBUTTON_YELLOW);
  if (yellowState != lastYellowState) {
    if (!yellowState) {
      digitalWrite(PIN_LED_YELLOW, !digitalRead(PIN_LED_YELLOW));
    }
    delay(50);
  }
  lastYellowState = yellowState;

  if (mainEventFlags & FLAG_GREEN_PUSHBUTTON) {
    delay(30);
    mainEventFlags &= ~FLAG_GREEN_PUSHBUTTON;
    if (!digitalRead(PIN_PUSHBUTTON_GREEN)) {
      digitalWrite(PIN_LED_GREEN, !digitalRead(PIN_LED_GREEN));
    }
  }

  if (mainEventFlags & FLAG_BLUE_PUSHBUTTON) {
    delay(30);
    mainEventFlags &= ~FLAG_BLUE_PUSHBUTTON;
    if (!digitalRead(PIN_PUSHBUTTON_BLUE)) {
      digitalWrite(PIN_LED_BLUE, !digitalRead(PIN_LED_BLUE));
      rgbCounter++;
      if (rgbCounter > 7) {
        rgbCounter = 0;
      }
      digitalWrite(PIN_RGB_BLUE, rgbCounter % 2);
      digitalWrite(PIN_RGB_GREEN, rgbCounter / 2 % 2);
      digitalWrite(PIN_RGB_RED, rgbCounter / 4 % 2);
    }
  }
}

void green_pushbutton_isr() {
  mainEventFlags |= FLAG_GREEN_PUSHBUTTON;
}

void blue_pushbutton_isr() {
  mainEventFlags |= FLAG_BLUE_PUSHBUTTON;
}
