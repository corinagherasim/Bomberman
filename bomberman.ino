#include "LedControl.h"

#include <LiquidCrystal.h>

#include <EEPROM.h>

#include "pitches.h"


const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 3;
const byte d7 = 4;
const byte v0 = 5;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int lcdWidth = 16;

//all of these below are from hw6
const int pinSW = 2;
const int pinX = A0;
const int pinY = A1;

byte swState = LOW;
byte lastSwState = LOW;

int xValue = 0;
int yValue = 0;
int lastYValue = 0;
int lastXValue = 0;

const int minThresholdCenter = 350;
const int maxThresholdCenter = 700;
const int minThresholdMargin = 200;
const int maxThresholdMargin = 900;

const int ledPin = A2;
const int buzzerPin = 13;
const int buttonPin = A3;

byte buttonState = LOW;
byte lastButtonState = LOW;

bool joystickMove = true;
bool lastJoystickMove = false;
bool currentJoystickMove = false;

unsigned long lastDebounceTimeJoy = 0;
unsigned long debounceDelayJoy = 200;

unsigned long lastDebounceTimeButton = 0;
unsigned long debounceDelayButton = 50;

int playerRow = 0;
int playerCol = 0;
int bombRow = 0;
int bombCol = 0;

unsigned long lastBlinkTimePlayer = 0;
unsigned long blinkIntervalPlayer = 500;
bool isPlayerLedOn = true;

unsigned long lastBlinkTimeBomb = 0;
unsigned long blinkIntervalBomb = 100;
bool isBombLedOn = true;
unsigned long bombStartTime = 0;
unsigned long bombDelay = 3000;
unsigned long elapsedTime;
bool bombActive = false;

bool gameInProgress = false;

int n = 0;
int lives = 3;
int set = 0;
bool soundOnOrOff = true;
int lcdBrightness = 0;
int matrixBrightness = 0;
bool gameFinished = false;
int highscoreMess = 0;

unsigned long gameStartTime = 0;

const int HIGHSCORE_COUNT = 5;

char playerName[4] = "AAA";
const char alphabet[27] = {
  'A',
  'B',
  'C',
  'D',
  'E',
  'F',
  'G',
  'H',
  'I',
  'J',
  'K',
  'L',
  'M',
  'N',
  'O',
  'P',
  'Q',
  'R',
  'S',
  'T',
  'U',
  'V',
  'W',
  'X',
  'Y',
  'Z'
};

unsigned long currentScore;

struct HighScore {
  unsigned long score;
  char playerName[4];
};

HighScore highscores[HIGHSCORE_COUNT] = {
  {
    200,
    "AAA"
  },
  {
    200,
    "AAA"
  },
  {
    200,
    "AAA"
  },
  {
    200,
    "AAA"
  },
  {
    200,
    "AAA"
  }
};

const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte matrixSize = 8;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);

bool walls[matrixSize][matrixSize];

const int lcdBrightnessAddr = 0; // EEPROM address for storing LCD brightness
const int matrixBrightnessAddr = 1; // EEPROM address for storing matrix brightness
const int soundStateAddr = 2; // EEPROM address for storing if the sound is on or off
const int playerNameAddr = 15; // EEPROM address for storing the name of the player

//inspired by surdubob Github
enum menuStates {
  START_GAME,
  HIGHSCORES,
  SETTINGS,
  ABOUT,
  HOW_TO_PLAY
}
menuState;

enum menuStatesSettings {
  NAME,
  LCD,
  MATRIX,
  SOUND,
  RESET_HIGHSCORES,
  BACK
}
menuStateSettings;

// theme song
int melody[] = {

  NOTE_C4,
  NOTE_G3,
  NOTE_G3,
  NOTE_A3,
  NOTE_G3,
  0,
  NOTE_B3,
  NOTE_C4
};

int noteDurations[] = {

  4,
  8,
  8,
  4,
  4,
  4,
  4,
  4
};

// drawings on LCD
byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  0b00000
};

byte arrowRight[8] = {
  0b00000,
  0b00000,
  0b00100,
  0b00110,
  0b11111,
  0b00110,
  0b00100,
  0b00000
};

byte arrowLeft[8] = {
  0b00000,
  0b00000,
  0b00100,
  0b01100,
  0b11111,
  0b01100,
  0b00100,
  0b00000
};

byte arrowUp[8] = {
  0b00000,
  0b00100,
  0b01110,
  0b11111,
  0b00100,
  0b00100,
  0b00100,
  0b00000
};

