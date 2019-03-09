#ifndef MORSECODERS_MORSE_LCD_H
#define MORSECODERS_MORSE_LCD_H

#include <LiquidCrystal.h>

#define LCD_ROWS 2
#define LCD_COLS 16

class MorseLCD {
    private:
        char senderSym = 'S';
        char recieverSym = 'R';
        unsigned short printLen = LCD_COLS - 2;

        LiquidCrystal lcd;

    public:
        explicit MorseLCD(LiquidCrystal lcd): lcd(lcd){}

        void begin() {
            lcd.begin(LCD_ROWS, LCD_COLS);
            lcd.clear();
            reset();
        }

        void write(const String &str, bool sender = true) {
            lcd.setCursor(0, sender ? 0 : 1);
            lcd.print(sender ? senderSym : recieverSym);
            lcd.print(':');
            if (str.length() > printLen) {
                lcd.print(str.substring(str.length() - printLen, str.length()));
            } else {
                lcd.print(str);
                for (int i = 0; i < printLen; ++i) {
                    lcd.print(" ");
                }
            }
        }

        void changeSYM(char sym, bool sender = true) {
            (sender ? senderSym : recieverSym) = sym;
        }

        void reset() {
            this->write(" ", true);
            this->write(" ", false);
        }
};

#endif
