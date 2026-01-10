#include <LiquidCrystal.h>

// Pin Definitions
const int buttonPins[] = {6, 7, 8, 9, 10}; // Buttons connected to pins 6 to 10
const int numButtons = 5; // Number of buttons

// LCD setup
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // RS, E, D4, D5, D6, D7

// Game Variables
int correctButton; // Stores the index of the correct button
int timer = 30; // Game timer (30 seconds)
unsigned long previousMillis = 0; // For controlling the timer update interval
const long interval = 1000; // 1 second interval
bool gamePaused = false; // Flag to control the game pause state
bool firstBoot = true; // Flag to track if it's the first boot

enum DisplayState { PRESS_ANY_BUTTON, TIMER, GAME_OVER, YOU_WON };
DisplayState currentState = PRESS_ANY_BUTTON;

// Function declarations
void displayTime();
void resetGame();
void displayPressAnyButton();
void displayGameOver();
void displayYouWon();

// Setup
void setup() {
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP); // Set button pins as input using internal pull-up resistors
  }

  lcd.begin(16, 2); // Initialize the LCD (16x2)
  randomSeed(analogRead(0)); // Initialize random number generator
  resetGame(); // Start the game in the initial state
}

// Main game loop
void loop() {
  unsigned long currentMillis = millis();

  if (currentState == PRESS_ANY_BUTTON) {
    for (int i = 0; i < numButtons; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        if (firstBoot) {
          // If it's the first boot, show "Press any button" screen and wait for input
          firstBoot = false; // Set firstBoot to false after the first button press
        }
        currentState = TIMER; // Move to the timer state
        lcd.clear();
        displayTime();
        return;
      }
    }
  }

  if (currentState == TIMER) {
    for (int i = 0; i < numButtons; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        delay(200); // Simple debounce

        if (i == correctButton) {
          currentState = YOU_WON;
          gamePaused = true;
          displayYouWon();
          return;
        } else {
          timer -= 10; //Timer decrease
          if (timer < 0) {
            timer = 0;
          }
          displayTime();
        }
      }
    }

    if (timer <= 0) {
      currentState = GAME_OVER;
      gamePaused = true;
      displayGameOver();
      return;
    }

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      timer--;
      displayTime();
    }
  }

  if (gamePaused) {
    for (int i = 0; i < numButtons; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        delay(200); // Simple debounce
        resetGame();
        return;
      }
    }
  }
}

void displayTime() {
  lcd.setCursor(0, 0);
  lcd.print("Time left:");
  lcd.setCursor(0, 1);
  lcd.print(String(timer) + "s      "); // Clear extra characters
}

void resetGame() {
  gamePaused = false;
  if (firstBoot) {
    currentState = PRESS_ANY_BUTTON; // Start with "Press any button" on first boot
    displayPressAnyButton();
  } else {
    currentState = TIMER; // Skip "Press any button" and move directly to TIMER
    lcd.clear();
    displayTime();
  }
  timer = 30;
  correctButton = random(0, numButtons); // Choose a new random button
}

void displayPressAnyButton() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Start The Game");
}

void displayGameOver() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over!");
  lcd.setCursor(0, 1);
  lcd.print("Press to retry");
}

void displayYouWon() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Won!");
  lcd.setCursor(0, 1);
  lcd.print("Press to retry");
}
