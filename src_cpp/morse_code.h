#ifndef MORSECODERS_MORSE_CODE_H
#define MORSECODERS_MORSE_CODE_H

#include <avr/pgmspace.h>

#define BUF_SIZE 8

const PROGMEM char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,?'!";
const PROGMEM char morse_A[] = "*-";
const PROGMEM char morse_B[] = "-***";
const PROGMEM char morse_C[] = "-*-*";
const PROGMEM char morse_D[] = "-**";
const PROGMEM char morse_E[] = "*";
const PROGMEM char morse_F[] = "**-*";
const PROGMEM char morse_G[] = "--*";
const PROGMEM char morse_H[] = "****";
const PROGMEM char morse_I[] = "**";
const PROGMEM char morse_J[] = "*---";
const PROGMEM char morse_K[] = "-*-";
const PROGMEM char morse_L[] = "*-**";
const PROGMEM char morse_M[] = "--";
const PROGMEM char morse_N[] = "-*";
const PROGMEM char morse_O[] = "---";
const PROGMEM char morse_P[] = "*--*";
const PROGMEM char morse_Q[] = "--*-";
const PROGMEM char morse_R[] = "*-*";
const PROGMEM char morse_S[] = "***";
const PROGMEM char morse_T[] = "-";
const PROGMEM char morse_U[] = "**-";
const PROGMEM char morse_V[] = "***-";
const PROGMEM char morse_W[] = "*--";
const PROGMEM char morse_X[] = "-**-";
const PROGMEM char morse_Y[] = "-*--";
const PROGMEM char morse_Z[] = "--**";
const PROGMEM char morse_0[] = "-----";
const PROGMEM char morse_1[] = "*----";
const PROGMEM char morse_2[] = "**---";
const PROGMEM char morse_3[] = "***--";
const PROGMEM char morse_4[] = "****-";
const PROGMEM char morse_5[] = "*****";
const PROGMEM char morse_6[] = "-****";
const PROGMEM char morse_7[] = "--***";
const PROGMEM char morse_8[] = "---**";
const PROGMEM char morse_9[] = "----*";
const PROGMEM char morse_dot[] = "*-*-*-";
const PROGMEM char morse_comma[] = "--**--";
const PROGMEM char morse_question[] = "**--**";
const PROGMEM char morse_apostrophe[] = "*----*";
const PROGMEM char morse_excamation[] = "-*-*--";

const PROGMEM char * const morse_arr[] = {
        morse_A,
        morse_B,
        morse_C,
        morse_D,
        morse_E,
        morse_F,
        morse_G,
        morse_H,
        morse_I,
        morse_J,
        morse_K,
        morse_L,
        morse_M,
        morse_N,
        morse_O,
        morse_P,
        morse_Q,
        morse_R,
        morse_S,
        morse_T,
        morse_U,
        morse_V,
        morse_W,
        morse_X,
        morse_Y,
        morse_Z,
        morse_0,
        morse_1,
        morse_2,
        morse_3,
        morse_4,
        morse_5,
        morse_6,
        morse_7,
        morse_8,
        morse_9,
        morse_dot,
        morse_comma,
        morse_question,
        morse_apostrophe,
        morse_excamation,
};


boolean isValidLetter (char x) {
    for (int i = 0; i < strlen_P(chars); ++i) {
        if (x == pgm_read_byte_near(chars + i)){
            return true;
        }
    }
    return false;
}


char morseToChar (const char * m) {
    for (int i = 0; i < strlen_P(chars); ++i) {
        if (strcmp_P(m, (char *) pgm_read_word(&(morse_arr[i]))) == 0) {
            return pgm_read_byte_near(chars + i);
        }
    }
    return '\0';
}

String morseToChar (const String &m) {
    char buff[BUF_SIZE];
    m.toCharArray(buff, BUF_SIZE, 0);

    char x = morseToChar(buff);
    if (x == '\0') {
        return "";
    } else {
        return String(x);
    }
}

String charToMorse (const char c){
    char m[BUF_SIZE];

    for (int i = 0; i < strlen_P(chars); ++i) {
        if (toUpperCase(c) == pgm_read_byte_near(chars + i)){
            strcpy_P(m, (char *) pgm_read_word(&(morse_arr[i])));
            return String(m);
        }
    }

    return "";
}

String textToMorse (const String &t) {
    String r = "";
    for (int i = 0; i < t.length(); ++i) {
        if (t[i] == ' ') {
            r += " ";
        } else {
            r += charToMorse(t[i]);
            r += " ";
        }
    }
    return r;
}

#endif
