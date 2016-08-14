#ifndef XBEEFRAMEREMOTECOMMANDRESPONSE_H
#define XBEEFRAMEREMOTECOMMANDRESPONSE_H

#include "XbeeFrame.h"
#include "Xbee.h"


class XbeeFrameRemoteCommandResponse : public XbeeFrame
{
    public:
        XbeeFrameRemoteCommandResponse(XbeeFrame::frame *frmData);
        virtual ~XbeeFrameRemoteCommandResponse();

        struct frame_remote_resp {
            uint8_t frame_id;
            uint8_t addr[8];
            uint8_t addr_net[2];
            char cmd[2];
            Xbee::xbee_payload_at_cmd_status status;
            uint8_t value[4];
            uint8_t crc;
        };

        XbeeAddress getAddress();

        Xbee::xbee_payload_at_cmd_status getStatus();
        std::string getStatusName();
        uint8_t getFrameId() { return frm_data->frame_id; }
        std::string getCommand();
        size_t getReturnDataLength();
        void getRawValue(uint8_t *buffer, size_t buflen);

        bool hasByteData();
        bool hasShortData();
        bool hasWordData();
        bool hasLongData();
        bool hasRawData();

        uint8_t getByteValue();
        uint16_t getShortValue();
        uint32_t getWordValue();
        uint64_t getLongValue();

        virtual void print();

    protected:
    private:
        frame_remote_resp *frm_data;
};

#endif // XBEEFRAMEREMOTECOMMANDRESPONSE_H
