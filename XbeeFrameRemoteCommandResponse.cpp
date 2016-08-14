#include "XbeeFrameRemoteCommandResponse.h"
#include "XbeeLogger.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <stddef.h>
#include <string.h>

using namespace std;

XbeeFrameRemoteCommandResponse::XbeeFrameRemoteCommandResponse(XbeeFrame::frame *frmData):XbeeFrame(frmData)
{
    frm_data = reinterpret_cast<frame_remote_resp*>(frm.data);
/*    stringstream ss;
    ss << "XbeeFrameRemoteCommandResponse constr: " << hex << (int)frmData->length_msb << (int)frmData->length_lsb;
    XbeeLogger::GetInstance().doLog(ss.str(), XbeeLogger::Severity::Info, "XbeeFrameRemoteCommandResponse");
*/
}

XbeeFrameRemoteCommandResponse::~XbeeFrameRemoteCommandResponse()
{
    //dtor
}

string XbeeFrameRemoteCommandResponse::getCommand()
{
    return string(frm_data->cmd, 2);
}

void XbeeFrameRemoteCommandResponse::print()
{
    XbeeFrame::print();

    stringstream ss;
    ss << "Response to remote " << getCommand() << " command ID " << (int)getFrameId() << " with result " << getStatusName();
    if (getReturnDataLength() == 4)
        ss << " (value=" << getWordValue() << ")";
    ss << endl;

    XbeeLogger::GetInstance().doLog(ss.str(), XbeeLogger::Severity::Info, "XbeeFrameRemoteCommandResponse");
}

string XbeeFrameRemoteCommandResponse::getStatusName()
{
    return Xbee::getStatusName(frm_data->status);
}

Xbee::xbee_payload_at_cmd_status XbeeFrameRemoteCommandResponse::getStatus()
{
    return frm_data->status;
}

size_t XbeeFrameRemoteCommandResponse::getReturnDataLength()
{
    return getDataSize() - offsetof(struct frame_remote_resp, value) - 1;
}

bool XbeeFrameRemoteCommandResponse::hasByteData()
{
    return !hasRawData() && getReturnDataLength() == 1;
}

bool XbeeFrameRemoteCommandResponse::hasShortData()
{
    return !hasRawData() && getReturnDataLength() == 2;
}

bool XbeeFrameRemoteCommandResponse::hasWordData()
{
    return !hasRawData() && getReturnDataLength() == 4;
}

bool XbeeFrameRemoteCommandResponse::hasLongData()
{
    return !hasRawData() && getReturnDataLength() == 8;
}

bool XbeeFrameRemoteCommandResponse::hasRawData()
{
    return string(frm_data->cmd) == XBEE_CMD_ND;
}

uint8_t XbeeFrameRemoteCommandResponse::getByteValue()
{
    int dataLen = getReturnDataLength();

    if (!hasByteData())
    {
        stringstream ss;
        ss << "There is no byte response (value length=" << dataLen << ") for command " << frm_data->cmd[0] << frm_data->cmd[1];
        throw invalid_argument(ss.str());
    }
    return frm_data->value[0];
}

uint16_t XbeeFrameRemoteCommandResponse::getShortValue()
{
    int dataLen = getReturnDataLength();

    if (!hasShortData())
    {
        stringstream ss;
        ss << "There is no short response (value length=" << dataLen << ") for command " << frm_data->cmd[0] << frm_data->cmd[1];
        throw invalid_argument(ss.str());
    }
    return be16toh(reinterpret_cast<uint16_t*>(frm_data->value)[0]);
}

uint32_t XbeeFrameRemoteCommandResponse::getWordValue()
{
    int dataLen = getReturnDataLength();

    if (!hasWordData())
    {
        stringstream ss;
        ss << "There is no word response (value length=" << dataLen << ") for command " << frm_data->cmd[0] << frm_data->cmd[1];
        throw invalid_argument(ss.str());
    }
    return be32toh(reinterpret_cast<uint32_t*>(frm_data->value)[0]);
}

uint64_t XbeeFrameRemoteCommandResponse::getLongValue()
{
    int dataLen = getReturnDataLength();

    if (!hasLongData())
    {
        stringstream ss;
        ss << "There is no long response (value length=" << dataLen << ") for command " << frm_data->cmd[0] << frm_data->cmd[1];
        throw invalid_argument(ss.str());
    }
    return be64toh(reinterpret_cast<uint64_t*>(frm_data->value)[0]);
}

void XbeeFrameRemoteCommandResponse::getRawValue(uint8_t *buffer, size_t maxlen)
{
    size_t len = getReturnDataLength();
    if (len > maxlen)
        throw runtime_error("getRawValue:buffer too small");

    memcpy(buffer, frm_data->value, len);
}

XbeeAddress XbeeFrameRemoteCommandResponse::getAddress()
{
/*    uint32_t *ad = reinterpret_cast<uint32_t*>(frm_data->addr);
    if (ad == NULL)
        throw runtime_error("getAddress:no address");
    cout << "frm_data=" << frm_data << endl;
    cout << "addr=" << frm_data->addr << endl;

    uint32_t ad0 = be32toh(*ad);
    uint32_t ad1 = be32toh(*(ad+1));
    uint8_t na0 = frm_data->addr_net[0];
    uint8_t na1 = frm_data->addr_net[1];
    return XbeeAddress(ad0, ad1, na0*0x100+na1);*/
    uint32_t *ad = reinterpret_cast<uint32_t*>(frm_data->addr);
    return XbeeAddress(be32toh(*ad), be32toh(*(ad+1)), frm_data->addr_net[0]*0x100+frm_data->addr_net[1]);
}
