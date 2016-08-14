#ifndef XBEEFRAMECOMMAND_H
#define XBEEFRAMECOMMAND_H

#include "XbeeFrame.h"

#include <string>

class XbeeFrameCommand : public XbeeFrame
{
    public:

        enum class returnType:uint8_t
        {
            NONE=0, BYTE=1, SHORT=2, WORD=4, LONG=8, RAW=9
        };

        XbeeFrameCommand(std::string cmd, uint8_t frmId);
        XbeeFrameCommand(std::string cmd, uint8_t param, uint8_t frmId);
        XbeeFrameCommand(XbeeFrame::frame *frmData);

        virtual ~XbeeFrameCommand();

        struct frame_cmd
        {
            uint8_t frame_id;
            char cmd[2];
            union
            {
                uint8_t param1;
                uint8_t crc;
            } param_specific;
            uint8_t crc_with_param;
        };

        virtual void print();
        uint8_t getFrameId() { return frm_data->frame_id; }
//        uint8_t getCrc();
    protected:

    private:
        frame_cmd *frm_data;

        void setData(std::string cmd, uint8_t frmId);
};

#endif // XBEEFRAMECOMMAND_H
