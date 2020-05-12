#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

#define LINE_0 0
#define LINE_1 1
#define PIN_POT 0

#define PIN_PUSHBUTTON_GREEN 2
#define PIN_PUSHBUTTON_YELLOW 3
#define PIN_PUSHBUTTON_BLUE 4
#define PIN_LED_GREEN 6
#define PIN_LED_YELLOW 7

volatile int mainEventFlags = 0;
#define FLAG_YELLOW_PUSHBUTTON 0x01
#define FLAG_GREEN_PUSHBUTTON 0x02


unsigned long yellowTimerMs;
uint8_t isYellowTimerRunning = 0;
unsigned long greenTimerMs;
uint8_t isGreenTimerRunning = 0;
unsigned long priorTimeMs = 0;

void setup() {
  lcd.begin(16, 2);

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
      yellowTimerMs = 0;
      isGreenTimerRunning = 0;
      greenTimerMs = 0;
    }
  unsigned long currentTimeMs = millis();
  unsigned long elapsedTimeMs = currentTimeMs - priorTimeMs;
  priorTimeMs = currentTimeMs;
  
  if(isYellowTimerRunning) {
    yellowTimerMs = yellowTimerMs + elapsedTimeMs;
  }
  if(isGreenTimerRunning) {
    greenTimerMs = greenTimerMs + elapsedTimeMs;
  }
  
  updateLcd();
    delay(100);
  
}

void updateLcd(){
  lcd.setCursor(0, LINE_1);
  lcd.print(yellowTimerMs / 1000);
  lcd.print(".");
  lcd.print(yellowTimerMs / 100 % 10);
  
  lcd.setCursor(0, LINE_0);
  lcd.print(greenTimerMs / 1000);
  lcd.print(".");
  lcd.print(greenTimerMs / 100 % 10);
  //lcd.print("    ");
}


void yellow_pushbutton_isr() {
  mainEventFlags |= FLAG_YELLOW_PUSHBUTTON;
}

void green_pushbutton_isr() {
  mainEventFlags |= FLAG_GREEN_PUSHBUTTON;
}