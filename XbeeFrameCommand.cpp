#include "XbeeFrameCommand.h"

#include <iostream>

using namespace std;

XbeeFrameCommand::XbeeFrameCommand(std::string cmd, uint8_t param, uint8_t frmId):XbeeFrame(XbeeApiType::AT_CMD, 5)
{
    setData(cmd, frmId);
    frm_data->param_specific.param1 = param;
    frm_data->crc_with_param = checksum(&frm.type, getDataSize(), 0xff);
}

XbeeFrameCommand::XbeeFrameCommand(string cmd, uint8_t frmId):XbeeFrame(XbeeApiType::AT_CMD, 4)
{
    setData(cmd, frmId);
    frm_data->param_specific.crc = checksum(&frm.type, getDataSize(), 0xff);
}

XbeeFrameCommand::~XbeeFrameCommand()
{
    //dtor
}

void XbeeFrameCommand::setData(string cmd, uint8_t frmId)
{
    frm_data = reinterpret_cast<frame_cmd*>(frm.data);
    frm_data->frame_id = frmId;
    frm_data->cmd[0] = cmd[0];
    frm_data->cmd[1] = cmd[1];
}

void XbeeFrameCommand::print()
{
    XbeeFrame::print();
    cout << "Frame ID:" << hex << (int)frm_data->frame_id << endl;
    cout << "Command:" << frm_data->cmd[0] << frm_data->cmd[1] << endl;
}

/*
uint8_t XbeeFrameCommand::getCrc()
{
    return frm.data[getDataSize()-1];
}
*/
