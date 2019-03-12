#ifndef MORSECODERS_MORSE_BUZZER_AND_LED_H
#define MORSECODERS_MORSE_BUZZER_AND_LED_H

#ifdef BUZZER_PIN
#ifdef LED_PIN
#ifdef BUZZER_FREQ
#ifdef BUZZER_INTERVAL

#define LENGTH_TO_PROCESS 52
#define LENGTH_CURRENT_SOUND 36

class BuzzerLED {
    private:
        bool buzzer = true;
        bool led = true;

        bool willBeep = false;
        bool willLed = false;

        char toProcess [LENGTH_TO_PROCESS]{};
        char currentSound [LENGTH_CURRENT_SOUND]{};

        uint8_t indexToProcess = 0;

        unsigned long start = 0;
        bool beepCharCurrent = false;

        int pop (char * s) {
            int c = s[0];
            for (int i = 1; i < strlen(s); ++i) {
                s[i - 1] = s[i];
            }
            s[strlen(s) - 1] = '\0';
            return c;
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

        void toSound (int pop) {
            if (isValidMorseLetter(static_cast<char>(pop))) {
                 morseToSound(charToMorse(static_cast<const char>(pop))).toCharArray(currentSound, LENGTH_CURRENT_SOUND);
            }
        }
        void checkSound () {
            if (millis() - start > BUZZER_INTERVAL) {
                if (currentSound[0] == '\0') {
                    beepCharCurrent = false;
                } else {
                    int c = pop(currentSound);
                    beepCharCurrent = c == 'b';
                    start = millis();
                }
            }
            if (beepCharCurrent) {
                flash();
                beep();
            }
        }

        void beepAccordingly () {
            if (willBeep) {
                tone(BUZZER_PIN, BUZZER_FREQ);
                willBeep = false;
            } else {
                noTone(BUZZER_PIN);
            }
        }
        void ledAccordingly () {
            if (willLed) {
                digitalWrite(LED_PIN, HIGH);
                willLed = false;
            } else {
                digitalWrite(LED_PIN, LOW);
            }
        }

    public:
        void begin () {
            pinMode(LED_PIN, OUTPUT);
            pinMode(BUZZER_PIN, OUTPUT);
            toProcess[0] = '\0';
            currentSound[0] = '\0';
        }

        void loop () {
            if (currentSound[0] == '\0' && canPop()) toSound(pop());
            if (currentSound[0] != '\0') checkSound();
            if (!buzzer) willBeep = false;
            if (!led) willLed = false;
            beepAccordingly();
            ledAccordingly();
        }

        bool isBuzzer () const {
            return buzzer;
        }
        void setBuzzer (bool buzzer = true) {
            this->buzzer = buzzer;
        }

        bool isLed () const {
            return led;
        }
        void setLed (bool led = true) {
            this->led = led;
        }

        bool canAdd (unsigned int l) {
            return l + indexToProcess < LENGTH_TO_PROCESS - 2;
        }
        bool add (char * s) {
            if (canAdd(strlen(s))) {
                for (int i = 0; i < strlen(s) && indexToProcess < LENGTH_TO_PROCESS - 2; ++i) {
                    toProcess[indexToProcess] = static_cast<char>(toUpperCase(s[i]));
                    indexToProcess++;
                }
                toProcess[indexToProcess] = '\0';
                return true;
            }
            return false;
        }
        bool add (char s) {
            if (canAdd(1)) {
                toProcess[indexToProcess] = static_cast<char>(toUpperCase(s));
                indexToProcess++;
                return true;
            }
            return false;
        }

        bool canPop () {
            return toProcess[0] != '\0';
        }
        int pop () {
            if (canPop()) {
                indexToProcess--;
                return pop(toProcess);
            } else {
                return -1;
            }
        }

        void clear () {
            toProcess[0] = '\0';
            currentSound[0] = '\0';
            indexToProcess = 0;
        }

        void beep (bool b = true) {
            willBeep = b;
        }
        void flash (bool b = true) {
            willLed = b;
        }
};

#endif
#endif
#endif
#endif
#endif
