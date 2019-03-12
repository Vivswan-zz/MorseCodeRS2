#ifndef MORSECODERS_CONNECTION_H
#define MORSECODERS_CONNECTION_H

#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include <Stream.h>

#define HC12_SPEED 9600
#define SERIAL_SPEED 9600

#define BUFFER_SIZE 52
#define STORAGE_BUFFER_SIZE 52

class Connection {
    char channel = 'A';

    class Connect{
        const char id = static_cast<char>('A' + random(25));

        char recvId = '\0';

        Stream *stream;

        char buffer[BUFFER_SIZE]{};
        char store[STORAGE_BUFFER_SIZE]{};

        unsigned long timer = 0;

    public:

        explicit Connect(Stream *stream): stream(stream){}

        void loop() {
            if (stream->available()) {
                stream->readBytesUntil('\0', buffer, BUFFER_SIZE);
                stream->print(buffer);
            }
        }

        bool isConnected(){
            return false;
        }

    };

    private:
        SoftwareSerial HC12;
        Connect *serialConnect = nullptr;


    public:
        explicit Connection(const SoftwareSerial &HC12) : HC12(HC12) {}

        void begin () {
            HC12.begin(HC12_SPEED);
            Serial.begin(SERIAL_SPEED);
            serialConnect = new Connect(&Serial);
        }

        void loop() {
            serialConnect->loop();
        }

        void write (char * x){
            Serial.write(x);
            HC12.write(x);
        }

        int read() {
            if (Serial.available()){
                return Serial.read();
            }
            if (HC12.available()){
                return HC12.read();
            }
            return -1;
        }

        bool isConnected(){
            return serialConnect->isConnected();
        }


        char getChannel() const {
            return channel;
        }

        void setChannel(char channel) {
            this->channel = channel;
        }

};

#endif
