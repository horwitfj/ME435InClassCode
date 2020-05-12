#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

#define LINE0 0
#define LINE1 1
#define PIN_POT 0

#define PIN_PUSHBUTTON_GREEN 2
#define PIN_PUSHBUTTON_YELLOW 3
#define PIN_PUSHBUTTON_BLUE 4

#define PIN_LED_GREEN 6
#define PIN_LED_YELLOW 7

volatile uint8_t mainEventFlags = 0;
#define FLAG_YELLOW_PUSHBUTTON 0x01
#define FLAG_GREEN_PUSHBUTTON 0x02

int pot;
int yellowCount = 0;
int greenCount = 0;
unsigned nowTime = millis();
unsigned lastTime = nowTime;

uint8_t blueState = HIGH;
uint8_t lastBlueState = HIGH;
uint8_t greenState = HIGH;
uint8_t lastGreenState = HIGH;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Foster Horwitz");

  pinMode(PIN_PUSHBUTTON_YELLOW, INPUT_PULLUP);
  pinMode(PIN_PUSHBUTTON_GREEN, INPUT_PULLUP);
  pinMode(PIN_PUSHBUTTON_BLUE, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(PIN_PUSHBUTTON_YELLOW), yellow_pushbutton_isr, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_PUSHBUTTON_GREEN), green_pushbutton_isr, FALLING);

  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  digitalWrite(PIN_LED_YELLOW, LOW);
  digitalWrite(PIN_LED_GREEN, LOW);
}

void loop() {
  nowTime = millis();
  pot = analogRead(PIN_POT);
  lcd.setCursor(0, 1);
  lcd.print("Y=");
  lcd.print(yellowCount);
  lcd.print(" G=");
  lcd.print(greenCount);
  lcd.print(" T=");
  lcd.print(nowTime/1000);
  if (nowTime - lastTime > 2000) {
    Serial.print("Pot =");
    Serial.println(pot);
    Serial.print(" \n");
    lastTime = nowTime;
  }

  if (mainEventFlags & FLAG_YELLOW_PUSHBUTTON) {
    delay(30);
    mainEventFlags &= ~FLAG_YELLOW_PUSHBUTTON;
    if (!digitalRead(PIN_PUSHBUTTON_YELLOW)) {
      // Do the action!
      Serial.print("Pressed Yellow\n");
      yellowCount++;
      digitalWrite(PIN_LED_YELLOW, HIGH);
      digitalWrite(PIN_LED_GREEN, LOW);
    }
  }

  if (mainEventFlags & FLAG_GREEN_PUSHBUTTON) {
    delay(30);
    mainEventFlags &= ~FLAG_GREEN_PUSHBUTTON;
    if (!digitalRead(PIN_PUSHBUTTON_GREEN)) {
      // Do the action!
      Serial.print("Pressed Green\n");
      greenCount++;
      digitalWrite(PIN_LED_YELLOW, LOW);
      digitalWrite(PIN_LED_GREEN, HIGH);
    }
  }

  blueState = digitalRead(PIN_PUSHBUTTON_BLUE);
  if (blueState != lastBlueState) {
    if (!blueState) {
      yellowCount = 0;      
      greenCount = 0;
      digitalWrite(PIN_LED_YELLOW, LOW);
      digitalWrite(PIN_LED_GREEN, LOW);
    }
    delay(50);
  }
  lastBlueState = blueState;
}


void yellow_pushbutton_isr() {
  mainEventFlags |= FLAG_YELLOW_PUSHBUTTON;
}

void green_pushbutton_isr() {
  mainEventFlags |= FLAG_GREEN_PUSHBUTTON;
}