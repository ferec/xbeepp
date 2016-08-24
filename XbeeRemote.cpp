#include "XbeeRemote.h"
#include "XbeeLocal.h"
#include "XbeeFrameRemoteCommand.h"
#include "XbeeLogger.h"
#include "XbeeCommand.h"
#include "XbeeCommandResponse.h"
#include "XbeeException.h"

#include <sstream>
#include <stdexcept>

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
    XbeeRemote *remote = dynamic_cast<XbeeRemote*>(xbee);
    if (remote != nullptr && cmd.getCommand() == XBEE_CMD_AP)
        remote->setValueAP(resp.getByteValue());
    if (remote != nullptr && cmd.getCommand() == XBEE_CMD_AO)
        remote->setValueAO(resp.getByteValue());
    if (remote != nullptr && cmd.getCommand() == XBEE_CMD_ID)
        remote->setValueID(resp.getLongValue());
    if (remote != nullptr && cmd.getCommand() == XBEE_CMD_VR)
        remote->setValueVR(resp.getShortValue());
    if (remote != nullptr && cmd.getCommand() == XBEE_CMD_HV)
        remote->setValueHV(resp.getShortValue());

    try
    {
        XbeePort::pinID pin = XbeeCommand::getPortForCommand(cmd.getCommand());
        remote->setPortFunction(pin, static_cast<XbeePort::pinFunction>(resp.getByteValue()));
        remote->handledRequest();
    } catch (XbeeException &ex) {
        XbeeLogger::GetInstance().doLog(string("setResponseByCommand:") + ex.what(), XbeeLogger::Severity::Info, "XbeeRemote");
    }

/*    if (cmd.getCommand() == XBEE_CMD_P0)
        remote->setPortFunction(XbeePort::pinID::P0, static_cast<XbeePort::pinFunction>(resp.getByteValue()));
    if (cmd.getCommand() == XBEE_CMD_P1)
        remote->setPortFunction(XbeePort::pinID::P1, static_cast<XbeePort::pinFunction>(resp.getByteValue()));
    if (cmd.getCommand() == XBEE_CMD_P2)
        remote->setPortFunction(XbeePort::pinID::P2, static_cast<XbeePort::pinFunction>(resp.getByteValue()));
    if (cmd.getCommand() == XBEE_CMD_D0)
        remote->setPortFunction(XbeePort::pinID::D0, static_cast<XbeePort::pinFunction>(resp.getByteValue()));
    if (cmd.getCommand() == XBEE_CMD_D1)
        remote->setPortFunction(XbeePort::pinID::D1, static_cast<XbeePort::pinFunction>(resp.getByteValue()));
    if (cmd.getCommand() == XBEE_CMD_D2)
        remote->setPortFunction(XbeePort::pinID::D2, static_cast<XbeePort::pinFunction>(resp.getByteValue()));
    if (cmd.getCommand() == XBEE_CMD_D3)
        remote->setPortFunction(XbeePort::pinID::D3, static_cast<XbeePort::pinFunction>(resp.getByteValue()));
    if (cmd.getCommand() == XBEE_CMD_D4)
        remote->setPortFunction(XbeePort::pinID::D4, static_cast<XbeePort::pinFunction>(resp.getByteValue()));
    if (cmd.getCommand() == XBEE_CMD_D5)
        remote->setPortFunction(XbeePort::pinID::D5, static_cast<XbeePort::pinFunction>(resp.getByteValue()));*/

}

bool XbeeRemote::isInitialized()
{
    return Xbee::isInitialized() && missingRequests == 0;
}

void XbeeRemote::queryValue(string value)
{
    XbeeCommand cmd(value);
    response_handler h = { setResponseByCommand, cmd };

    missingRequests++;
    sendCommand(value, h);
}