byte arrowDown[8] = {
  0b00000,
  0b00100,
  0b00100,
  0b00100,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

byte soundOn[8] = {
  0b00000,
  0b00000,
  0b11111,
  0b11101,
  0b11101,
  0b11111,
  0b00000,
  0b00000
};

byte soundOff[8] = {
  0b00000,
  0b00000,
  0b11111,
  0b10111,
  0b10111,
  0b11111,
  0b00000,
  0b00000
};

void setup() {
  Serial.begin(9600);
  pinMode(pinSW, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  analogWrite(v0, 100);
  lcd.begin(16, 2);

  lc.shutdown(0, false);
  lc.setIntensity(0, 8);

  lcd.createChar(0, heart); //used for lives
  lcd.createChar(1, arrowRight); //used for menu
  lcd.createChar(2, arrowLeft); //used for menu
  lcd.createChar(3, arrowUp); //used for menu
  lcd.createChar(4, arrowDown); //used for menu
  lcd.createChar(5, soundOn); //used for changing sound
  lcd.createChar(6, soundOff); //used for changing sound

  // reading the values saved in EEPROM and showing them
  lcdBrightness = EEPROM.read(lcdBrightnessAddr);
  analogWrite(v0, lcdBrightness);

  matrixBrightness = EEPROM.read(matrixBrightnessAddr);
  lc.setIntensity(0, matrixBrightness);

  soundOnOrOff = EEPROM.read(soundStateAddr);

  for (int k = 0; k < 3; ++k) {
    playerName[k] = EEPROM.read(playerNameAddr + k);
  }

  for (int i = 0; i < HIGHSCORE_COUNT; ++i) {
    highscores[i].score = EEPROM.read((4 + i) * sizeof(HighScore));
    for (int j = 0; j < sizeof(highscores[i].playerName); ++j) {
      highscores[i].playerName[j] = EEPROM.read((4 + i) * sizeof(HighScore) + sizeof(highscores[i].score) + j);
    }
  }

  centerTextOnLcd(F("Welcome to"), 0);
  centerTextOnLcd(F("Bomberman"), 1);
  if (soundOnOrOff == true) {
    themeSong();
  } else {
    delay(3000);
  }

  lcd.clear();

  menuState = START_GAME;
  menuStateSettings = NAME;

}

void loop() {
  readJoystickValues();
  //if the game is not in progress it shows the menu but if it is it uses the functions for the game and it shows on the lcd the lives i have left to play (maximum 3 and when at 0 the game stops)
  if (!gameInProgress) {
    // set checks in what menu we are if set = 1 we are in the settings menu and controls that
    if (set == 1) {
      joystickMoveMenuSettings();
      checkButtonPressSettings();
    } else {
      joystickMoveMenu();
      checkButtonPress();
    }
  } else {
    lcd.clear();
    int startColumn = (lcdWidth - lives) / 2;
    unsigned long currentTime = millis();
    unsigned long elapsedTimeInSeconds = (currentTime - gameStartTime) / 1000;
    centerTextOnLcd(String("Score: ") + String(elapsedTimeInSeconds), 0);
    for (int i = 0; i < lives; i++) {
      lcd.setCursor(startColumn + i, 1);
      lcd.write((byte) 0);
    }
    delay(100);
    joystickMoveMatrix();
    bomb();
    stopGame();
    currentScore = elapsedTimeInSeconds;
    elapsedTimeInSeconds = 0;
    if (currentScore == 200) {
      lcd.clear();
      displayX();
      centerTextOnLcd(F("Game over!"), 0);
      delay(3000);
      gameFinished = false;
      gameInProgress = false;
      lives = 3;
    }
  }
}

void readJoystickValues() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
  swState = digitalRead(pinSW);
}

// printing main menu depending on the menu state which can be changed with the joystick
// the arrows are added depending the directions you can move to from the current position
void mainMenu(int menuState) {
  lcd.clear();
  switch (menuState) {
  case START_GAME: {
    displayMaze();
    lcd.setCursor(1, 0);
    centerTextOnLcdMenu(F("Start game"), 0, false, true);
    break;
  }

  case HIGHSCORES: {
    displayCup();
    lcd.setCursor(1, 0);
    centerTextOnLcdMenu(F("Leaderboard"), 0, true, true);
    break;
  }

  case SETTINGS: {
    displaySettingsLogo();
    lcd.setCursor(1, 0);
    centerTextOnLcdMenu(F("Settings"), 0, true, true);
    break;
  }

  case HOW_TO_PLAY: {
    displayQuestionMark();
    lcd.setCursor(1, 0);
    centerTextOnLcdMenu(F("How to play"), 0, true, true);
    break;
  }

  case ABOUT: {
    displayGithubLogo();
    lcd.setCursor(1, 0);
    centerTextOnLcdMenu(F("About"), 0, true, false);
    break;
  }
  }
}

void menuSettings(int menuStateSettings) {
  displaySettingsLogo();
  lcd.clear();
  switch (menuStateSettings) {
  case NAME: {
    lcd.setCursor(1, 0);
    centerTextOnLcdMenu(F("Choose name"), 1, false, true);
    break;
  }

  case LCD: {
    lcd.setCursor(1, 0);
    centerTextOnLcdMenu(F("Contrast"), 0, false, false);
    centerTextOnLcdMenu(F("LCD"), 1, true, true);
    break;
  }

  case MATRIX: {
    lcd.setCursor(1, 0);
    centerTextOnLcdMenu(F("Brightness"), 0, false, false);
    centerTextOnLcdMenu(F("Matrix"), 1, true, true);
    break;
  }

  case SOUND: {
    lcd.setCursor(1, 0);
    centerTextOnLcdMenu(F("Sound"), 0, false, false);
    if (soundOnOrOff == false) {
      lcd.setCursor(5, 1);
      lcd.print("ON");
      lcd.write(byte(6));
      lcd.print("OFF");
      lcd.setCursor(0, 1);
      lcd.write(byte(2));
      lcd.setCursor(lcdWidth - 1, 1);
      lcd.write(byte(1));
    }
    if (soundOnOrOff == true) {
      lcd.setCursor(5, 1);
      lcd.print("ON");
      lcd.write(byte(5));
      lcd.print("OFF");
      lcd.setCursor(0, 1);
      lcd.write(byte(2));
      lcd.setCursor(lcdWidth - 1, 1);
      lcd.write(byte(1));
    }
    break;
  }

  case RESET_HIGHSCORES: {
    lcd.setCursor(1, 0);
    centerTextOnLcdMenu(F("Reset"), 0, false, false);
    centerTextOnLcdMenu(F("Leaderboard"), 1, true, true);
    break;
  }

  case BACK: {
    lcd.setCursor(1, 0);
    centerTextOnLcdMenu(F("Back to"), 0, false, false);
    centerTextOnLcdMenu(F("main menu"), 1, true, false);
    break;
  }
  }
}

// checks if the button is pressed on the menu and if it is i put what it's supposed to do
// when moving the menu there are specific sounds added
// there is the same logic on the settings menu and on the main menu
void checkButtonPress() {
  if (swState != lastSwState) {
    if (swState == LOW && (millis() - lastDebounceTimeButton > debounceDelayButton)) {
      menuChoosingSound();
      switch (menuState) {
      case START_GAME:
        Serial.println("Button pressed in START_GAME state");
        lcd.setCursor(1, 0);
        lcd.write(byte(1));
        centerTextOnLcd(F("Game starting..."), 0);
        delay(2000);
        gameInProgress = true;
        gameStartTime = millis();
        generateRandomMap();
        resetPlayer();
        readJoystickValues();
        joystickMoveMatrix();
        bomb();
        break;

      case HIGHSCORES:
        Serial.println("Button pressed in HIGHSCORES state");
        displayHighScores();
        break;

      case SETTINGS:
        Serial.println("Button pressed in SETTINGS state");
        lcd.clear();
        centerTextOnLcd(F("Loading"), 0);
        centerTextOnLcd(F("settings..."), 1);
        delay(2000);
        set = 1;
        break;

      case HOW_TO_PLAY:
        Serial.println("Button pressed in HOW_TO_PLAY state");
        lcd.clear();
        centerTextOnLcd(F("Move player"), 0);
        centerTextOnLcd(F("with joystick"), 1);
        delay(2000);
        lcd.clear();
        centerTextOnLcd(F("To plant bombs"), 0);
        centerTextOnLcd(F("press joystick"), 1);
        delay(2000);
        lcd.clear();
        centerTextOnLcd(F("Bomb destroys up,"), 0);
        centerTextOnLcd(F("down.left,right"), 1);
        delay(2000);
        lcd.clear();
        centerTextOnLcd(F("Bomb kills you"), 0);
        centerTextOnLcd(F("when led on"), 1);
        delay(2000);
        lcd.clear();
        centerTextOnLcd(F("You have"), 0);
        centerTextOnLcd(F("3 lives"), 1);
        delay(2000);
        lcd.clear();
        centerTextOnLcd(F("Winning point"), 0);
        centerTextOnLcd(F("top left corner"), 1);
        delay(2000);
        lcd.clear();
        centerTextOnLcd(F("To win you do"), 0);
        centerTextOnLcd(F("the map 2 times"), 1);
        delay(2000);
        break;

      case ABOUT:
        Serial.println("Button pressed in ABOUT state");
        lcd.setCursor(1, 0);
        displayScrollingText(F("Bomberman made by Corina Gherasim"), 0);
        centerTextOnLcd(F("Github user:"), 0);
        centerTextOnLcd(F("corinagherasim"), 1);
        delay(2000);
        break;
      }
      lastDebounceTimeButton = millis();
    }
    lastSwState = swState;
  }
}

void checkButtonPressSettings() {
  if (swState != lastSwState) {
    if (swState == LOW && (millis() - lastDebounceTimeButton > debounceDelayButton)) {
      menuChoosingSound();
      switch (menuStateSettings) {
      case NAME:
        Serial.println("Button pressed in NAME state");
        chooseName();
        break;

      case LCD:
        Serial.println("Button pressed in LCD state");
        adjustBrightnessLCD();
        break;

      case MATRIX:
        Serial.println("Button pressed in MATRIX state");
        adjustBrightnessMatrix();
        break;

      case SOUND:
        Serial.println("Button pressed in SOUND state");
        soundOnOrOff = !soundOnOrOff;
        EEPROM.update(soundStateAddr, soundOnOrOff);
        break;

      case RESET_HIGHSCORES:
        Serial.println("Button pressed in RESET_HIGHSCORES state");
        lcd.clear();
        resetHighscores();
        centerTextOnLcd(F("Reseting"), 0);
        centerTextOnLcd(F("leaderboard..."), 1);
        delay(2000);
        break;

      case BACK:
        Serial.println("Button pressed in BACK state");
        menuStateSettings = NAME;
        menuState = SETTINGS;
        set = 0;
        break;
      }
      lastDebounceTimeButton = millis();
    }
    lastSwState = swState;
  }
}

// moving the menu with the joystick and changing the menu states depending on the current menu state
// there is the same logic on the settings menu and on the main menu
void joystickMoveMenu() {
  if ((minThresholdCenter <= xValue && xValue <= maxThresholdCenter) && (minThresholdCenter <= yValue && yValue <= maxThresholdCenter)) {
    joystickMove = true;
  }

  if (xValue > maxThresholdMargin && joystickMove) {
    if (menuState == START_GAME) {
      menuMovingSound();
      menuState = HIGHSCORES;
    } else if (menuState == HIGHSCORES) {
      menuMovingSound();
      menuState = SETTINGS;
    } else if (menuState == SETTINGS) {
      menuMovingSound();
      menuState = HOW_TO_PLAY;
    } else if (menuState == HOW_TO_PLAY) {
      menuMovingSound();
      menuState = ABOUT;
    }
    joystickMove = false;
  }

  if (xValue < minThresholdMargin && joystickMove) {
    if (menuState == ABOUT) {
      menuMovingSound();
      menuState = HOW_TO_PLAY;
    } else if (menuState == HOW_TO_PLAY) {
      menuMovingSound();
      menuState = SETTINGS;
    } else if (menuState == SETTINGS) {
      menuMovingSound();
      menuState = HIGHSCORES;
    } else if (menuState == HIGHSCORES) {
      menuMovingSound();
      menuState = START_GAME;
    }
    joystickMove = false;
  }

  mainMenu(menuState);

  if (lastJoystickMove != joystickMove) {
    lastDebounceTimeJoy = millis();
  }

  if (millis() - lastDebounceTimeJoy > debounceDelayJoy) {
    if (joystickMove != currentJoystickMove) {
      currentJoystickMove = joystickMove;
    }
  }

  lastJoystickMove = joystickMove;
  delay(50);
}

void joystickMoveMenuSettings() {
  if ((minThresholdCenter <= xValue && xValue <= maxThresholdCenter) && (minThresholdCenter <= yValue && yValue <= maxThresholdCenter)) {
    joystickMove = true;
  }

  if (xValue > maxThresholdMargin && joystickMove) {
    if (menuStateSettings == NAME) {
      menuMovingSound();
      menuStateSettings = LCD;
    } else if (menuStateSettings == LCD) {
      menuMovingSound();
      menuStateSettings = MATRIX;
    } else if (menuStateSettings == MATRIX) {
      menuMovingSound();
      menuStateSettings = SOUND;
    } else if (menuStateSettings == SOUND) {
      menuMovingSound();
      menuStateSettings = RESET_HIGHSCORES;
    } else if (menuStateSettings == RESET_HIGHSCORES) {
      menuMovingSound();
      menuStateSettings = BACK;
    }
    joystickMove = false;
  }

  if (xValue < minThresholdMargin && joystickMove) {
    if (menuStateSettings == BACK) {
      menuMovingSound();
      menuStateSettings = RESET_HIGHSCORES;
    } else if (menuStateSettings == RESET_HIGHSCORES) {
      menuMovingSound();
      menuStateSettings = SOUND;
    } else if (menuStateSettings == SOUND) {
      menuMovingSound();
      menuStateSettings = MATRIX;
    } else if (menuStateSettings == MATRIX) {
      menuMovingSound();
      menuStateSettings = LCD;
    } else if (menuStateSettings == LCD) {
      menuMovingSound();
      menuStateSettings = NAME;
    }
    joystickMove = false;
  }

  menuSettings(menuStateSettings);

  if (lastJoystickMove != joystickMove) {
    lastDebounceTimeJoy = millis();
  }

  if (millis() - lastDebounceTimeJoy > debounceDelayJoy) {
    if (joystickMove != currentJoystickMove) {
      currentJoystickMove = joystickMove;
    }
  }

  lastJoystickMove = joystickMove;
  delay(50);
}

// with this we choose the name of the player that is saved on EEPROM and shown after finishing the game and in highscores
// to change the position of the letter we want to modify we move the joystick left and right
// to change the letter on the position selected we move the joystick up and down
// there are arrows displayed so that it indicates how to move to change the name
// there is also "^" under the position of the letter that is changed now
// to save the name and to go back to settings menu the button should be pressed
void chooseName() {
  lcd.clear();
  bool editingName = true;
  int cursorPosition = 0;
  int lastCursorPosition = 0;
  int lastXMove = 0;
  int lastYMove = 0;

  while (editingName) {
    centerTextOnLcd(String(playerName), 0);
    lcd.setCursor(6 + lastCursorPosition, 1);
    lcd.print(' ');
    lcd.setCursor(6 + cursorPosition, 1);
    lcd.print('^');
    lcd.setCursor(0, 1);
    lcd.write(byte(2));
    lcd.setCursor(lcdWidth - 1, 1);
    lcd.write(byte(1));
    lcd.setCursor(0, 0);
    lcd.write(byte(3));
    lcd.setCursor(lcdWidth - 1, 0);
    lcd.write(byte(4));

    readJoystickValues();

    int xMove = 0;

    if (xValue < minThresholdMargin && cursorPosition > 0) {
      xMove = -1;
    } else if (xValue > maxThresholdMargin && cursorPosition < 2) {
      xMove = 1;
    }

    if (xMove != lastXMove) {
      lastCursorPosition = cursorPosition;
      cursorPosition += xMove;
      delay(200);
    }

    lastXMove = xMove;

    int yMove = 0;

    if (yValue < minThresholdMargin) {
      yMove = 1;
    } else if (yValue > maxThresholdMargin) {
      yMove = -1;
    }

    if (yMove != lastYMove) {
      playerName[cursorPosition] = alphabet[(playerName[cursorPosition] - 'A' + yMove + 26) % 26];
      delay(200);
    }

    lastYMove = yMove;

    buttonState = digitalRead(buttonPin);

    if (buttonState != lastButtonState) {
      lastDebounceTimeButton = millis();
    }

    if (millis() - lastDebounceTimeButton > debounceDelayButton) {
      if (buttonState == LOW && !lastButtonState) {
        for (int k = 0; k < 3; ++k) {
          EEPROM.update(playerNameAddr + k, playerName[k]);
        }
        editingName = false;
        delay(500);
      }
    }

    lastButtonState = buttonState;
  }

  lcd.clear();
}

// the highscores are displayed with the place score and name
// if the score is 200 (the initial one) it is shown 0 without the name so it shows there are no games played
void displayHighScores() {
  for (int i = 0; i < HIGHSCORE_COUNT; ++i) {
    lcd.clear();
    centerTextOnLcd(F("Leaderboard:"), 0);
    if (highscores[i].score == 200) {
      centerTextOnLcd(String(i + 1) + String(". ") + String(0), 1);
    } else {
      centerTextOnLcd(String(i + 1) + String(". ") + String(highscores[i].score) + String(" - ") + String(highscores[i].playerName), i + 1);
    }
    delay(2000);
  }
}

// checks if the score is smaller than any of the scores saved as highscores and if the game is finished
// we also check gameFinished because to avoid being a highscore of someone losing very quick
int checkHighScore(unsigned long score) {
  for (int i = 0; i < HIGHSCORE_COUNT; ++i) {
    if (score < highscores[i].score && gameFinished == true) {
      return i;
    }
  }
  return -1; //it is not a highscore
}

void manageHighScores() {
  int index = checkHighScore(currentScore);
  if (index != -1) {
    lcd.clear();
    highscoreMess = 1;

    // we are creating a temporary structure so that we make space for the new highscore
    // we are saving the structures HighScore below index
    // we are then updating the score and the name of the player in tempHighScore
    // we are then putting tempHighScore on the index position and then writing HighScore structures in EEPROM
    HighScore tempHighScore;

    for (int i = HIGHSCORE_COUNT - 1; i > index; --i) {
      highscores[i] = highscores[i - 1];
    }

    strncpy(tempHighScore.playerName, playerName, sizeof(playerName));

    tempHighScore.score = currentScore;

    highscores[index] = tempHighScore;

    for (int i = 0; i < HIGHSCORE_COUNT; ++i) {
      EEPROM.put((4 + i) * sizeof(HighScore), highscores[i]);
    }
  }
}

// to reset highscores we change what is saved on the EEPROM with the initial values: 200 for score and "AAA" for playerName
// we do that because to be a highscore it should be a score smaller than what is saved and by giving such a high value it will always be smaller and "AAA" is the initial playerName
void resetHighscores() {
  for (int i = 0; i < HIGHSCORE_COUNT; ++i) {
    highscores[i].score = 200;
    strncpy(highscores[i].playerName, "AAA", sizeof("AAA"));
    EEPROM.update((4 + i) * sizeof(HighScore), highscores[i].score);

    for (int j = 0; j < sizeof(highscores[i].playerName); ++j) {
      EEPROM.update((4 + i) * sizeof(HighScore) + sizeof(highscores[i].score) + j, highscores[i].playerName[j]);
    }
  }
}

// changes the contrast of the LCD depending on some levels that have associated a value
// to change the levels you move the joystick up and down depending on the arrows that are shown
// while changing the levels it is shown also on the LCD
// after pressing the button the chosen value is saved on EEPROM
void adjustBrightnessLCD() {
  lcd.clear();
  centerTextOnLcd(F("Contrast"), 0);
  int lcdLevelBrightness;
  bool adjustmentCompleteLCD = false;

  if (lcdBrightness == 75) {
    lcdLevelBrightness = 1;
  } else if (lcdBrightness == 100) {
    lcdLevelBrightness = 2;
  } else if (lcdBrightness == 125) {
    lcdLevelBrightness = 3;
  }

  while (!adjustmentCompleteLCD) {
    lcd.setCursor(0, 1);
    lcd.print(lcdLevelBrightness);
    readJoystickValues();

    if (yValue < minThresholdMargin && lastYValue >= minThresholdMargin) {
      if (lcdLevelBrightness == 2) {
        lcdLevelBrightness = 1;
        lcdBrightness = 75;
      } else if (lcdLevelBrightness == 3) {
        lcdLevelBrightness = 2;
        lcdBrightness = 100;
      }
      analogWrite(v0, lcdBrightness);
    }

    if (yValue > maxThresholdMargin && lastYValue <= maxThresholdMargin) {
      if (lcdLevelBrightness == 1) {
        lcdLevelBrightness = 2;
        lcdBrightness = 100;
      } else if (lcdLevelBrightness == 2) {
        lcdLevelBrightness = 3;
        lcdBrightness = 125;
      }
      analogWrite(v0, lcdBrightness);
    }

    lcdBrightness = map(lcdLevelBrightness, 1, 3, 75, 125);

    lcd.setCursor(lcdWidth - 2, 1);
    for (int i = 1; i <= 3; i++) {
      if (i == lcdLevelBrightness - 1) {
        lcd.write(byte(3));
      } else if (i == lcdLevelBrightness) {
        lcd.write(byte(4));
      } else {
        lcd.print(" ");
      }
    }

    buttonState = digitalRead(buttonPin);

    if (buttonState != lastButtonState) {
      lastDebounceTimeButton = millis();
    }

    if (millis() - lastDebounceTimeButton > debounceDelayButton) {
      if (buttonState == LOW && !lastButtonState) {
        if (lcdLevelBrightness == 1) {
          lcdBrightness = 75;
        } else if (lcdLevelBrightness == 2) {
          lcdBrightness = 100;
        } else if (lcdLevelBrightness == 3) {
          lcdBrightness = 125;
        }
        analogWrite(v0, lcdBrightness);
        EEPROM.update(lcdBrightnessAddr, lcdBrightness);
        adjustmentCompleteLCD = true;
      }
    }

    lastButtonState = buttonState;
    lastYValue = yValue;
  }
  lcd.clear();
}

// changes the brightness of the matrix depending on some levels that have associated a brightness
// to change the levels you move the joystick up and down depending on the arrows that are shown
// while changing the levels it is shown also on the matrix
// after pressing the button the chosen brightness is saved on EEPROM
void adjustBrightnessMatrix() {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, HIGH);
    }
  }
  lcd.clear();
  centerTextOnLcd(F("Brightness"), 0);
  int matrixLevelBrightness;
  bool adjustmentCompleteMatrix = false;

  if (matrixBrightness == 2) {
    matrixLevelBrightness = 1;
  } else if (matrixBrightness == 6) {
    matrixLevelBrightness = 2;
  } else if (matrixBrightness == 10) {
    matrixLevelBrightness = 3;
  }

  while (!adjustmentCompleteMatrix) {
    lcd.setCursor(0, 1);
    lcd.print(matrixLevelBrightness);
    readJoystickValues();

    if (yValue < minThresholdMargin && lastYValue >= minThresholdMargin) {
      if (matrixLevelBrightness == 2) {
        matrixLevelBrightness = 1;
        matrixBrightness = 2;
      } else if (matrixLevelBrightness == 3) {
        matrixLevelBrightness = 2;
        matrixBrightness = 6;
      }
      lc.setIntensity(0, matrixBrightness);
    }

    if (yValue > maxThresholdMargin && lastYValue <= maxThresholdMargin) {
      if (matrixLevelBrightness == 1) {
        matrixLevelBrightness = 2;
        matrixBrightness = 6;
      } else if (matrixLevelBrightness == 2) {
        matrixLevelBrightness = 3;
        matrixBrightness = 10;
      }
      lc.setIntensity(0, matrixBrightness);
    }

    matrixBrightness = map(matrixLevelBrightness, 1, 3, 2, 10);
    Serial.print("matrixLevelBrightness: ");
    Serial.println(matrixLevelBrightness);

    lcd.setCursor(lcdWidth - 2, 1);
    for (int i = 1; i <= 3; i++) {
      if (i == matrixLevelBrightness - 1) {
        lcd.write(byte(3));
      } else if (i == matrixLevelBrightness) {
        lcd.write(byte(4));
      } else {
        lcd.print(" ");
      }
    }

    buttonState = digitalRead(buttonPin);

    if (buttonState != lastButtonState) {
      lastDebounceTimeButton = millis();
    }

    if (millis() - lastDebounceTimeButton > debounceDelayButton) {
      if (buttonState == LOW && !lastButtonState) {
        if (matrixLevelBrightness == 1) {
          matrixBrightness = 2;
        } else if (matrixLevelBrightness == 2) {
          matrixBrightness = 6;
        } else if (matrixLevelBrightness == 3) {
          matrixBrightness = 10;
        }
        lc.setIntensity(0, matrixBrightness);
        EEPROM.update(matrixBrightnessAddr, matrixBrightness);
        adjustmentCompleteMatrix = true;
      }
    }

    lastButtonState = buttonState;
    lastYValue = yValue;
  }
  lcd.clear();
}

