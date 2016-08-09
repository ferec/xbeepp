#include "XbeeFrameRemoteCommandResponse.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <stddef.h>

using namespace std;

XbeeFrameRemoteCommandResponse::XbeeFrameRemoteCommandResponse(XbeeFrame::frame *frmData):XbeeFrame(frmData)
{
    //ctor
}

XbeeFrameRemoteCommandResponse::~XbeeFrameRemoteCommandResponse()
{
    //dtor
}

string XbeeFrameRemoteCommandResponse::getCommand()
{
    return string(frm_data->cmd, 2);
}

void XbeeFrameRemoteCommandResponse::print(bool debug)
{
    XbeeFrame::print(debug);

    cout << "Response to remote " << getCommand() << " command ID " << (int)getFrameId() << " with result " << getStatusName();
    if (getReturnDataLength() == 4)
        cout << " (value=" << getWordValue() << ")";
    cout << endl;
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

uint32_t XbeeFrameRemoteCommandResponse::getWordValue()
{
    int dataLen = getReturnDataLength();

    if (dataLen != 4)
    {
        stringstream ss;
        ss << "There is no word response (value length=" << dataLen << ") for command " << frm_data->cmd[0] << frm_data->cmd[1];
        throw invalid_argument(ss.str());
    }
    return be32toh(reinterpret_cast<uint32_t*>(frm_data->value)[0]);
}

XbeeAddress XbeeFrameRemoteCommandResponse::getAddress()
{
    uint32_t *ad = reinterpret_cast<uint32_t*>(frm_data->addr);
    cout << "ad=" << frm_data << endl;
    return XbeeAddress(be32toh(*ad), be32toh(*(ad+1)), frm_data->addr_net[0]*0x100+frm_data->addr_net[1]);
}
