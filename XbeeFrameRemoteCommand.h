#ifndef XBEEFRAMEREMOTECOMMAND_H
#define XBEEFRAMEREMOTECOMMAND_H

#include "XbeeFrame.h"
#include "XbeeAddress.h"

class XbeeFrameRemoteCommand : public XbeeFrame
{
    public:
        XbeeFrameRemoteCommand(std::string cmd, uint8_t frmId, XbeeAddress addr);
        XbeeFrameRemoteCommand(std::string cmd, uint8_t param, uint8_t frmId, XbeeAddress addr);
        virtual ~XbeeFrameRemoteCommand();

        struct frame_remote_cmd
        {
            uint8_t frame_id;
            uint8_t addr[8];
            uint8_t addr_net[2];
            uint8_t options;
            char cmd[2];
            union
            {
                uint8_t param1;
                uint8_t crc;
            } param_specific;
            uint8_t crc_with_param;
        };

    protected:
    private:
        frame_remote_cmd *frm_data;

        void setData(std::string cmd, uint8_t frmId);
};

#endif // XBEEFRAMEREMOTECOMMAND_H
