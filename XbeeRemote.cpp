#include "XbeeRemote.h"
#include "XbeeLocal.h"
#include "XbeeFrameRemoteCommand.h"
#include "XbeeLogger.h"
#include "XbeeCommandResponse.h"

#include <sstream>
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

void XbeeRemote::sendCommand(std::string cmd, uint8_t param)
{
    response_handler h = {};
    sendCommand(cmd, param, h);
}

void XbeeRemote::sendCommand(std::string cmd, response_handler &hnd)
{
    uint8_t frmId = localXbee.nextFrameId();
    XbeeFrameRemoteCommand req(cmd, frmId, getAddress());

    localXbee.writeFrame(req);

    if (hnd.cb != NULL)
        setHandler(frmId, hnd);
}

void XbeeRemote::sendCommand(std::string cmd, uint8_t param, response_handler &hnd)
{
    uint8_t frmId = localXbee.nextFrameId();
    XbeeFrameRemoteCommand req(cmd, param, frmId, getAddress());

    localXbee.writeFrame(req);

    if (hnd.cb != NULL)
        setHandler(frmId, hnd);
}

//void XbeeRemote::portConfigured(xbee_port port, XbeeCommandResponse &resp)
void XbeeRemote::portConfigured(Xbee *xbee, XbeeCommandResponse &resp)
{
//    cout << "portConfigured " << Xbee::getPortName(port) << " as " << Xbee::getPortFunctionName(fnc) << " with result:" << Xbee::getStatusName(result) << endl;
    XbeeLogger::GetInstance().doLog("portConfigured " + Xbee::getPortName(resp.getPort()) + " with result:" + Xbee::getStatusName(resp.getStatus()), XbeeLogger::Severity::Info, "XbeeRemote");
}

/*void XbeeRemote::configurePortFunction(xbee_port port, xbee_port_function fnc)
{
    struct response_handler hnd = { portConfigured //, port, fnc
        };
    sendCommand(getCommandForPort(port), static_cast<uint8_t>(fnc), hnd);
}*/

/*void XbeeRemote::initialize()
{
    XbeeLogger::GetInstance().doLog("initializing " + getAddress().toString(), XbeeLogger::Severity::Info, "XbeeRemote");
}*/
