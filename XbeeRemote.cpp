#include "XbeeRemote.h"
#include "XbeeLocal.h"
#include "XbeeFrameRemoteCommand.h"
#include "XbeeLogger.h"
#include "XbeeCommand.h"
#include "XbeeCommandResponse.h"
#include "XbeeException.h"

#include <sstream>
#include <stdexcept>
#include <iostream>

using namespace std;

XbeeRemote::XbeeRemote(XbeeAddress _addr, XbeeLocal &local):Xbee(_addr),localXbee(local),missingRequests(0)
{
    if (_addr == local.getAddress())
        throw runtime_error("Xbee cant be as remote and local at the same time");
}

XbeeRemote::~XbeeRemote()
{
}

void XbeeRemote::sendCommand(std::string cmd, uint8_t param)
{
    response_handler h = {};
    sendCommand(cmd, param, h);
}

void XbeeRemote::sendCommand(std::string cmd, response_handler &hnd)
{
    uint8_t frmId = localXbee.nextFrameId();
    XbeeFrameRemoteCommand req(cmd, frmId, getAddress());

    if (hnd.cb != NULL)
        setHandler(frmId, hnd);

    localXbee.writeFrame(req);
}

void XbeeRemote::sendCommand(std::string cmd, uint8_t param, response_handler &hnd)
{
    uint8_t frmId = localXbee.nextFrameId();
    XbeeFrameRemoteCommand req(cmd, param, frmId, getAddress());

    if (hnd.cb != NULL)
        setHandler(frmId, hnd);

    localXbee.writeFrame(req);
}

void XbeeRemote::portConfigured(Xbee *xbee, XbeeCommand &cmd, XbeeCommandResponse &resp)
{
    XbeeLogger::GetInstance().doLog("portConfigured with result:" + XbeeCommandResponse::getStatusName(resp.getStatus()), XbeeLogger::Severity::Info, "XbeeRemote");
}

void XbeeRemote::configurePortFunction(XbeePort::pinID port, XbeePort::pinFunction fnc)
{
    if (!isInitialized())
    {
        stringstream ss;
        ss << "cant configure while Xbee is not initialized:" << missingRequests;
        throw XbeeException(ss.str());
    }
    Xbee::configurePortFunction(port, fnc);
}

void XbeeRemote::setResponseByCommand(Xbee *xbee, XbeeCommand &cmd, XbeeCommandResponse &resp)
{
    if (xbee == nullptr)
        throw runtime_error("setResponseByCommand:xbee null");

    XbeeRemote *remote = dynamic_cast<XbeeRemote*>(xbee);

    if (remote == nullptr)
        throw runtime_error("setResponseByCommand:xbee not of type expected");


    XbeePort::pinID pin = cmd.getPort();
    if (pin == XbeePort::pinID::NONE)
        remote->setValueByCommand(cmd, resp);
    else
        try
        {
            remote->setPortFunction(pin, static_cast<XbeePort::pinFunction>(resp.getByteValue()));
        } catch (XbeeException &ex) {
            XbeeLogger::GetInstance().doLog(string("setResponseByCommand:") + ex.what(), XbeeLogger::Severity::Info, "XbeeRemote");
        }
    remote->handledRequest();
//    cout << remote->getName() << " handled for " << cmd.getCommand() << ":" << remote->missingRequests << endl;
}

bool XbeeRemote::isInitialized()
{
    return Xbee::isInitialized() && missingRequests == 0;
}

void XbeeRemote::queryValue(string value)
{
    XbeeCommand cmd(value);
    response_handler h = { setResponseByCommand, cmd };

    if (!isInitialized())
    {
        missingRequests++;
//        cout << getName() << " increment for " << value << ":" << missingRequests << endl;
    }

    sendCommand(value, h);
}

void XbeeRemote::queryValue(XbeePort::pinID pin)
{
    XbeeCommand cmd(pin);
    response_handler h = { setResponseByCommand, cmd };

    if (!isInitialized())
    {
        missingRequests++;
//        cout << getName() << " increment for " << cmd.getCommand() << ":" << missingRequests << endl;
    }

    sendCommand(cmd.getCommand(), h);
}
