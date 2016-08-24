#ifndef XBEEFRAMEREMOTECOMMANDRESPONSE_H
#define XBEEFRAMEREMOTECOMMANDRESPONSE_H

#include "XbeeFrame.h"
#include "XbeeCommandResponse.h"
#include "XbeeAddress.h"

class XbeeFrameRemoteCommandResponse : public XbeeFrame, public XbeeCommandResponse
{
    public:
        XbeeFrameRemoteCommandResponse(XbeeFrame::frame *frmData);
        virtual ~XbeeFrameRemoteCommandResponse();

        struct frame_remote_resp {
            uint8_t frame_id;
            uint8_t addr[8];
            uint8_t addr_net[2];
            char cmd[2];
            XbeeCommandResponse::status status;
            uint8_t value[4];
            uint8_t crc;
        };

        XbeeAddress getAddress();

//        virtual const bool isLocal() { return false; }
        virtual const XbeeCommandResponse::status getStatus();

//        virtual const uint64_t getValue();
        virtual const XbeeCommand::returnType getReturnType();
        const std::string getCommandString();

        uint8_t getFrameId() { return frm_data->frame_id; }
        virtual size_t getResponseDataSize();
        size_t getRawValue(uint8_t *buffer, size_t buflen);

        virtual void print();

    protected:
        uint8_t getByteValue();
        uint16_t getShortValue();
        uint32_t getWordValue();
        uint64_t getLongValue();

        bool hasRawData();

    private:
        const frame_remote_resp *frm_data;
};

#endif // XBEEFRAMEREMOTECOMMANDRESPONSE_H
