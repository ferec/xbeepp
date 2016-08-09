#include "XbeeFrameCommandResponse.h"
#include "XbeeFrameDiscovery.h"

#include "XbeeLocal.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <string.h>
#include <stddef.h>

using namespace std;

XbeeFrameCommandResponse::XbeeFrameCommandResponse(XbeeFrame::frame *frmData):XbeeFrame(frmData)
{
    frm_data = reinterpret_cast<frame_response*>(frm.data);
}

XbeeFrameCommandResponse::XbeeFrameCommandResponse():XbeeFrame(XbeeApiType::AT_RESPONSE, 4)
{
    frm_data = reinterpret_cast<frame_response*>(frm.data);
}

XbeeFrameCommandResponse::~XbeeFrameCommandResponse()
{
    //dtor
}

/*uint8_t XbeeFrameCommandResponse::getCrc()
{
    return frm.data[getDataSize()-1];
}*/

uint8_t XbeeFrameCommandResponse::getByteValue()
{
    if (getReturnDataLength() != 1)
    {
        stringstream ss;
        ss << "There is no byte response (value length=" << getReturnDataLength() << ") for command " << frm_data->cmd[0] << frm_data->cmd[1];
        throw invalid_argument(ss.str());
    }
    return frm_data->value[0];
}

uint16_t XbeeFrameCommandResponse::getShortValue()
{
    if (getReturnDataLength() != 2)
    {
        stringstream ss;
        ss << "There is no short response (value length=" << getReturnDataLength() << ") for command " << frm_data->cmd[0] << frm_data->cmd[1];
        throw invalid_argument(ss.str());
    }
    return be16toh(reinterpret_cast<uint16_t*>(frm_data->value)[0]);
}

uint32_t XbeeFrameCommandResponse::getWordValue()
{
    if (getReturnDataLength() != 4)
    {
        stringstream ss;
        ss << "There is no word response (value length=" << getReturnDataLength() << ") for command " << frm_data->cmd[0] << frm_data->cmd[1];
        throw invalid_argument(ss.str());
    }
    return be32toh(reinterpret_cast<uint32_t*>(frm_data->value)[0]);
}

uint64_t XbeeFrameCommandResponse::getLongValue()
{
    if (getReturnDataLength() != 8)
    {
        stringstream ss;
        ss << "There is no long response (value length=" << getReturnDataLength() << ") for command " << frm_data->cmd[0] << frm_data->cmd[1];
        throw invalid_argument(ss.str());
    }
    return be64toh(reinterpret_cast<uint64_t*>(frm_data->value)[0]);
}

size_t XbeeFrameCommandResponse::getReturnDataLength()
{
    return getDataSize() - offsetof(struct frame_response, value) - 1;
//    return getDataSize()-5;
}

void XbeeFrameCommandResponse::getRawValue(uint8_t *buffer, size_t buflen)
{
    size_t len = getReturnDataLength();
    if (len > buflen)
        throw runtime_error("getRawValue:buffer too small");

    memcpy(buffer, frm_data->value, len);
}

void XbeeFrameCommandResponse::print(bool debug)
{
    XbeeFrame::print(debug);

    if (debug)
    {
        cout << "Frame ID:" << hex << (int)frm_data->frame_id << endl;
        cout << "Command:" << frm_data->cmd[0] << frm_data->cmd[1] << endl;
        cout << "Status:" << getStatusName() << endl;
        if (hasByteData())
            cout << "Value(byte):" << (unsigned int)getByteValue() << endl;
        if (hasShortData())
            cout << "Value(short):" << (unsigned int)getShortValue() << endl;
        if (hasWordData())
            cout << "Value(32bit):" << getWordValue() << endl;
        if (hasLongData())
            cout << "Value(64bit):" << getLongValue() << endl;
    }
    else
        cout << "Response to " << frm_data->cmd[0] << frm_data->cmd[1] << " with result " << getStatusName() << " (Frame ID:" << hex << (int)frm_data->frame_id << ")" << endl;

/*    if (hasRawData())
    {
        cout << "Value(raw):" << endl;
        uint8_t buffer[100];
        getRawValue(buffer, 100);
        XbeeLocal::hex_dump(buffer, getRawDataSize());
    }*/
}

string XbeeFrameCommandResponse::getStatusName()
{
    return Xbee::getStatusName(frm_data->status);
}

bool XbeeFrameCommandResponse::hasByteData()
{
    return !hasRawData() && getReturnDataLength() == 1;
}

bool XbeeFrameCommandResponse::hasShortData()
{
    return !hasRawData() && getReturnDataLength() == 2;
}

bool XbeeFrameCommandResponse::hasWordData()
{
    return !hasRawData() && getReturnDataLength() == 4;
}

bool XbeeFrameCommandResponse::hasLongData()
{
    return !hasRawData() && getReturnDataLength() == 8;
}

bool XbeeFrameCommandResponse::hasRawData()
{
    return strncmp(frm_data->cmd, XBEE_CMD_ND, 2) == 0;
}

XbeeFrameCommandResponse* XbeeFrameCommandResponse::createFrame(XbeeFrame::frame *frmData)
{
    frame_response *fr = reinterpret_cast<frame_response*>(frmData->data);
    if (strncmp(fr->cmd, XBEE_CMD_ND, 2) == 0)
        return new XbeeFrameDiscovery(frmData);
    return new XbeeFrameCommandResponse(frmData);
}

string XbeeFrameCommandResponse::getCommand()
{
    return string(frm_data->cmd, 2);
}
