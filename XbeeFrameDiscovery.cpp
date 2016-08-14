#include "XbeeFrameDiscovery.h"

#include "XbeeLocal.h"
#include "XbeeLogger.h"

//#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace std;

XbeeFrameDiscovery::XbeeFrameDiscovery(XbeeFrame::frame *frmData):XbeeFrameCommandResponse(frmData)
{
    disc1 = reinterpret_cast<discovery_data_1*>(frm_data->value);

    uint16_t len = XbeeFrameCommandResponse::getReturnDataLength();

    int i;
    for (i=0;i<=len-10;i++)
        if (disc1->ni[i] == 0)
            break;

    if (i >= len-10)
        throw runtime_error("Invalid discover package - NI field not terminated");

//    cout << "ni_len=" << i << endl;
//    cout << "ni=" << disc1->ni << "XXX" << endl;

    disc2 = reinterpret_cast<discovery_data_2*>(disc1->ni + i + 1);

}


XbeeFrameDiscovery::~XbeeFrameDiscovery()
{
    //dtor
}

uint16_t XbeeFrameDiscovery::getAddrParent()
{
    return disc2->addr_parent[0]*0x100+disc2->addr_parent[1];
}

void XbeeFrameDiscovery::print()
{
    XbeeFrameCommandResponse::print();

    stringstream ss;

    ss << "Network ID:" << getNetworkID() << endl;
    ss << "Parent:" << hex << getAddrParent() << endl;
    ss << "Device type:" << getDeviceTypeName() << endl;
    ss << "Device status:" << (int)getStatus() << endl;
    ss << "Profile ID:" << getProfileId() << endl;
    ss << "Manufacturer:" << getManufacturer() << endl;


    XbeeLogger &log = XbeeLogger::GetInstance();

    log.doLog(ss.str(), XbeeLogger::Severity::Debug, "XbeeFrameDiscovery");

    ss.clear();
    ss.str("Discovered ");
    ss << getAddress().toString() << endl;
    log.doLog(ss.str(), XbeeLogger::Severity::Debug, "XbeeFrameDiscovery");

//    XbeeLocal::hex_dump(disc1, getRawDataSize());
}

XbeeAddress XbeeFrameDiscovery::getAddress()
{
    uint32_t *ad = reinterpret_cast<uint32_t*>(disc1->addr);
    return XbeeAddress(be32toh(*ad), be32toh(*(ad+1)), disc1->addr_net[0]*0x100+disc1->addr_net[1]);
}

string XbeeFrameDiscovery::getNetworkID()
{
    return string(disc1->ni);
}

XbeeFrameDiscovery::deviceType XbeeFrameDiscovery::getDeviceType()
{
    if (disc2->dev_type < 0 || disc2->dev_type > 2)
    {
        stringstream ss;
        ss << "Invalid device type " << hex << (int)disc2->dev_type;
        throw runtime_error(ss.str());
    }
    return static_cast<XbeeFrameDiscovery::deviceType>(disc2->dev_type);
}

string XbeeFrameDiscovery::getDeviceTypeName()
{
    switch (getDeviceType())
    {
    case deviceType::Coordinator:
        return "Coordinator";
    case deviceType::Router:
        return "Router";
    case deviceType::EndDevice:
        return "End device";
    default:
        return "Unknown";
    }
}

uint8_t XbeeFrameDiscovery::getStatus()
{
    return disc2->status;
}

uint16_t XbeeFrameDiscovery::getProfileId()
{
    return disc2->profile_id[0]*0x100 + disc2->profile_id[1];
}

uint16_t XbeeFrameDiscovery::getManufacturer()
{
    return disc2->manufacturer[0]*0x100 + disc2->manufacturer[1];
}
