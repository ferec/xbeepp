#include "XbeePort.h"

using namespace std;

string XbeePort::getFunctionName(XbeePort::pinFunction fnc)
{
    typedef XbeePort::pinFunction pf;
    switch(fnc)
    {
    case pf::UnmonitoredInput:
        return "Unmonitored input";
    case pf::Specific:
        return "Port specific";
    case pf::AnalogInput:
        return "AD converter";
    case pf::DigitalInput:
        return "Digital input";
    case pf::DigitalOutputLow:
        return "Digital output low";
    case pf::DigitalOutputHigh:
        return "Digital output high";
    case pf::None:
        return "None";
    }
    return "Unknown";
}

string XbeePort::getName(XbeePort::pinID port)
{
    typedef XbeePort::pinID pt;

    switch(port)
    {
    case pt::P0:
        return "P0";
    case pt::P1:
        return "P1/DIO11";
    case pt::P2:
        return "P2/DIO12";
    case pt::P3:
        return "P3/DIO13";
    case pt::P4:
        return "P4/DIO14";
    case pt::P5:
        return "P5/DIO15";
    case pt::P6:
        return "P6/DIO16";
    case pt::P7:
        return "P7/DIO17";
    case pt::P8:
        return "P8/DIO18";
    case pt::P9:
        return "P9/DIO19";
    case pt::D0:
        return "DIO0/AD0";
    case pt::D1:
        return "DIO1/AD1";
    case pt::D2:
        return "DIO2/AD2";
    case pt::D3:
        return "DIO3/AD3";
    case pt::D4:
        return "DIO4";
    case pt::D5:
        return "DIO5";
    case pt::D6:
        return "DIO6";
    case pt::D7:
        return "DIO7";
    case pt::D8:
        return "DIO8";
    case pt::NONE:
        return "NONE";
    }
    return "unknown";
}

vector<XbeePort::pinID> XbeePort::getPins(xbeeVersion hv)
{
    vector<XbeePort::pinID> pins;

    switch (hv)
    {
    case xbeeVersion::S2C:
    case xbeeVersion::S2CPro:
        pins.push_back(pinID::P3);
        pins.push_back(pinID::P4);
        pins.push_back(pinID::P5);
        pins.push_back(pinID::P6);
        pins.push_back(pinID::P7);
        pins.push_back(pinID::P8);
        pins.push_back(pinID::P9);
        pins.push_back(pinID::D8);
    default:
    case xbeeVersion::S2:
    case xbeeVersion::S2Pro:
        pins.push_back(pinID::P0);
        pins.push_back(pinID::P1);
        pins.push_back(pinID::P2);
        pins.push_back(pinID::D0);
        pins.push_back(pinID::D1);
        pins.push_back(pinID::D2);
        pins.push_back(pinID::D3);
        pins.push_back(pinID::D4);
        pins.push_back(pinID::D5);
        pins.push_back(pinID::D6);
        pins.push_back(pinID::D7);
    }


    return pins;
}
