#include <iostream>
#include <stdexcept>

#include <unistd.h>

#include "XbeeLocal.h"
#include "XbeeRemote.h"

using namespace std;


int main(int argc, char *argv[])
{
    XbeeLocal xbee;

    xbee.getSerialPort().setDeviceName("/dev/ttyUSB0");
    xbee.getSerialPort().setBaudrate(B19200);

    try
    {
        xbee.initialize();
//        xbee.print();
    } catch (exception &ex)
    {
        cerr << "cant initialize xbee:" << ex.what() << endl;
        return 1;
    }

    cout << "initialized" << endl;

    xbee.configurePortFunction(Xbee::xbee_port::D0, Xbee::xbee_port_function::DigitalInput);
    xbee.configurePortFunction(Xbee::xbee_port::D1, Xbee::xbee_port_function::DigitalOutputLow);
    xbee.configurePortFunction(Xbee::xbee_port::D2, Xbee::xbee_port_function::DigitalOutputHigh);

    cout << "configured" << endl;

    sleep(5);

    cout << "slept" << endl;

//    xbee.discover();

//    sleep(5);

//    xbee.print();

    auto devs = xbee.getDiscoveredDevices();

    cout << "print discovered devs:" << endl;

    for (auto it=devs.begin(); it!=devs.end();it++)
    {
//        (*it)->print();
        cout << Xbee::getTime() << ":sending remote AT cmd" << endl;
        (*it)->configurePortFunction(Xbee::xbee_port::D1, Xbee::xbee_port_function::DigitalOutputLow);
        (*it)->configurePortFunction(Xbee::xbee_port::D2, Xbee::xbee_port_function::DigitalOutputHigh);
        cout << Xbee::getTime() << ":remote AT cmd sent" << endl;
    }

    sleep(2);

    cout << "End main" << endl;

    xbee.uninit();
    return 0;
}
