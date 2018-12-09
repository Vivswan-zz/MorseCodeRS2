#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

#define DOT_INTERVAL    150
#define BUZZER_FREQ     700
#define MORSE_READ_RATE 2

//Visuals
#define LED_PIN         2
#define BUZZER_PIN      5

//Button
#define MORSE_KEY_PIN   3
#define EDIT_PIN        13
#define SETTING_PIN     6

// LCD
#define RS              0
#define EN              1
#define D4              7
#define D5              8
#define D6              9
#define D7              12

// HC-12
#define HC_TX           10
#define HC_RX           11


#define DASH_INTERVAL   DOT_INTERVAL * 3
#define WORD_INTERVAL   DOT_INTERVAL * 7

const String MORSE_CODE[][2]= {
  {"A", "*-"},
  {"B", "-***"},
  {"C", "-*-*"},
  {"D", "-**"},
  {"E", "*"},
  {"F", "**-*"},
  {"G", "--*"},
  {"H", "****"},
  {"I", "**"},
  {"J", "*---"},
  {"K", "-*-"},
  {"L", "*-**"},
  {"M", "--"},
  {"N", "-*"},
  {"O", "---"},
  {"P", "*--*"},
  {"Q", "--*-"},
  {"R", "*-*"},
  {"S", "***"},
  {"T", "-"},
  {"U", "**-"},
  {"V", "***-"},
  {"W", "*--"},
  {"X", "-**-"},
  {"Y", "-*--"},
  {"Z", "--**"},
  {"0", "-----"},
  {"1", "*----"},
  {"2", "**---"},
  {"3", "***--"},
  {"4", "****-"},
  {"5", "*****"},
  {"6", "-****"},
  {"7", "--***"},
  {"8", "---**"},
  {"9", "----*"},
  {".", "*-*-*-"},
  {",", "--**--"},
  {"?", "**--**"},
  {"'", "*----*"},
  {"!", "-*-*--"},
};
#define MORSE_CODE_LENGTH  41

SoftwareSerial HC12(HC_TX, HC_RX);
LiquidCrystal LCD(RS, EN, D4, D5, D6, D7);

String morseCode = "";
String morseString = "";
String receiveString = "";

boolean lastSpace = false;
boolean noAddSpace = false;
boolean buzzerBeap = false;

boolean ledOnReceive = true;
boolean buzzerOnReceive = true;

void LCDLast (String s, int i) {
  LCD.setCursor(2, i);
  if (s.length() > 14) {
    LCD.print(String(s.substring(s.length() - 14, s.length())));
  } else {
    LCD.print(s);
    LCD.print("              ");
  }
}

String singleMorseToText (String m) {
  for (int i=0; i < MORSE_CODE_LENGTH; i++) {
    if(MORSE_CODE[i][1] == m) {
      return MORSE_CODE[i][0];
    }
  }
  return "";
}
String textToMorse (String t) {
  String r = "";
  
  for (int i = 0; i < t.length(); i++) {
    if (t[i] == ' ') {
      r += " ";
      continue;
    }
    for (int j = 0; j < MORSE_CODE_LENGTH; j++) {
      if(MORSE_CODE[j][0] == String(t[i])) {
        r += MORSE_CODE[j][1];
        r += " ";
        break;
      }
    }
  }
  
  return r;
}
String morseToSound (String m) {
  String r = "";
  
  for (int i = 0; i < m.length(); i++) {
    if(m[i] == '*') {
      r += "b ";
    } else if (m[i] == '-') {
      r += "bbb ";
    } else if (m[i] == ' ') {
      r += "  ";
    }
  }
  
  return r;
}
String textToSound (String t) {
  return morseToSound(textToMorse(t));
}

boolean isValidLetter (char x) {
  for (int i = 0; i < MORSE_CODE_LENGTH; i++) {
    if (x == MORSE_CODE[i][0][0]) {
      return true;
    }
  }
  return false;
}

void addToBuzzer(String text) {
  static String toProcess = "";
  static String toBeap = "";
  static unsigned long start = millis();
  
  if (text.length() > 0) {
    text.toUpperCase();
    toProcess += text;
  }
  
  if (toBeap.length() == 0 && toProcess.length() == 0) {
    start = millis();
  } else if (toBeap.length() == 0 && toProcess.length() > 0) {
    String x = String(toProcess[0]);
    receiveString += x;
    toBeap = textToSound(x);
    toProcess = toProcess.substring(1);
  } else if (toBeap.length() > 0 && millis() - start > DOT_INTERVAL * MORSE_READ_RATE) {
    toBeap = toBeap.substring(1);
    start = millis();
  }
  if (toBeap.length() > 0 && toBeap[0] == 'b') {
    if (ledOnReceive) {
      digitalWrite(LED_PIN, HIGH);
    }
    if (buzzerOnReceive) {
      buzzerBeap = true;
    }
  } else {
    if (ledOnReceive) {
      digitalWrite(LED_PIN, LOW);
    }
  }
}