//taken from surdubob Github
void centerTextOnLcd(String text, short line) {
  short l = text.length();
  short spaces = (lcdWidth - l) / 2;
  lcd.setCursor(spaces, line);
  lcd.print(text);
}

// used for menu to show the arrows that indicate the directions you can move
void centerTextOnLcdMenu(String text, short line, bool leftArrow, bool rightArrow) {
  short l = text.length();
  short spaces = (lcdWidth - l) / 2;
  lcd.setCursor(spaces, line);
  lcd.print(text);

  if (leftArrow) {
    lcd.setCursor(0, line);
    lcd.write(byte(2));
  }

  if (rightArrow) {
    lcd.setCursor(lcdWidth - 1, line);
    lcd.write(byte(1));
  }
}

void displayScrollingText(String text, short line) {
  short l = text.length();
  short spaces = (lcdWidth - l) / 2;
  for (short i = 0; i <= l; ++i) {
    lcd.setCursor(spaces, line);
    lcd.print(text.substring(i));
    delay(500);
    lcd.clear();
  }
}

//copied from homework #3 and changed so that it fits this homework
//apart from checking the movement of the joystick it also checks if the position you want to move the player to is in the walls array or not
void joystickMoveMatrix() {
  if ((minThresholdCenter <= xValue && xValue <= maxThresholdCenter) && (minThresholdCenter <= yValue && yValue <= maxThresholdCenter)) {
    joystickMove = false;
  }

  if (xValue > maxThresholdMargin && joystickMove == false && playerCol > 0 && !walls[playerRow][playerCol - 1]) {
    movePlayer(playerRow, playerCol, playerRow, playerCol - 1);
  }

  if (xValue < minThresholdMargin && joystickMove == false && playerCol < matrixSize - 1 && !walls[playerRow][playerCol + 1]) {
    movePlayer(playerRow, playerCol, playerRow, playerCol + 1);
  }

  if (yValue < minThresholdMargin && joystickMove == false && playerRow < matrixSize - 1 && !walls[playerRow + 1][playerCol]) {
    movePlayer(playerRow, playerCol, playerRow + 1, playerCol);
  }

  if (yValue > maxThresholdMargin && joystickMove == false && playerRow > 0 && !walls[playerRow - 1][playerCol]) {
    movePlayer(playerRow, playerCol, playerRow - 1, playerCol);
  }

  if (lastJoystickMove != joystickMove) {
    lastDebounceTimeJoy = millis();
  }

  if (millis() - lastDebounceTimeJoy > debounceDelayJoy) {
    if (joystickMove != currentJoystickMove) {
      currentJoystickMove = joystickMove;
    }

    if (currentJoystickMove == true) {
      lc.setLed(0, playerRow, playerCol, true);
    }
  }

  if (!currentJoystickMove) {
    // blink the player LED when not moving
    if (millis() - lastBlinkTimePlayer > blinkIntervalPlayer) {
      isPlayerLedOn = !isPlayerLedOn;
      lc.setLed(0, playerRow, playerCol, isPlayerLedOn);
      lastBlinkTimePlayer = millis();
    }
  }

  // if the player gets to left top corner the player levels up or finishes the game
  // after finishing the game it displays the name and the score and checks if it is a highscore
  // if the score is a new highscore it will show a message
  if (playerRow == matrixSize - 1 && playerCol == matrixSize - 1) {
    n += 1;
    if (n == 2) {
      displayCup();
      lcd.clear();
      centerTextOnLcd(String("Congrats, ") + String(playerName), 0);
      centerTextOnLcd(String("Score: ") + String(currentScore), 1);
      if (soundOnOrOff == true) {
        themeSong();
      } else {
        delay(3000);
      }
      gameFinished = true;
      manageHighScores();
      if (highscoreMess == 1) {
        centerTextOnLcd(F("You have a"), 0);
        centerTextOnLcd(F("new highscore!"), 1);
        delay(2000);
        highscoreMess = 0;
      }
      gameInProgress = false;
      n = 0;
      lives = 3;
    } else {
      menuChoosingSound();
      generateRandomMap();
      resetPlayer();
    }

  }

  lastJoystickMove = joystickMove;
}

