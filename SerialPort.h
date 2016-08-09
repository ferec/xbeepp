#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <termios.h>
#include <inttypes.h>

#include <string>
#include <mutex>

#define XBEE_DEFAULT_BAUDRATE B9600     // baud rate 9600
#define XBEE_DEFAULT_BITS 8             // 8 bits
#define XBEE_DEFAULT_PARITY false       // no parity check
#define XBEE_DEFAULT_STOP_BITS false    // single stop bit
#define XBEE_DEFAULT_FLOWCONTROL false  // no flow control
#define XBEE_DEFAULT_TIMEOUT    50      // 50 deciseconds = 5 seconds

class SerialPort
{
    public:
        SerialPort();

        bool isOpen() const { return devOpen; }

        void setBaudrate(speed_t speed); // baudrate defined in termios speed_t
        void setBits(uint8_t bits); // set character size mask 5-8 bits
        void setParityEnabled(bool parity); // sets parity check enabled/disabled
        void setParityOdd(bool odd); // sets odd or even parity if enabled by setParityEnabled
        void setDoubleStopBit(bool dbl); // sets double stop bits instead of one
        void setFlowControl(bool ctsrts); // sets hw flow control to enabled
        void setReadTimeout(uint16_t dsec); // sets read timeout in tenth of seconds (deciseconds)

        void setDeviceName(std::string name) { devName = name; }

        void initialize();
        void uninitialize();

        uint8_t readByte();
        void wait4Char(char c);
        void readData(uint8_t *buffer, size_t len);
        void writeData(uint8_t *buffer, size_t len);

        virtual ~SerialPort();
    protected:
    private:
        bool devOpen;
        std::string devName;

        speed_t baudrate;
        uint8_t bits;
        bool parity, parityOdd, dblStop, flowCtrl;
        uint16_t timeout;
        int fd;

        std::mutex m_write;
};

#endif // SERIALPORT_H
