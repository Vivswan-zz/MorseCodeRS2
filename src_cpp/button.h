#ifndef MORSECODERS_BUTTON_H
#define MORSECODERS_BUTTON_H

class ArduinoButton{
    private:
        const uint8_t pin;
        int lastState = LOW;

        void (*click)() = nullptr;

        void (*multiClick)(int count) = nullptr;
        unsigned long startClick = millis();
        int maxClickInterval = 0;
        int clickCount = 0;

        void (*stateChange)(int state) = nullptr;

    public:
        explicit ArduinoButton(const uint8_t pin): pin(pin) {}

        void begin() {
            pinMode(pin, INPUT);
        }

        void loop() {
            int state = digitalRead(pin);
            unsigned long now = millis();

            if (now - startClick > maxClickInterval && clickCount > 0 && multiClick != nullptr) {
                multiClick(clickCount);
                clickCount = 0;
            }

            if (state == HIGH && lastState == LOW && click != nullptr) {
                click();

                startClick = now;
                clickCount++;
            }

            if (state != lastState && stateChange != nullptr) {
                stateChange(state);
            }

            lastState = state;
        }

        void setClick(void (*onclick)()) {
            click = onclick;
        }

        void setMultiClick(void (*onMultiClick)(int count), int maxClickInterval) {
            this->multiClick = onMultiClick;
            this->maxClickInterval = maxClickInterval;
        }

        void setOnStateChange(void (*onStateChange)(int state)) {
            this->stateChange = onStateChange;
        }

};

#endif