//change the position of player
void movePlayer(int currentRow, int currentCol, int newRow, int newCol) {
  lc.setLed(0, currentRow, currentCol, false);
  lc.setLed(0, newRow, newCol, true);
  playerRow = newRow;
  playerCol = newCol;
  joystickMove = true;
}

void resetPlayer() {
  lc.setLed(0, 0, 0, true);
  playerRow = 0;
  playerCol = 0;
}

//when i press the button it makes a bomb on the position of the player that destroys the wall from above, below, left and right
//there is a buzzer that is on and the bomb is blinking for 3 seconds before exploding
//the player should move from the spot of the bomb before exploding or the game will restart
//there is also a led that warns the player that is in the spot of the bomb
void bomb() {
  swState = digitalRead(pinSW);

  if (swState != lastSwState) {
    lastDebounceTimeButton = millis();
  }

  if (millis() - lastDebounceTimeButton > debounceDelayButton) {
    if (swState == LOW && !lastSwState) {
      if (!bombActive) {
        bombStartTime = millis();
        bombActive = true;
        bombRow = playerRow;
        bombCol = playerCol;
        isBombLedOn = true;
      }
    }
  }

  if (bombActive) {
    if (soundOnOrOff == true) {
      tone(buzzerPin, 400, 100);
    }
    if (
      (playerRow == bombRow && playerCol == bombCol) ||
      (playerRow == bombRow - 1 && playerCol == bombCol) ||
      (playerRow == bombRow + 1 && playerCol == bombCol) ||
      (playerRow == bombRow && playerCol == bombCol - 1) ||
      (playerRow == bombRow && playerCol == bombCol + 1)
    ) {
      digitalWrite(ledPin, HIGH);
    } else {
      digitalWrite(ledPin, LOW);
    }
    elapsedTime = millis() - bombStartTime;
    if (elapsedTime >= bombDelay) {
      if (bombRow > 0) {
        walls[bombRow - 1][bombCol] = false; // up
        lc.setLed(0, bombRow - 1, bombCol, false);
      }
      if (bombRow < matrixSize - 1) {
        walls[bombRow + 1][bombCol] = false; // down
        lc.setLed(0, bombRow + 1, bombCol, false);
      }
      if (bombCol > 0) {
        walls[bombRow][bombCol - 1] = false; // left
        lc.setLed(0, bombRow, bombCol - 1, false);
      }
      if (bombCol < matrixSize - 1) {
        walls[bombRow][bombCol + 1] = false; // right
        lc.setLed(0, bombRow, bombCol + 1, false);
      }

      // turn off bomb LED after exploding
      lc.setLed(0, bombRow, bombCol, false);
      bombActive = false;

      if (
        (playerRow == bombRow && playerCol == bombCol) ||
        (playerRow == bombRow - 1 && playerCol == bombCol) ||
        (playerRow == bombRow + 1 && playerCol == bombCol) ||
        (playerRow == bombRow && playerCol == bombCol - 1) ||
        (playerRow == bombRow && playerCol == bombCol + 1)
      ) {
        // restart game if the player has lives left
        lives -= 1;
        generateRandomMap();
        resetPlayer();
        digitalWrite(ledPin, LOW);
        // stops the game if the player has no more lives
        if (lives == 0) {
          lcd.clear();
          displayX();
          centerTextOnLcd(F("Game over!"), 0);
          delay(3000);
          gameFinished = false;
          gameInProgress = false;
          lives = 3;
        }
      }
    } else {
      if (millis() - lastBlinkTimeBomb > blinkIntervalBomb) {
        isBombLedOn = !isBombLedOn;
        lc.setLed(0, bombRow, bombCol, isBombLedOn);
        lastBlinkTimeBomb = millis();
      }
    }
  }

  lastSwState = swState;
}

