#ifndef MORSECODERS_CONNECTION_H
#define MORSECODERS_CONNECTION_H

#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include <Stream.h>

#define HC12_SPEED 9600
#define SERIAL_SPEED 9600

#define BUFFER_SIZE 52

class Connection {
    // Message Syntax: <channel>:<receiver id>@<sender id>:<message><\n>
    class Connect{
    public:
        const char id;
        char recvId = '\0';
        char channel = 'A';

        Stream *stream;
        char buffer[BUFFER_SIZE]{};

        void (*receiveFunc)(char * cArr) = nullptr;

        explicit Connect(Stream *stream, const char id): stream(stream), id(id) {
            buffer[0] = '\0';
        }
        void loop() {
            if (stream->available()) {
                buffer[stream->readBytesUntil('\n', buffer, BUFFER_SIZE)] = '\0';
                isMessage();
                buffer[0] = '\0';
            }
        }

        void isMessage() {
            if (
                    buffer[0] == channel &&
                    (buffer[2] == id     || buffer[2] == '*') &&
                    (buffer[4] == recvId || recvId == '\0') &&
                    strlen(buffer) < BUFFER_SIZE &&
                    strlen(buffer + 6) > 0
            ) {
                this->receiveFunc(buffer + 6);
            }
        }

        void writeinfo(){
            stream->print(channel);
            stream->print(":*@");
            stream->print(id);
            stream->print(":");
        }
        void write (char * x){
            writeinfo();
            stream->println(x);
        }
        void write (char x){
            writeinfo();
            stream->println(x);
        }

        void onReceive(void (*receiveFunc)(char * cArr)) {
            this->receiveFunc = receiveFunc;
        }
    };

    private:
        SoftwareSerial HC12;
        Connect *serialConnect = nullptr;
        Connect *HC12Connect = nullptr;

    public:
        explicit Connection(const SoftwareSerial &HC12) : HC12(HC12) { }

        void begin () {
            HC12.begin(HC12_SPEED);
            Serial.begin(SERIAL_SPEED);
            char id = 'A' + random(25);
            serialConnect = new Connect(&Serial, id);
            HC12Connect = new Connect(&HC12, id);
        }
        void loop() {
            serialConnect->loop();
            HC12Connect->loop();
        }

        void write (char * x){
            serialConnect->write(x);
            HC12Connect->write(x);
        }
        void write (char x){
            serialConnect->write(x);
            HC12Connect->write(x);
        }
        void onReceive(void (*receiveFunc)(char * cArr)) {
            serialConnect->receiveFunc = receiveFunc;
            HC12Connect->receiveFunc = receiveFunc;
        }

        char getID() const {
            return serialConnect->id;
        }
        char getRecvID() const {
            return HC12Connect->id;
        }
        char getChannel() const {
            return serialConnect->channel;
        }
        void setChannel(char channel) {
            serialConnect->channel = channel;
            HC12Connect->channel = channel;
        }

};

#endif
