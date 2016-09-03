#include <iostream>
#include <sstream>
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
    virtual void xbeeDiscovered(XbeeRemote *remote)
        {
            XbeeLogger::GetInstance().doLog("discovered", XbeeLogger::Severity::Info, "DiscoveryListener");
        }
};

int main(int argc, char *argv[])
{

    TestLogger myLogger;

    XbeeLogger::setLogger(myLogger);
    XbeeLogger &log = XbeeLogger::GetInstance();
    log.setSeverity(XbeeLogger::Severity::Info);

    XbeeLocal xbee;

    xbee.getSerialPort().setDeviceName("/dev/ttyUSB0");
    xbee.getSerialPort().setBaudrate(B115200);

    TestDiscover discover;
    xbee.addDiscoveryListener(&discover);

    typedef XbeePort::pinID pt;
    typedef XbeePort::pinFunction pf;

    try
    {
        xbee.configurePortFunction(pt::D0, pf::DigitalInput);
        log.doLog("TEST - this should not be visible, Xbee not initialized and exception should be thrown", XbeeLogger::Severity::Error, "main");
    } catch (XbeeException &ex) {
        log.doLog("TEST - exception is thrown when tried to call a function on noninitialized Xbee", XbeeLogger::Severity::Info, "main");
    }

    try
    {
        xbee.initialize();
//        xbee.print();
    } catch (exception &ex)
    {
//        Logger::GetInstance() << "cant initialize xbee:" << ex.what() << endl;
        log.doLog(string("cant initialize xbee:") + ex.what(), XbeeLogger::Severity::Error, "main");
//        cerr << "cant initialize xbee:" << ex.what() << endl;
        return 1;
    }

    log.doLog("initialized", XbeeLogger::Severity::Info, "main");

    log.doLog(string("HW version:") + xbee.getHWVersionString(), XbeeLogger::Severity::Info, "main");
    log.doLog(string("Name:") + xbee.getName(), XbeeLogger::Severity::Info, "main");
    log.doLog(string("Role:") + xbee.getNetRoleString(), XbeeLogger::Severity::Info, "main");

    xbee.configurePortFunction(pt::D0, pf::DigitalInput);
    xbee.configurePortFunction(pt::D1, pf::DigitalOutputLow);
    xbee.configurePortFunction(pt::D2, pf::DigitalOutputHigh);

    try
    {
        xbee.configurePortFunction(pt::P0, pf::AnalogInput);
        log.doLog("TEST - this should not be visible, P0 cannot be set as Analog input", XbeeLogger::Severity::Error, "main");
    } catch (XbeeException &ex) {
        log.doLog(string("TEST - P0 cannot be set as Analog input ") + ex.what(), XbeeLogger::Severity::Info, "main");
    }

    log.doLog("configured", XbeeLogger::Severity::Info, "main");


    auto devs = xbee.getDiscoveredDevices();

    log.doLog("local Xbee", XbeeLogger::Severity::Info, "main");

    sleep(3);

    xbee.print();

    log.doLog("print discovered remote xbees", XbeeLogger::Severity::Info, "main");

    try
    {
        for (auto it=devs.begin(); it!=devs.end();it++)
        {
            stringstream ss;
            (*it)->print();
            ss << "HW version:" << (*it)->getHWVersionString();
            ss << " name:" << (*it)->getName();
            log.doLog(ss.str(), XbeeLogger::Severity::Info, "main");

            log.doLog(string("Role:") + (*it)->getNetRoleString(), XbeeLogger::Severity::Info, "main");

            ss.clear();
            ss.str(string());
            ss << getTime() << ":sending remote AT cmd";
            log.doLog(ss.str(), XbeeLogger::Severity::Info, "main");
            (*it)->configurePortFunction(pt::D1, pf::DigitalOutputLow);
            (*it)->configurePortFunction(pt::D2, pf::DigitalOutputHigh);
            ss.clear();
            ss.str(string());
            ss << getTime() << ":remote AT cmd sent";
            log.doLog(ss.str(), XbeeLogger::Severity::Info, "main");
        }
    } catch (XbeeException &ex) {
        log.doLog(string("remote xbee config exception:") + ex.what(), XbeeLogger::Severity::Error, "main");
    }

    sleep(2);

    log.doLog("End main", XbeeLogger::Severity::Info, "main");

    xbee.uninit();
    return 0;
}
