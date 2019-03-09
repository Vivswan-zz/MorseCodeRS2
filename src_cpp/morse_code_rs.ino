#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include "connection.h"
#include "morse_lcd.h"
#include "morse_code.h"

#define DOT_INTERVAL    150
#define BUZZER_FREQ     700
#define MORSE_READ_RATE 2

//Visuals
#define LED_PIN         2
#define BUZZER_PIN      6

//Button
#define MORSE_KEY_PIN   4
#define EDIT_PIN        13
#define SETTING_PIN     SCL

// LCD
#define RS              3
#define EN              5
#define D4              7
#define D5              8
#define D6              9
#define D7              12

// HC-12
#define HC_TX           10
#define HC_RX           11


#define DASH_INTERVAL   DOT_INTERVAL * 3
#define WORD_INTERVAL   DOT_INTERVAL * 7

Connection connection(SoftwareSerial(HC_RX, HC_TX));
MorseLCD morseLCD(LiquidCrystal(RS, EN, D4, D5, D6, D7));

String morseCode = "";
String morseString = "";
String receiveString = "";

boolean lastSpace = false;
boolean noAddSpace = false;
boolean buzzerBeap = false;

boolean ledOnReceive = true;
boolean buzzerOnReceive = true;

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
String textToSound (const String &t) {
    return morseToSound(textToMorse(t));
}

void addToBuzzer(String text) {
    static String toProcess = "";
    static String toBeap = "";
    static unsigned long start = millis();

    if (text != nullptr && text.length() > 0) {
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
//    received(x);
//    static unsigned long start = millis();
//    boolean confirmation = false;
//
//    start = millis();
//    if (x != '#') {
//        HC12.print(String(x));
//    } else {
//        confirmation = true;
//    }
//    int count = 0;
//    while (HC12.available() || !confirmation) {
//        char incomingByte = HC12.read();
//        delay(5);
//        if(!confirmation && incomingByte == '&') {
//            morseLCD.changeSYM('S');
//            confirmation = true;
//        } else {
//            received(incomingByte);
//        }
//        if (count >= 5) {
//            start = millis();
//            confirmation = true;
//            morseString = morseString.substring(0, morseString.length() - 1);
//            morseLCD.changeSYM('X');
//        }
//        if (millis() - start > DOT_INTERVAL && !confirmation) {
//            HC12.print(String(x));
//            start = millis();
//
//            count++;
//        }
//    }
}
void received (char x) {
    if (x == ' ' || isValidLetter(x)) {
//        HC12.print("&");
        addToBuzzer(String(x));
    } else if (x == '-' && receiveString.length() > 0) {
//        HC12.print("&");
        receiveString = receiveString.substring(0, receiveString.length() - 1);
    } else if (x == '_') {
//        HC12.print("&");
        receiveString = "";
    }
}

void loopEditButton () {
    static unsigned long start = millis();
    static unsigned int count = 0;
    static boolean lastButtonState = LOW;
    auto buttonState = static_cast<boolean>(digitalRead(EDIT_PIN));

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
    auto buttonState = static_cast<boolean>(digitalRead(SETTING_PIN));

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
    auto buttonState = static_cast<boolean>(digitalRead(MORSE_KEY_PIN));
    unsigned long timeInterval = millis() - start;

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
            morseCode = morseToChar(morseCode);
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
    morseLCD.write(morseString + morseCode);
    morseLCD.write(receiveString, false);
}

void loopBuzzer() {
    addToBuzzer(nullptr);
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

    connection.begin();
    morseLCD.begin();
    Serial.print(textToMorse("Vivswan Shah."));
    Serial.write('\n');
    Serial.print(morseToChar("*"));
    Serial.write('\n');
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
    connection.loop();
    setting();


    loopEditButton();
    loopSettingButton();
    loopMorseKey();
    loopLCD();
    loopBuzzer();
    loopHC12();
}