#include "XbeeFrameDataSample.h"

#include <iostream>
#include <bitset>
#include <stdexcept>

using namespace std;

XbeeFrameDataSample::XbeeFrameDataSample(XbeeFrame::frame *frmData):XbeeFrame(frmData)
{
    frm_data = reinterpret_cast<frame_sample*>(frm.data);
}

XbeeFrameDataSample::~XbeeFrameDataSample()
{
    //dtor
}

XbeeAddress XbeeFrameDataSample::getAddress()
{
    uint32_t *ad = reinterpret_cast<uint32_t*>(frm_data->addr);

    return XbeeAddress(*ad, *(ad+1), frm_data->addr_net[0]*0x100+frm_data->addr_net[1]);
}

void XbeeFrameDataSample::print(bool debug)
{
    XbeeFrame::print(debug);

    if (debug)
    {
        cout << "Address:" << getAddress().toString() << endl;
        cout << "Options:" << hex << (int)getReceiveOptions() << endl;
        cout << "Nr of samples:" << hex << (int)getNrSamples() << endl;
        cout << "Digital mask:" << bitset<16>(getDigitalMask()) << endl;

        uint8_t amask = getAnalogMask();
        cout << "Analog mask:" << bitset<8>(amask) << endl;

        if (getDigitalMask())
            cout << "Digital values:" << bitset<16>(getDigitalValues()) << endl;

        if (amask & 1)
            cout << "ADC0:" << getAnalogValue(0) << endl;
        if (amask & 2)
            cout << "ADC1:" << getAnalogValue(1) << endl;
        if (amask & 4)
            cout << "ADC2:" << getAnalogValue(2) << endl;
        if (amask & 8)
            cout << "ADC3:" << getAnalogValue(3) << endl;
    } else
        cout << "Samples received from " << getAddress().toString() << endl;

//    XbeeLocal::hex_dump(disc1, getRawDataSize());
}

uint8_t XbeeFrameDataSample::getReceiveOptions()
{
    return frm_data->receive_options;
}

uint8_t XbeeFrameDataSample::getNrSamples()
{
    return frm_data->nr_samples;
}

uint16_t XbeeFrameDataSample::getDigitalMask()
{
    return frm_data->digital_mask[0]*0x100+frm_data->digital_mask[1];
}

uint8_t XbeeFrameDataSample::getAnalogMask()
{
    return frm_data->analog_mask;
}

uint16_t XbeeFrameDataSample::getDigitalValues()
{
    if (getDigitalMask() == 0)
        throw runtime_error("There are no digital input ports");
    return frm_data->samples[0]*0x100+frm_data->samples[1];
}

uint16_t XbeeFrameDataSample::getAnalogValue(uint8_t index)
{
    if (index > 4)
        throw invalid_argument("Analog inputs are indexed 0-3");

    uint8_t mask = getAnalogMask();
    if (mask == 0)
        throw runtime_error("There are no analog input ports");

    if (! (mask & (1 << index)))
        throw runtime_error("This ADC is not enabled");

    uint8_t pos = 0;
    for (uint8_t i=0; i<index;i++)
        if ((mask>>i)&1)
            pos++;

    uint8_t *base = frm_data->samples + (getDigitalMask()?2:0);

    return base[pos*2]*0x100+base[pos*2+1];
}
