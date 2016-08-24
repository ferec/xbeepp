#include <iostream>
#include <stdexcept>

#include <unistd.h>

#include "XbeeLocal.h"
#include "XbeeRemote.h"
#include "XbeeLogger.h"
#include "utils.h"
#include "XbeeException.h"

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

    typedef XbeePort::pinID pt;
    typedef XbeePort::pinFunction pf;

    try
    {
        xbee.configurePortFunction(pt::D0, pf::DigitalInput);
        cout << "TEST - this should not be visible, Xbee not initialized and exception should be thrown" << endl;
    } catch (XbeeException &ex) {
        cout << "tested - exception is thrown when tried to call a function on noninitialized Xbee" << endl;
    }

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


    xbee.configurePortFunction(pt::D0, pf::DigitalInput);
    xbee.configurePortFunction(pt::D1, pf::DigitalOutputLow);
    xbee.configurePortFunction(pt::D2, pf::DigitalOutputHigh);

    try
    {
        xbee.configurePortFunction(pt::P0, pf::AnalogInput);
    } catch (XbeeException &ex) {
        cout << "tested: P0 cannot be set as Analog input " << ex.what() << endl;
    }

    cout << "configured" << endl;

//    sleep(5);

    cout << "slept" << endl;

//    xbee.discover();

//    sleep(5);

//    xbee.print();

    auto devs = xbee.getDiscoveredDevices();

    cout << "local device:";

    xbee.print();

    cout << "print discovered devs:" << endl;

    try
    {
        for (auto it=devs.begin(); it!=devs.end();it++)
        {
            (*it)->print();
            cout << "HW version:" << (*it)->getHWVersionString() << endl;
            cout << "name:" << (*it)->getName() << endl;
            cout << getTime() << ":sending remote AT cmd" << endl;
            (*it)->configurePortFunction(pt::D1, pf::DigitalOutputLow);
            (*it)->configurePortFunction(pt::D2, pf::DigitalOutputHigh);
            cout << getTime() << ":remote AT cmd sent" << endl;
        }
    } catch (XbeeException &ex) {
        cout << "remote xbee config exception:" << ex.what() << endl;
    }

    sleep(2);

    cout << "End main" << endl;

    cout << "HW version:" << xbee.getHWVersionString() << endl;
    cout << "Name:" << xbee.getName() << endl;
    xbee.uninit();
    return 0;
}
