#include <EnableInterrupt.h>

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
#define FLAG_RED_PUSHBUTTON 0x01
#define FLAG_YELLOW_PUSHBUTTON 0x02
#define FLAG_GREEN_PUSHBUTTON 0x04
#define FLAG_BLUE_PUSHBUTTON 0x08

int photoReading;
int potReading;
String inputString = "";               // a String to hold incoming data
volatile bool stringComplete = false;  // whether the string is complete

void setup() {
  Serial.begin(9600);
  inputString.reserve(200);
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

  enableInterrupt(PIN_PUSHBUTTON_RED, red_pushbutton_isr, FALLING);
  enableInterrupt(PIN_PUSHBUTTON_YELLOW, yellow_pushbutton_isr, FALLING);
  enableInterrupt(PIN_PUSHBUTTON_GREEN, green_pushbutton_isr, FALLING);
  enableInterrupt(PIN_PUSHBUTTON_BLUE, blue_pushbutton_isr, FALLING);
}

void loop() {
  if (stringComplete) {
    // Do stuff with the message
    if (inputString.equalsIgnoreCase("ON")) {
      digitalWrite(LED_BUILTIN, HIGH);
    } else if (inputString.equalsIgnoreCase("OFF")) {
      digitalWrite(LED_BUILTIN, LOW);
    } else if (inputString.equalsIgnoreCase("ADC")) {
      potReading = analogRead(PIN_ANALOG_POT);
      photoReading = analogRead(PIN_ANALOG_PHOTOCELL);
      Serial.print("POT ");
      Serial.println(potReading);
      Serial.print("PHOTO ");
      Serial.println(photoReading);
    } else if (inputString.equalsIgnoreCase("RON")) {
      digitalWrite(PIN_LED_RED, HIGH);
    } else if (inputString.equalsIgnoreCase("YON")) {
      digitalWrite(PIN_LED_YELLOW, HIGH);
    } else if (inputString.equalsIgnoreCase("GON")) {
      digitalWrite(PIN_LED_GREEN, HIGH);
    } else if (inputString.equalsIgnoreCase("BON")) {
      digitalWrite(PIN_LED_BLUE, HIGH);
    } else if (inputString.equalsIgnoreCase("ROFF")) {
      digitalWrite(PIN_LED_RED, LOW);
    } else if (inputString.equalsIgnoreCase("YOFF")) {
      digitalWrite(PIN_LED_YELLOW, LOW);
    } else if (inputString.equalsIgnoreCase("GOFF")) {
      digitalWrite(PIN_LED_GREEN, LOW);
    } else if (inputString.equalsIgnoreCase("BOFF")) {
      digitalWrite(PIN_LED_BLUE, LOW);
    } else if (inputString.equalsIgnoreCase("Off")) {
      rgb(0);
    } else if (inputString.equalsIgnoreCase("Blue")) {
      rgb(1);
    } else if (inputString.equalsIgnoreCase("Green")) {
      rgb(2);
    } else if (inputString.equalsIgnoreCase("Cyan")) {
      rgb(3);
    } else if (inputString.equalsIgnoreCase("Red")) {
      rgb(4);
    } else if (inputString.equalsIgnoreCase("Purple")) {
      rgb(5);
    } else if (inputString.equalsIgnoreCase("Yellow")) {
      rgb(6);
    } else if (inputString.equalsIgnoreCase("White")) {
      rgb(7);
    }

    //Prepare for the next message
    inputString = "";
    stringComplete = false;
  }

  if (mainEventFlags & FLAG_RED_PUSHBUTTON) {
    delay(30);
    mainEventFlags &= ~FLAG_RED_PUSHBUTTON;
    if (!digitalRead(PIN_PUSHBUTTON_RED)) {
      Serial.println("RBUT");
    }
  }
  if (mainEventFlags & FLAG_YELLOW_PUSHBUTTON) {
    delay(30);
    mainEventFlags &= ~FLAG_YELLOW_PUSHBUTTON;
    if (!digitalRead(PIN_PUSHBUTTON_YELLOW)) {
      Serial.println("YBUT");
    }
  }

  if (mainEventFlags & FLAG_GREEN_PUSHBUTTON) {
    delay(30);
    mainEventFlags &= ~FLAG_GREEN_PUSHBUTTON;
    if (!digitalRead(PIN_PUSHBUTTON_GREEN)) {
      Serial.println("GBUT");
    }
  }

  if (mainEventFlags & FLAG_BLUE_PUSHBUTTON) {
    delay(30);
    mainEventFlags &= ~FLAG_BLUE_PUSHBUTTON;
    if (!digitalRead(PIN_PUSHBUTTON_BLUE)) {
      Serial.println("BBUT");
    }
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }
}

void rgb(int val) {
  digitalWrite(PIN_RGB_BLUE, val % 2);
  digitalWrite(PIN_RGB_GREEN, val / 2 % 2);
  digitalWrite(PIN_RGB_RED, val / 4 % 2);
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
