#include "Xbee.h"

#include <stdexcept>
#include <iostream>
#include <sstream>

#include <sys/time.h>

using namespace std;

Xbee::Xbee():lastFrame(0),valueAP(0),valueAO(0),valueID(0),valueVR(0),portFunction{},digitalValue(0),analogValue{0}
{
    //ctor
}

Xbee::Xbee(XbeeAddress &_addr):addr(_addr),lastFrame(0),valueAP(0),valueAO(0),valueID(0),valueVR(0),portFunction{},digitalValue(0),analogValue{0}
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
    cout << "Address=" << addr.toString() << endl;
    cout << "API enabled(AP)=" << (int)valueAP << endl;
    cout << "API output(AO)=" << (int)valueAO << endl;
    cout << "PAN ID=0x" << hex << (int)valueID << endl;
    cout << "Firmware version=0x" << hex << valueVR << endl;

    for(int i=0;i<XBEE_MAX_PORTS;i++)
        if ((int)portFunction[i] != 0)
            cout << getPortName(static_cast<xbee_port>(i)) << ":" << getPortFunctionName(portFunction[i]) << endl;
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
            ss << "Unknown:" << (int)stat << endl;
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
