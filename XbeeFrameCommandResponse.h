#ifndef XBEEFRAMECOMMANDRESPONSE_H
#define XBEEFRAMECOMMANDRESPONSE_H

#include "XbeeFrame.h"
#include "Xbee.h"


class XbeeFrameCommandResponse : public XbeeFrame
{
    public:
        XbeeFrameCommandResponse();

        static XbeeFrameCommandResponse* createFrame(XbeeFrame::frame *frmData);

        virtual ~XbeeFrameCommandResponse();

        enum class returnType:uint8_t
        {
            NONE=0, BYTE=1, SHORT=2, WORD=4, LONG=8, RAW=9
        };

        struct frame_response
        {
            uint8_t frame_id;
            char cmd[2];
            Xbee::xbee_payload_at_cmd_status status;
            uint8_t value[0];
        };

        virtual void print(bool debug);

        Xbee::xbee_payload_at_cmd_status getStatus();
        std::string getStatusName();
        uint8_t getFrameId() { return frm_data->frame_id; }

        std::string getCommand();

        uint8_t getByteValue();
        uint16_t getShortValue();
        uint32_t getWordValue();
        uint64_t getLongValue();

        size_t getReturnDataLength();
        void getRawValue(uint8_t *buffer, size_t buflen);

        bool hasByteData();
        bool hasShortData();
        bool hasWordData();
        bool hasLongData();
        bool hasRawData();

//        uint8_t getCrc();
    protected:
        XbeeFrameCommandResponse(XbeeFrame::frame *frmData);
        frame_response *frm_data;

    private:
};

#endif // XBEEFRAMECOMMANDRESPONSE_H
