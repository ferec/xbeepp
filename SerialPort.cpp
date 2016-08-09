#include "SerialPort.h"
#include "utils.h"

#include <iostream>
#include <stdexcept>

#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

using namespace std;

SerialPort::SerialPort():devOpen(false),baudrate(XBEE_DEFAULT_BAUDRATE),bits(XBEE_DEFAULT_BITS),parity(XBEE_DEFAULT_PARITY),parityOdd(false),
    dblStop(XBEE_DEFAULT_STOP_BITS), flowCtrl(XBEE_DEFAULT_FLOWCONTROL),timeout(XBEE_DEFAULT_TIMEOUT),fd(0)
{
}

SerialPort::~SerialPort()
{
    uninitialize();
}

void SerialPort::setBaudrate(speed_t speed)
{
    baudrate = speed;
}


void SerialPort::setBits(uint8_t _bits)
{
    bits = _bits;
}

void SerialPort::setParityEnabled(bool _parity)
{
    parity = _parity;
}

void SerialPort::setParityOdd(bool odd)
{
    parityOdd = odd;
}

void SerialPort::setDoubleStopBit(bool dbl)
{
    dblStop = dbl;
}

void SerialPort::setFlowControl(bool ctsrts)
{
    flowCtrl = ctsrts;
}

void SerialPort::setReadTimeout(uint16_t dsec)
{
    timeout = dsec;
}

void SerialPort::initialize()
{
    if (devName.length() == 0)
        throw invalid_argument("serial device name not set");
	fd = open( devName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

	if (fd < 0)
        throw runtime_error(strerror(errno));

    struct termios options;

	if (tcgetattr(fd, &options) == -1)
        throw runtime_error(strerror(errno));

	if (cfsetispeed( &options, baudrate) < 0)
        throw runtime_error(strerror(errno));

    if (cfsetospeed( &options, baudrate) < 0)
        throw runtime_error(strerror(errno));

	// Enable the receiver and set local mode...
	options.c_cflag |= (CLOCAL | CREAD);

	// Set bits
	options.c_cflag &= ~CSIZE;

	switch (bits) {
	    case 5:
            options.c_cflag |= CS5;
            break;
	    case 6:
            options.c_cflag |= CS6;
            break;
	    case 7:
            options.c_cflag |= CS7;
            break;
	    case 8:
            options.c_cflag |= CS8;
            break;
        default:
            throw invalid_argument("serial comm bits can only be set to 5-8");
	}

	// Set parity mode
	if (parity)
        options.c_cflag |= PARENB;
    else
        options.c_cflag &= ~PARENB;

	if (parityOdd)
        options.c_cflag |= PARODD;
    else
        options.c_cflag &= ~PARODD;

	// Set stop bits
	if (dblStop)
        options.c_cflag |= CSTOPB;
    else
        options.c_cflag &= ~CSTOPB;

	// Set no flow control
	if (flowCtrl)
        options.c_cflag |= CRTSCTS;
    else
        options.c_cflag &= ~CRTSCTS;

//    cout << "iflags:" << hex << options.c_iflag << endl;

	// Set raw
	cfmakeraw(&options);
//    options.c_lflag &= ~(ICANON | ECHO | ISIG);

	// Set timeout
    options.c_cc[VTIME] = timeout;

	if (tcsetattr( fd, TCSADRAIN, &options) == -1)
        throw runtime_error(strerror(errno));
}

void SerialPort::uninitialize()
{
    if (fd > 0)
    {
        close(fd);
//        cout << "closing dev" << endl;
    }
}

void SerialPort::writeData(uint8_t *buffer, size_t len)
{
    size_t have=0;
    int l;

    lock_guard<mutex> blWr(m_write);
//    lock();
    do
    {
        l = write(fd, buffer+have, len-have);
        if (l>0)
            have+=l;
    } while((l == -1 && errno == EAGAIN) || have < len);

//    unlock();

    if (l == -1)
        throw runtime_error(strerror(errno));
}

void SerialPort::readData(uint8_t *buffer, size_t len)
{

//    cout << "readData:" << len << " buffer:" << (void*)buffer << endl;

    size_t have=0;
    int l;

    lock_guard<mutex> blWr(m_write);

    do
    {
        l = read(fd, buffer+have, len-have);
        //cout << "l=" << l << endl;


        if (l>0)
        {
            cout << "read:" << l;
            hex_dump(buffer+have, l);
            have+=l;
        }

    } while((l == -1 && errno == EAGAIN) || have < len);

    if (l == -1)
    {
        cerr << "read error" << endl;
        throw runtime_error(strerror(errno));
    }
}

uint8_t SerialPort::readByte()
{
    uint8_t buffer;
    int l;
    do
    {
        l = read(fd, &buffer, 1);
    } while(l == -1 && errno == EAGAIN);

    if (l == -1)
        throw runtime_error(strerror(errno));

    return buffer;
}

void SerialPort::wait4Char(char c)
{
    uint8_t buffer;
    int i=0;
    int l;
    do
    {
        l = read(fd, &buffer, 1);

//        if (l>0)
//            cout << "read=" << buffer << endl;
        if (l>0)
            i++;

        if (l<0)
            usleep(100*1000);
    } while((l == -1 && errno == EAGAIN) || buffer != c);

    if (i>1)
        cout << "read " << (i-1) << " chars before frame start" << endl;

    if (l == -1)
        throw runtime_error(strerror(errno));
    if (buffer != c)
        runtime_error("character not received");
}
