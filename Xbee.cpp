#include "Xbee.h"
#include "XbeeLogger.h"
#include "XbeeFrameCommand.h"
#include "XbeeFrameCommandResponse.h"
#include "XbeeFrameDataSample.h"
#include "XbeeFrameRemoteCommandResponse.h"
#include "XbeeCommandResponse.h"

#include <stdexcept>
//#include <iostream>
#include <sstream>

#include <sys/time.h>

using namespace std;

Xbee::Xbee():valueAP(0),valueAO(0),valueID(0),valueVR(0),valueHV(0),portFunction{},digitalValue(0),analogValue{0},initialized(false)
{
}

Xbee::Xbee(XbeeAddress &_addr):addr(_addr),valueAP(0),valueAO(0),valueID(0),valueVR(0),valueHV(0),portFunction{},digitalValue(0),analogValue{0},initialized(false)
{
}

Xbee::~Xbee()
{
    //dtor
}

XbeeAddress& Xbee::getAddress()
{
    return addr;
}

void Xbee::print()
{
    XbeeLogger &log = XbeeLogger::GetInstance();

    log.doLog(string("Address=") + addr.toString(), XbeeLogger::Severity::Info, "Xbee");

    stringstream ss;
    ss << "API enabled(AP)=" << (int)valueAP;
    log.doLog(ss.str(), XbeeLogger::Severity::Debug, "Xbee");

    ss.clear();
    ss.str("API output(AO)=");
    ss << (int)valueAO;
    log.doLog(ss.str(), XbeeLogger::Severity::Debug, "Xbee");

    ss.clear();
    ss.str("PAN ID=0x");
    ss << hex << (int)valueID;
    log.doLog(ss.str(), XbeeLogger::Severity::Debug, "Xbee");

    ss.clear();
    ss.str("Firmware version=0x");
    ss << hex << (int)valueVR;
    log.doLog(ss.str(), XbeeLogger::Severity::Debug, "Xbee");

    for(int i=0;i<XBEE_MAX_PORTS;i++)
        if ((int)portFunction[i] != 0)
        {
            ss.clear();
            ss.str(string());
            ss << getPortName(static_cast<xbee_port>(i)) << ":" << getPortFunctionName(portFunction[i]) << endl;
            log.doLog(ss.str(), XbeeLogger::Severity::Debug, "Xbee");
        }
}

void Xbee::setAddress(uint32_t hi, uint32_t lo, uint16_t net)
{
//    cout << "setAddress:" << hex << hi << " " << lo << endl;
    addr.setAddress(hi, lo, net);
}

string Xbee::getCommandForPort(xbee_port port)
{
    switch(port)
    {
    case xbee_port::P0:
        return CMD(P0);
    case xbee_port::P1:
        return CMD(P1);
    case xbee_port::P2:
        return CMD(P2);
    case xbee_port::P3:
        return CMD(P3);
    case xbee_port::P4:
        return CMD(P4);
    case xbee_port::P5:
        return CMD(P5);
    case xbee_port::P6:
        return CMD(P6);
    case xbee_port::P7:
        return CMD(P7);
    case xbee_port::P8:
        return CMD(P8);
    case xbee_port::P9:
        return CMD(P9);
    case xbee_port::D0:
        return CMD(D0);
    case xbee_port::D1:
        return CMD(D1);
    case xbee_port::D2:
        return CMD(D2);
    case xbee_port::D3:
        return CMD(D3);
    case xbee_port::D4:
        return CMD(D4);
    case xbee_port::D5:
        return CMD(D5);
    case xbee_port::D8:
        return CMD(D8);
    }
    throw invalid_argument("Xbee port does not exists");
}

string Xbee::getPortName(xbee_port port)
{
    switch(port)
    {
    case xbee_port::P0:
        return "P0";
    case xbee_port::P1:
        return "P1/DIO11";
    case xbee_port::P2:
        return "P2/DIO12";
    case xbee_port::P3:
        return "P3/DIO13";
    case xbee_port::P4:
        return "P4/DIO14";
    case xbee_port::P5:
        return "P5/DIO15";
    case xbee_port::P6:
        return "P6/DIO16";
    case xbee_port::P7:
        return "P7/DIO17";
    case xbee_port::P8:
        return "P8/DIO18";
    case xbee_port::P9:
        return "P9/DIO19";
    case xbee_port::D0:
        return "DIO0/AD0";
    case xbee_port::D1:
        return "DIO1/AD1";
    case xbee_port::D2:
        return "DIO2/AD2";
    case xbee_port::D3:
        return "DIO3/AD3";
    case xbee_port::D4:
        return "DIO4";
    case xbee_port::D5:
        return "DIO5";
    case xbee_port::D8:
        return "DIO8";
    }
    return "unknown";
}