void send(char x) {
  static unsigned long start = millis();
  boolean confirmation = false;
  
  start = millis();
  if (x != '#') {
    HC12.print(String(x));
  } else {
    confirmation = true;
  }
  int count = 0;
  while (HC12.available() || !confirmation) {
    char incomingByte = HC12.read();
    delay(5);
    if(!confirmation && incomingByte == '&') {
      LCD.setCursor(0, 0);
      LCD.print("S");
      confirmation = true;
    } else {
      received(incomingByte);
    }
    if (count >= 5) {
      start = millis();
      confirmation = true;
      morseString = morseString.substring(0, morseString.length() - 1);
      LCD.setCursor(0, 0);
      LCD.print("X");
    }
    if (millis() - start > DOT_INTERVAL && !confirmation) {
      HC12.print(String(x));
      start = millis();
      
      count++;
    }
  }
}
void received (char x) {
    if (x == ' ' || isValidLetter(x)) {
      HC12.print("&");
      addToBuzzer(String(x));
    } else if (x == '-' && receiveString.length() > 0) {
      HC12.print("&");
      receiveString = receiveString.substring(0, receiveString.length() - 1);
    } else if (x == '_') {
      HC12.print("&");
      receiveString = "";
    }
}

void loopEditButton () {
  static unsigned long start = millis();
  static unsigned int count = 0;
  static boolean lastButtonState = LOW;
  boolean buttonState = digitalRead(EDIT_PIN);
  
  if (buttonState != lastButtonState) {
    start = millis();
    if (buttonState == HIGH) {
      count++;
      noAddSpace = true;
    }
    lastButtonState = buttonState;
  }
  if (millis() - start > DOT_INTERVAL) {
    if (count == 1 && morseString.length() > 0) {
      if (lastSpace && morseString[morseString.length() - 1] == ' ') {
        morseString = morseString.substring(0, morseString.length() - 2);
        send('-');
        send('-');
        lastSpace = false;
      } else {
        morseString = morseString.substring(0, morseString.length() - 1);
        send('-');
      }
    }
    if (count == 2) {
      morseString = "";
      morseCode = "";
      send('_');
    }
    if (count == 3) {
      receiveString = "";
    }
    count = 0;
  }
}
void loopSettingButton () {
  static boolean lastButtonState = LOW;
  boolean buttonState = digitalRead(SETTING_PIN);
  
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      if (ledOnReceive && buzzerOnReceive) {
        ledOnReceive = false;
        buzzerOnReceive = true;
      } else if (!ledOnReceive && buzzerOnReceive) {
        ledOnReceive = true;
        buzzerOnReceive = false;
      } else {
        ledOnReceive = true;
        buzzerOnReceive = true;
      }
    }
    lastButtonState = buttonState;
  }
}
void loopMorseKey () {
  static unsigned long start = millis();
  static boolean lastButtonState = LOW;
  boolean buttonState = digitalRead(MORSE_KEY_PIN);
  unsigned int timeInterval = millis() - start;

  if (lastButtonState != buttonState) {
    noAddSpace = false;
    if (buttonState == LOW) {
      if (timeInterval < DOT_INTERVAL) {
        morseCode += "*";
      } else {
        morseCode += "-";
      }
    }
    if (morseCode.length() > 7) {
      morseCode = "";
    }
    start = millis();
    lastButtonState = buttonState;
  } 
  
  if(buttonState == LOW){ 
    if(morseCode.length() > 0 && timeInterval > DASH_INTERVAL && timeInterval < WORD_INTERVAL) {
      morseString += singleMorseToText(morseCode);
      morseCode = "";
      send(morseString[morseString.length() - 1]);
    } else if (morseString.length() > 0 && morseString[morseString.length() - 1] != ' ' && (!noAddSpace)
              && timeInterval > WORD_INTERVAL && timeInterval < WORD_INTERVAL + DOT_INTERVAL) {
      morseString += " "; 
      lastSpace = true;
      send(morseString[morseString.length() - 1]);
    }
  }
}
void loopLCD () {
  LCDLast(morseString + morseCode, 0);
  LCDLast(receiveString, 1);
}
void loopBuzzer() {
  addToBuzzer("");
  if (buzzerBeap) {
    tone(BUZZER_PIN, BUZZER_FREQ);
  } else {
    noTone(BUZZER_PIN);
  }
  buzzerBeap = false;
}
void loopHC12 () {
  send('#');
}

void setup() {
  pinMode(MORSE_KEY_PIN, INPUT);
  pinMode(EDIT_PIN, INPUT);
  pinMode(SETTING_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  HC12.begin(9600);

  LCD.begin(16, 2);
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("S:");
  LCD.setCursor(0, 1);
  LCD.print("R:");
}

void setting() {
  if (morseString.length() > 14) {
    morseString = morseString.substring(1);
  }
  if (receiveString.length() > 14) {
    receiveString = receiveString.substring(1);
  }  
}

void loop() {
  setting();
  
  loopEditButton();
  loopSettingButton();
  loopMorseKey();
  loopLCD();
  loopBuzzer();
  loopHC12();
}