#include "XbeeFrameRemoteCommand.h"

using namespace std;

XbeeFrameRemoteCommand::XbeeFrameRemoteCommand(string cmd, uint8_t frmId, XbeeAddress addr):XbeeFrame(XbeeApiType::REMOTE_AT_CMD, 2+8+2+1+2)
{
    setData(cmd, frmId);
    addr.getAddress64(frm_data->addr);
    addr.getAddressNet(frm_data->addr_net);
    frm_data->param_specific.crc = checksum(&frm.type, getDataSize(), 0xff);
}

XbeeFrameRemoteCommand::XbeeFrameRemoteCommand(std::string cmd, uint8_t param, uint8_t frmId, XbeeAddress addr):XbeeFrame(XbeeApiType::REMOTE_AT_CMD, 2+8+2+1+2+1)
{
    setData(cmd, frmId);
    addr.getAddress64(frm_data->addr);
    addr.getAddressNet(frm_data->addr_net);
    frm_data->param_specific.param1 = param;
    frm_data->crc_with_param = checksum(&frm.type, getDataSize(), 0xff);
}

XbeeFrameRemoteCommand::~XbeeFrameRemoteCommand()
{
    //dtor
}

void XbeeFrameRemoteCommand::setData(string cmd, uint8_t frmId)
{
    frm_data = reinterpret_cast<frame_remote_cmd*>(frm.data);
    frm_data->frame_id = frmId;
    frm_data->cmd[0] = cmd[0];
    frm_data->cmd[1] = cmd[1];
    frm_data->options = 2; // Apply changes on remote
}