string Xbee::getPortFunctionName(xbee_port_function fnc)
{
    switch(fnc)
    {
    case xbee_port_function::UnmonitoredInput:
        return "Unmonitored input";
    case xbee_port_function::Specific:
        return "Port specific";
    case xbee_port_function::AnalogInput:
        return "AD converter";
    case xbee_port_function::DigitalInput:
        return "Digital input";
    case xbee_port_function::DigitalOutputLow:
        return "Digital output low";
    case xbee_port_function::DigitalOutputHigh:
        return "Digital output high";
    }
    return "Unknown";
}

bool Xbee::getDigitalValue(uint8_t index)
{
    return digitalValue[index];
}

void Xbee::setDigitalValue(uint8_t index, bool value)
{
    digitalValue[index] = value;
}

uint16_t Xbee::getAnalogValue(uint8_t index)
{
    return analogValue[index];
}

void Xbee::setAnalogValue(uint8_t index, uint16_t value)
{
    analogValue[index] = value;
}

Xbee::xbee_port_function Xbee::getPortFunction(xbee_port port)
{
    uint8_t index = static_cast<uint8_t>(port);
    if (index < 0 || index >= XBEE_MAX_PORTS)
        throw invalid_argument("XBee port count limited to 20 - see xbee_port enum");

    return portFunction[index];
}

void Xbee::setPortFunction(xbee_port port, xbee_port_function cfg)
{
    uint8_t index = static_cast<uint8_t>(port);
    if (index < 0 || index >= XBEE_MAX_PORTS)
        throw invalid_argument("XBee port count limited to 20 - see xbee_port enum");

    portFunction[index]=cfg;
}

string Xbee::getStatusName(xbee_payload_at_cmd_status stat)
{
    stringstream ss;
    switch(stat)
    {
    case Xbee::xbee_payload_at_cmd_status::OK:
        return "Ok";
    case Xbee::xbee_payload_at_cmd_status::ERROR:
        return "Error";
    case Xbee::xbee_payload_at_cmd_status::INVALID_CMD:
        return "Invalid command";
    case Xbee::xbee_payload_at_cmd_status::INVALID_PARAM:
        return "Invalid parameter";
    default:
            ss << "Unknown:" << (int)stat;
            return ss.str();
    }

}

string Xbee::getTime()
{
    timeval tv;
    gettimeofday(&tv, 0);

    stringstream ss;
    ss << tv.tv_sec << "." << tv.tv_usec;

    return ss.str();
}

string Xbee::getHWVersionString()
{
    stringstream ss;

    ss << "XBee" << (isHWProVersion()?"-Pro ":" ") << getHWType();

    return ss.str();
}

bool Xbee::isHWProVersion()
{
    uint8_t mod = valueHV/0x100;
    return mod == 0x1A || mod == 0x21;
}

std::string Xbee::getHWType()
{
    uint8_t mod = valueHV/0x100;

    switch(mod)
    {
    case 0x19:
    case 0x1A:
        return "S2";
    case 0x21:
    case 0x22:
        return "S2C";
    }

    return "XX";
}

void Xbee::initialize()
{
    try
    {
        updateState();
    } catch (exception &e) {
        XbeeLogger::GetInstance().doLog(string("Cant initialize ") + getAddress().toString() + ":" + e.what(), XbeeLogger::Severity::Warning, "XbeeLocal");
    }

    initialized = true;

}

void Xbee::updateState()
{
    setValueAP(sendCommandWithResponseSync(XBEE_CMD_AP, XbeeFrameCommand::returnType::BYTE));
    setValueAO(sendCommandWithResponseSync(XBEE_CMD_AO, XbeeFrameCommand::returnType::BYTE));
    setValueID(sendCommandWithResponseSync(XBEE_CMD_ID, XbeeFrameCommand::returnType::LONG));
    setValueVR(sendCommandWithResponseSync(XBEE_CMD_VR, XbeeFrameCommand::returnType::SHORT));
    setValueHV(sendCommandWithResponseSync(XBEE_CMD_HV, XbeeFrameCommand::returnType::SHORT));

    updatePortConfig(XBEE_CMD_P0, xbee_port::P0);
    updatePortConfig(XBEE_CMD_P1, xbee_port::P1);
    updatePortConfig(XBEE_CMD_P2, xbee_port::P2);
    updatePortConfig(XBEE_CMD_D0, xbee_port::D0);
    updatePortConfig(XBEE_CMD_D1, xbee_port::D1);
    updatePortConfig(XBEE_CMD_D2, xbee_port::D2);
    updatePortConfig(XBEE_CMD_D3, xbee_port::D3);
    updatePortConfig(XBEE_CMD_D4, xbee_port::D4);
    updatePortConfig(XBEE_CMD_D5, xbee_port::D5);

/*   These ports are not available on XBee24-ZB

    updatePortConfig(XBEE_CMD_P3, xbee_port::P3);
    updatePortConfig(XBEE_CMD_P4, xbee_port::P4);
    updatePortConfig(XBEE_CMD_P5, xbee_port::P5);
    updatePortConfig(XBEE_CMD_P6, xbee_port::P6);
    updatePortConfig(XBEE_CMD_P7, xbee_port::P7);
    updatePortConfig(XBEE_CMD_P8, xbee_port::P8);
    updatePortConfig(XBEE_CMD_P9, xbee_port::P9);
    updatePortConfig(XBEE_CMD_D8, xbee_port::D8);*/
}

