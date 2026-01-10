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

// Cheat Mode Variables
int lastButton = -1; // Last button pressed
int consecutivePresses = 0; // Count of consecutive presses of the same button
unsigned long lastPressTime = 0; // Timestamp of the last button press
const unsigned long cheatWindow = 1000; // Time window (in ms) for consecutive presses

enum DisplayState { PRESS_ANY_BUTTON, TIMER, GAME_OVER, YOU_WON, NICE_TRY, NO_WIN };
DisplayState currentState = PRESS_ANY_BUTTON;

// Function declarations
void displayTime();
void resetGame();
void displayPressAnyButton();
void displayGameOver();
void displayYouWon();
void displayNiceTry();
void displayNoWin();

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

        // Cheat Mode Logic
        if (i == lastButton && currentMillis - lastPressTime <= cheatWindow) {
          consecutivePresses++;
        } else {
          consecutivePresses = 1; // Reset count if a different button is pressed or too much time has passed
        }
        lastButton = i;
        lastPressTime = currentMillis;

        if (consecutivePresses == 3) {
          currentState = NICE_TRY; // Trigger cheat mode
          displayNiceTry();
          return;
        }

        // Check for winning condition
        if (i == correctButton) {
          currentState = YOU_WON;
          gamePaused = true;
          displayYouWon();
          return;
        } else if (correctButton == -1) { // No correct button case
          currentState = NO_WIN;
          displayNoWin();
          return;
        } else {
          timer -= 10; // Timer decreases
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

  // Handle "You Won" or "Nice Try" states
  if (currentState == YOU_WON || currentState == NICE_TRY) {
    for (int i = 0; i < numButtons; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        delay(200); // Simple debounce
        resetGame(); // Reset the game
        return;
      }
    }
  }

  // Handle "No Win" page
  if (currentState == NO_WIN) {
    for (int i = 0; i < numButtons; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        delay(200); // Simple debounce
        resetGame(); // Reset the game
        return;
      }
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

// Display the remaining time
void displayTime() {
  lcd.setCursor(0, 0);
  lcd.print("Time left:");
  lcd.setCursor(0, 1);
  lcd.print(String(timer) + "s      "); // Clear extra characters
}

// Reset the game state
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

  // Introduce a chance for no correct button
  if (random(0, 101) < 20) { //20% here
    correctButton = -1; // No correct button
  } else {
    correctButton = random(0, numButtons); // Choose a new random button
  }

  // Reset cheat mode variables
  lastButton = -1;
  consecutivePresses = 0;
  lastPressTime = 0;
}

// Display "Press any button" message
void displayPressAnyButton() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Start The Game");
}

// Display "Game Over" message
void displayGameOver() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over!");
  lcd.setCursor(0, 1);
  lcd.print("Press to retry");
}

// Display "You Won" message
void displayYouWon() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Won!");
  lcd.setCursor(0, 1);
  lcd.print("Press to retry");
}

// Display "Nice Try" message
void displayNiceTry() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Nice try!");
  delay(2000); // Show "Nice try" for 2 seconds
  currentState = YOU_WON; // Transition to "You Won" state
  displayYouWon();
}

// Display "No Win" message
void displayNoWin() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("OOOPS"!");
  lcd.setCursor(0, 1);
  lcd.print("Press to retry");
}