//generates random walls depending on the level you are on
//the walls are put in an array
void generateRandomMap() {
  lc.clearDisplay(0);
  int wallPercentage = 0;

  if (n == 0) {
    wallPercentage = 55;
  } else {
    wallPercentage = 80;
  }

  for (int i = 0; i < matrixSize; ++i) {
    for (int j = 0; j < matrixSize; ++j) {
      int randomNumber = random(100);

      // the walls can't be generated where the player starts so that there is space for it to move
      if (i == 0 && j == 0) {
        lc.setLed(0, i, j, true);
      } else if (i == 1 && j == 0) {
        lc.setLed(0, i, j, false);
        walls[i][j] = false;
      } else if (i == 0 && j == 1) {
        lc.setLed(0, i, j, false);
        walls[i][j] = false;
      } else if (i == 1 && j == 1) {
        lc.setLed(0, i, j, false);
        walls[i][j] = false;
      } else {
        walls[i][j] = randomNumber < wallPercentage;
        if (walls[i][j]) {
          lc.setLed(0, i, j, true);
        }
      }
    }
  }
}

// stops the game without showing any message
void stopGame() {
  buttonState = digitalRead(buttonPin);

  if (buttonState != lastButtonState) {
    lastDebounceTimeButton = millis();
  }

  if (millis() - lastDebounceTimeButton > debounceDelayButton) {
    if (buttonState == LOW && !lastButtonState) {
      gameInProgress = false;
      lcd.clear();
      gameFinished = false;
      lives = 3;
    }
  }

  lastButtonState = buttonState;
}

