#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

#define DOT_INTERVAL    150
#define BUZZER_FREQ     700
#define BUZZER_INTERVAL 300


//Visuals Pins
#define LED_PIN         2
#define BUZZER_PIN      6

//Button Pins
#define MORSE_KEY_PIN   4
#define EDIT_PIN        13
#define SETTING_PIN     SCL

// LCD Pins
#define RS              3
#define EN              5
#define D4              7
#define D5              8
#define D6              9
#define D7              12

// HC-12 Pins
#define HC_TX           10
#define HC_RX           11


#define DASH_INTERVAL   DOT_INTERVAL * 3
#define WORD_INTERVAL   DOT_INTERVAL * 7

#include "connection.h"
#include "morse_lcd.h"
#include "morse_code.h"
#include "button.h"
#include "morse_buzzer_and_led.h"

SoftwareSerial HC12(HC_TX, HC_RX);
Connection connection(HC12);
MorseLCD morseLCD(LiquidCrystal(RS, EN, D4, D5, D6, D7));

ArduinoButton morseKeyButton(MORSE_KEY_PIN);
ArduinoButton editButton(EDIT_PIN);
ArduinoButton settingButton(SETTING_PIN);

BuzzerLED buzzerLed;

String morseCode = "";
String morseString = "";
String receiveString = "";

bool lastSpace = false;
bool noAddSpace = false;

void send(char x) {
    connection.write(x);
}
void received (char x) {
    if (x == ' ' || isValidMorseLetter(x)) {
        buzzerLed.add(x);
        receiveString = receiveString + x;
    } else if (x == '-' && receiveString.length() > 0) {
        receiveString = receiveString.substring(0, receiveString.length() - 1);
    } else if (x == '_') {
        receiveString = "";
    }
}
void received (char* x) {
    for (int i = 0; i < strlen(x); ++i) {
        received(x[i]);
    }
}

void onEditButtonClick(){
    noAddSpace = true;
}
void onEditButtonMultiClick (const int count) {
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
    } else {
        if (count == 2) {
            morseString = "";
            morseCode = "";
            buzzerLed.clear();
            send('_');
        } else if (count == 3) {
            receiveString = "";
        }
    }
}

void onSettingButtonMultiClick (const int count) {
    static bool settingButtonMode = false;
    if (count == 1) {
        if (!settingButtonMode) {
            connection.setChannel(connection.getChannel() < 'E' ? static_cast<char>(connection.getChannel() + 1) : 'A');
        } else {
            buzzerLed.setBuzzer(!buzzerLed.isBuzzer());
            buzzerLed.setLed(buzzerLed.isBuzzer() ^ buzzerLed.isLed());
        }
    }
    if (count == 2) settingButtonMode = !settingButtonMode;
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
            morseString += morseToChar(morseCode);
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
void LcdLoop() {
//    morseLCD.changeSYM(connection.getID());
//    morseLCD.changeSYM(connection.getRecvID() == '\0' ? 'R' : connection.getRecvID());
    morseLCD.write(morseString + morseCode);
    morseLCD.write(receiveString, false);
    morseLCD.footer(connection.getChannel());
    morseLCD.footer(static_cast<char>('0' + 1 + (buzzerLed.isLed() ? 0 : 2) + (buzzerLed.isBuzzer() ? 0 : 1)), false);
}

void setup() {
    randomSeed(static_cast<unsigned int>(analogRead(0) + analogRead(1) + analogRead(2) + analogRead(3) + analogRead(4) + analogRead(5)));
    morseKeyButton.begin();

    editButton.begin();
    editButton.setClick(onEditButtonClick);
    editButton.setMultiClick(onEditButtonMultiClick, DOT_INTERVAL * 2);

    settingButton.begin();
    settingButton.setMultiClick(onSettingButtonMultiClick, DOT_INTERVAL * 2);

    buzzerLed.begin();

    connection.begin();
    connection.onReceive(received);
    morseLCD.begin();

}
void loop() {
//    static unsigned long start = millis();
//    if (millis() - start > 1000) {
//        send('A');
//        start = millis();
//    }

    connection.loop();

    morseKeyButton.loop();
    editButton.loop();
    settingButton.loop();

    loopMorseKey();

    if (morseString.length() > 14) {
        morseString = morseString.substring(1);
    }
    if (receiveString.length() > 14) {
        receiveString = receiveString.substring(1);
    }

    LcdLoop();
    buzzerLed.loop();
}