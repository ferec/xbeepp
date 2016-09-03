#ifndef XBEEFRAMECOMMANDRESPONSE_H
#define XBEEFRAMECOMMANDRESPONSE_H

#include "XbeeFrame.h"
#include "XbeeCommandResponse.h"

class XbeeFrameCommandResponse : public XbeeFrame, public XbeeCommandResponse
{
    public:
        XbeeFrameCommandResponse();

        static XbeeFrameCommandResponse* createFrame(XbeeFrame::frame *frmData);

        virtual ~XbeeFrameCommandResponse();

        struct frame_response
        {
            uint8_t frame_id;
            char cmd[2];
            XbeeCommandResponse::status status;
            uint8_t value[0];
        };

        virtual void print();

//        virtual const bool isLocal() { return true; }

//        virtual const uint64_t getValue();
        virtual const XbeeCommand::returnType getReturnType();
        virtual const XbeeCommandResponse::status getStatus();
        virtual const std::string getCommandString();

        uint8_t getFrameId() { return frm_data->frame_id; }


        size_t getResponseDataSize();
        size_t getRawValue(uint8_t *buffer, size_t maxlen);

        bool hasRawData();

        uint8_t getByteValue();
        uint16_t getShortValue();
        uint32_t getWordValue();
        uint64_t getLongValue();

//        uint8_t getCrc();
    protected:
        XbeeFrameCommandResponse(XbeeFrame::frame *frmData);
        frame_response *frm_data;


    private:
};

#endif // XBEEFRAMECOMMANDRESPONSE_H
