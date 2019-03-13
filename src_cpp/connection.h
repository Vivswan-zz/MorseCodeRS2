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
    const char id = static_cast<char>('A' + random(25));
//        const char id = 'D';

    // Message Syntax: <channel>:<receiver id>@<sender id>:<message><\n>
    class Connect{
    public:
        const char id;
        char recvId = '\0';
        char channel = 'A';

        Stream *stream;

        char buffer[BUFFER_SIZE]{};
        char store[STORAGE_BUFFER_SIZE]{};

        explicit Connect(Stream *stream, const char id): stream(stream), id(id){
            buffer[0] = '\0';
            store[0] = '\0';
        }

        void writeinfo(){
            stream->print(channel);
            stream->print(":*@");
            stream->print(id);
            stream->print(":");
        }

        void isMessage(const char * buff) {
            if (
                    buff[0] == channel &&
                    (buff[2] == id     || buff[2] == '*') &&
                    (buff[4] == recvId || recvId == '\0') &&
                    strlen(buff) < BUFFER_SIZE &&
                    strlen(buff + 6) > 0
            ) {
                stream->println(buff);
                stream->println(buff + 6);
                if (strlen(buff) < STORAGE_BUFFER_SIZE - strlen(store) - 1) {
                    for (int i = 0; i < strlen(buff); ++i) {
                        store[strlen(store) + 1] = '\0';
                        store[strlen(store)] = buff[i];
                    }
                    store[STORAGE_BUFFER_SIZE - 1] = '\0';
                }
                stream->println(store);
            }
        }

        void loop() {
            if (stream->available()) {
                buffer[stream->readBytesUntil('\n', buffer, BUFFER_SIZE)] = '\0';
                isMessage(buffer);
                buffer[0] = '\0';
            }
        }

        void write (char * x){
            writeinfo();
            stream->println(x);
        }

        void write (char x){
            writeinfo();
            stream->println(x);
        }
    };

    private:
        SoftwareSerial HC12;
        Connect *serialConnect = nullptr;
        Connect *HC12Connect = nullptr;

    public:
        explicit Connection(const SoftwareSerial &HC12) : HC12(HC12) {}

        void begin () {
            HC12.begin(HC12_SPEED);
            Serial.begin(SERIAL_SPEED);
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

        int read() {
//            if (Serial.available()){
//                return Serial.read();
//            }
//            if (HC12.available()){
//                return HC12.read();
//            }
            return -1;
        }

        char getID() const {
            return this->id;
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
