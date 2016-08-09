#include "XbeeRemote.h"
#include "XbeeLocal.h"
#include "XbeeFrameRemoteCommand.h"

#include <iostream>
#include <stdexcept>

using namespace std;

XbeeRemote::XbeeRemote(XbeeAddress _addr, XbeeLocal &local):Xbee(_addr),localXbee(local)
{
    if (_addr == local.getAddress())
        throw runtime_error("Xbee cant be as remote and local at the same time");
}

XbeeRemote::~XbeeRemote()
{
}

void XbeeRemote::sendCommandWithParameter(std::string cmd, uint8_t param)
{
    response_handler h = {};
    sendCommandWithParameterAsync(cmd, param, h);
}

void XbeeRemote::sendCommandWithParameterAsync(std::string cmd, uint8_t param, response_handler &hnd)
{
    uint8_t frmId = nextFrameId();
    XbeeFrameRemoteCommand req(cmd, param, frmId, getAddress());

    if (hnd.cb != NULL)
        handlers[frmId] = hnd;

    localXbee.writeFrame(req);
}

void XbeeRemote::responseReceived(XbeeFrameRemoteCommandResponse *frm)
{

    uint8_t frmId = frm->getFrameId();
    cout << getTime() << ":Response received from " << getAddress().toString() << " ID " << (int)frmId << endl;

    if (handlers.count(frmId) != 0)
    {
        response_handler &hnd = handlers[frmId];

        hnd.cb(hnd.port, hnd.fnc, frm->getStatus());

        handlers.erase(frmId);
    }
}

void XbeeRemote::portConfigured(xbee_port port, xbee_port_function fnc, xbee_payload_at_cmd_status result)
{
    cout << "portConfigured " << Xbee::getPortName(port) << " as " << Xbee::getPortFunctionName(fnc) << " with result:" << Xbee::getStatusName(result) << endl;
}

void XbeeRemote::configurePortFunction(xbee_port port, xbee_port_function fnc)
{
    struct response_handler hnd = { portConfigured, port, fnc };
    sendCommandWithParameterAsync(getCommandForPort(port), static_cast<uint8_t>(fnc), hnd);
}

