#ifndef MORSECODERS_MORSE_LCD_H
#define MORSECODERS_MORSE_LCD_H

#include <LiquidCrystal.h>

#define LCD_ROWS 2
#define LCD_COLS 16

class MorseLCD {
    private:
        char senderSym = 'S';
        char receiverSym = 'R';
        unsigned short printLen = LCD_COLS - 4;

        LiquidCrystal lcd;

    public:
        explicit MorseLCD(const LiquidCrystal &lcd): lcd(lcd){}

        void begin() {
            lcd.begin(LCD_ROWS, LCD_COLS);
            lcd.clear();
            reset();
        }

        void write(const String &str, bool sender = true) {
            lcd.setCursor(0, static_cast<uint8_t>(sender ? 0 : 1));
            lcd.print(sender ? senderSym : receiverSym);
            lcd.print(':');
            if (str.length() > printLen) {
                lcd.print(str.substring(str.length() - printLen, str.length()));
            } else {
                lcd.print(str);
                for (int i = 0; i < printLen - str.length(); ++i) {
                    lcd.print(" ");
                }
            }
        }


        void footer (char c, bool sender = true) {
            lcd.setCursor(static_cast<uint8_t>(LCD_COLS - 2), static_cast<uint8_t>(sender ? 0 : 1));
            lcd.print(':');
            lcd.print(c);
        }

        void changeSYM(char sym, bool sender = true) {
            (sender ? senderSym : receiverSym) = sym;
        }

        void reset() {
            this->write(" ", true);
            this->write(" ", false);
        }
};

#endif