// sounds for the menu with the check wheter the sound is on or off
void menuMovingSound() {
  if (soundOnOrOff == true) {
    tone(buzzerPin, 4186, 100);
  }
}

void themeSong() {
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(buzzerPin, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
  }
}

void menuChoosingSound() {
  if (soundOnOrOff == true) {
    tone(buzzerPin, 4699, 100);
  }
}

// drawings for the matrix
void displayCup() {
  byte cup[8] = {
    B00111100,
    B00011000,
    B00011000,
    B00111100,
    B11111111,
    B10111101,
    B11111111,
    B00111100

  };

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, bitRead(cup[row], 7 - col));
    }
  }
}

void displayX() {
  byte gameOver[8] = {
    B11111111,
    B11000011,
    B10100101,
    B10011001,
    B10011001,
    B10100101,
    B11000011,
    B11111111

  };

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, bitRead(gameOver[row], 7 - col));
    }
  }
}

void displayMaze() {
  byte maze[8] = {
    B01010101,
    B10101010,
    B01010101,
    B10101010,
    B01010101,
    B10101010,
    B01010101,
    B10101010
  };

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, bitRead(maze[row], 7 - col));
    }
  }
}

void displayQuestionMark() {
  byte questionMark[8] = {
    B00010000,
    B00000000,
    B00010000,
    B00010000,
    B00100000,
    B00100100,
    B00100100,
    B00011000
  };

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, bitRead(questionMark[row], 7 - col));
    }
  }
}

void displayGithubLogo() {
  byte githubLogo[8] = {
    B00011000,
    B00011000,
    B00111100,
    B01111110,
    B01111110,
    B00111100,
    B00100100,
    B00000000,
  };

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, bitRead(githubLogo[row], 7 - col));
    }
  }
}

void displaySettingsLogo() {
  byte settingsLogo[8] = {
    B00011000,
    B00011000,
    B00111100,
    B01100110,
    B01100110,
    B00100100,
    B00000000,
    B00000000,
  };

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, bitRead(settingsLogo[row], 7 - col));
    }
  }
} 
