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

// Additional variables for the cheat
int buttonPressCount[5] = {0, 0, 0, 0, 0}; // To count button presses

enum DisplayState { PRESS_ANY_BUTTON, TIMER, GAME_OVER, YOU_WON, CHEAT };
DisplayState currentState = PRESS_ANY_BUTTON;

// Function declarations
void displayTime();
void resetGame();
void displayPressAnyButton();
void displayGameOver();
void displayYouWon();
void displayCheat();

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

        // Cheat condition: if the same button is pressed 3 times in a row
        buttonPressCount[i]++;
        if (buttonPressCount[i] == 3) {
          currentState = CHEAT;
          displayCheat();
          return;
        }

        // Regular game logic
        if (i == correctButton) {
          currentState = YOU_WON;
          gamePaused = true;
          displayYouWon();
          return;
        } else {
          timer -= 10; // Timer decrease
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
  currentState = PRESS_ANY_BUTTON; // Start with "Press any button"
  lcd.clear();
  displayPressAnyButton();
  timer = 30;
  correctButton = random(0, numButtons); // Choose a new random button
  for (int i = 0; i < numButtons; i++) {
    buttonPressCount[i] = 0; // Reset button press counts
  }
}

void displayPressAnyButton() {
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

void displayCheat() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Nice Try!"); // Showing the cheat message
  lcd.setCursor(0, 1);
  lcd.print("You won (cheat)");
  delay(2000); // Show the message for 2 seconds
  resetGame();
}
