#ifndef XBEEFRAMEDATASAMPLE_H
#define XBEEFRAMEDATASAMPLE_H

#include "XbeeFrame.h"
#include "XbeeAddress.h"


class XbeeFrameDataSample : public XbeeFrame
{
    public:
//        XbeeFrameDataSample();
        XbeeFrameDataSample(XbeeFrame::frame *frmData);
        virtual ~XbeeFrameDataSample();

        struct frame_sample {
            uint8_t addr[8];
            uint8_t addr_net[2];
            uint8_t receive_options;
            uint8_t nr_samples;
            uint8_t digital_mask[2];
            uint8_t analog_mask;
            uint8_t samples[0];
            uint8_t crc;
        };

        XbeeAddress getAddress();
        uint8_t getReceiveOptions();
        uint8_t getNrSamples();
        uint16_t getDigitalMask();
        uint8_t getAnalogMask();
        uint16_t getDigitalValues();
        uint16_t getAnalogValue(uint8_t index);

        void print(bool debug);
    protected:
    private:
        frame_sample *frm_data;
};

#endif // XBEEFRAMEDATASAMPLE_H
