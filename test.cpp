#include <iostream>
#include <stdexcept>

#include <unistd.h>

#include "XbeeLocal.h"
#include "XbeeRemote.h"
#include "XbeeLogger.h"

using namespace std;

/*void logFnc(string msg, string orig)
{
    cout << msg << endl;
}*/

class TestLogger: public XbeeLogger
{
public:
    virtual void doLog(std::string msg, Severity severity, std::string origin)
    {
//        cout << "my logger" << endl;
        XbeeLogger::doLog("mylogger" + msg, severity, origin);
    }
};

class TestDiscover: public XbeeDiscoveryListener
{
public:
    virtual void xbeeDiscovered(XbeeRemote *remote) { cout << "discovered" << endl; }
};

int main(int argc, char *argv[])
{

    TestLogger myLogger;

    XbeeLogger::setLogger(myLogger);
    XbeeLogger &log = XbeeLogger::GetInstance();
    log.setSeverity(XbeeLogger::Severity::Info);

    log.doLog("hellox", XbeeLogger::Severity::Warning, "main");

    XbeeLocal xbee;

    xbee.getSerialPort().setDeviceName("/dev/ttyUSB0");
    xbee.getSerialPort().setBaudrate(B19200);

    TestDiscover discover;
    xbee.addDiscoveryListener(&discover);

    try
    {
        xbee.initialize();
//        xbee.print();
    } catch (exception &ex)
    {
//        Logger::GetInstance() << "cant initialize xbee:" << ex.what() << endl;
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

    cout << "local device:";

    xbee.print();

    cout << "print discovered devs:" << endl;

    for (auto it=devs.begin(); it!=devs.end();it++)
    {
        (*it)->print();
        cout << Xbee::getTime() << ":sending remote AT cmd" << endl;
        (*it)->configurePortFunction(Xbee::xbee_port::D1, Xbee::xbee_port_function::DigitalOutputLow);
        (*it)->configurePortFunction(Xbee::xbee_port::D2, Xbee::xbee_port_function::DigitalOutputHigh);
        cout << Xbee::getTime() << ":remote AT cmd sent" << endl;
        cout << "HW version:" << (*it)->getHWVersionString() << endl;
    }

    sleep(2);

    cout << "End main" << endl;

    cout << "HW version:" << xbee.getHWVersionString() << endl;
    xbee.uninit();
    return 0;
}
