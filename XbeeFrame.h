#ifndef XBEEFRAME_H
#define XBEEFRAME_H

#include <inttypes.h>

#include <string>

#define XBEE_PACKET_START 0x7e
#define XBEE_FRAME_MAXSIZE 0x1ff

class XbeeFrame
{
    public:
        enum class XbeeApiType:uint8_t {
            AT_RESPONSE=0x88, AT_CMD=0x08, DATA_SAMPLE_RX=0x92, REMOTE_AT_CMD=0x17, REMOTE_AT_RESPONSE=0x97
        };

        struct frame
        {
            uint8_t init;
            uint8_t length_msb;
            uint8_t length_lsb;
            XbeeApiType type;
            uint8_t data[XBEE_FRAME_MAXSIZE];
        } frm;

        static XbeeFrame* createFrame(XbeeFrame::frame *frmData);

        virtual ~XbeeFrame();

        uint16_t getDataSize();
        static uint16_t getDataSize(frame &frmData);

        uint16_t getFullSize();
        uint16_t getFullSize(frame &frmData);

        XbeeFrame::XbeeApiType getType();
        std::string getTypeName();

        virtual void print();

        uint8_t checksum ( const void *bytes, uint_fast8_t length, uint_fast8_t initial);
        virtual uint8_t getCrc();
        uint8_t calculateCrc();

        void validate();

    protected:
        XbeeFrame(XbeeFrame::frame *frmData);
        XbeeFrame(XbeeApiType type, uint16_t size);
    private:
};

#endif // XBEEFRAME_H