void Xbee::updatePortConfig(string cmd, xbee_port p)
{
    try
    {
        uint64_t resp = sendCommandWithResponseSync(cmd, XbeeFrameCommand::returnType::BYTE);
        setPortFunction(p, static_cast<xbee_port_function>(resp));
    } catch (invalid_argument &iaex)
    {
//        stringstream ss;
        XbeeLogger::GetInstance().doLog(string("Cant get port configuration:") + getPortName(p), XbeeLogger::Severity::Warning, "XbeeLocal");
//        cerr << "Cant get port configuration:" << getPortName(p) << endl;
    }
}

/*void Xbee::responseReceived(XbeeFrameRemoteCommandResponse *frm)
{
    uint8_t frmId = frm->getFrameId();

    stringstream ss;
    ss << getTime() << ":Response received from " << getAddress().toString() << " ID " << (int)frmId;
//    cout << getTime() << ":Response received from " << getAddress().toString() << " ID " << (int)frmId << endl;
    XbeeLogger::GetInstance().doLog(ss.str(), XbeeLogger::Severity::Debug, "XbeeRemote");

    callHandler(frmId, frm->getStatus());
}*/

void Xbee::callHandler(uint8_t frmId, XbeeCommandResponse resp)
{
    lock_guard<mutex> blwr(mCallback);

    if (callbackHandlers.count(frmId) != 0)
    {
        response_handler &hnd = callbackHandlers[frmId];

        hnd.cb(this, resp);

        callbackHandlers.erase(frmId);
    }
}

void Xbee::setHandler(uint8_t frmId, response_handler &hnd)
{
    lock_guard<mutex> blwr(mCallback);

    if (callbackHandlers.count(frmId))
    {
        stringstream ss;
        ss << "Previous response not consumed for frame ID " << (int)frmId << " for " << getAddress().toString();
        XbeeLogger::GetInstance().doLog(ss.str(), XbeeLogger::Severity::Warning, "Xbee response handler");
    }

    callbackHandlers[frmId] = hnd;
}

void Xbee::setResponse(Xbee *xbee, XbeeCommandResponse &resp)
{
    xbee->lastResponse = new XbeeCommandResponse(resp);

    stringstream ss;
    ss << (resp.isLocal()?"local ":"") << "command response on " << xbee->getAddress().toString() << " to command " << xbee->lastResponse->getCommand() << endl;
    XbeeLogger::GetInstance().doLog(ss.str(), XbeeLogger::Severity::Info, "Xbee response handler");
}

uint64_t Xbee::sendCommandWithResponseSync(std::string cmd, XbeeFrameCommand::returnType rt)
{
    lock_guard<mutex> guard(mResponse);

    response_handler h = { setResponse } ;
    sendCommand(cmd, h);

    throw runtime_error("wait");
}

uint64_t Xbee::sendCommandWithResponseSync(std::string cmd, uint8_t param, XbeeFrameCommand::returnType rt)
{
    lock_guard<mutex> guard(mResponse);

    response_handler h = { setResponse } ;
    sendCommand(cmd, param, h);

    throw runtime_error("wait");
}

void Xbee::configurePortFunction(xbee_port port, xbee_port_function fnc)
{
    try
    {
        sendCommandWithResponseSync(getCommandForPort(port), static_cast<uint8_t>(fnc), XbeeFrameCommand::returnType::NONE);
//        sendCommand(getCommandForPort(port), static_cast<uint8_t>(fnc));
        setPortFunction(port, fnc);
    } catch (exception &e) {
        XbeeLogger::GetInstance().doLog(string("Cant configure port on ") + getAddress().toString() + ":" + e.what(), XbeeLogger::Severity::Warning, "XbeeLocal");
    }
}
