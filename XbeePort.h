#ifndef XBEEPORT_H_INCLUDED
#define XBEEPORT_H_INCLUDED

#include <string>
#include <vector>

#include "XbeeEnums.h"

class XbeePort
{
    public:
        enum class pinID:uint8_t {
            P0 = 0, // PWM0
            P1 = 1, // PWM1/DIO11
            D11 = P1,
            P2 = 2,
            D12 = P2,
            P3 = 3, // DOUT
            D13 = P3,
            P4 = 4, //DIN
            D14 = P4,
            P5 = 5,
            D15 = P5, // SPI_MISO (SMT module only)
            P6 = 6,
            D16 = P6, // SPI_MOSI (SMT module only)
            P7 = 7,
            D17 = P7, // SPI_SSEL (SMT module only)
            P8 = 8,
            D18 = P8, // SPI_SCLK (SMT module only)
            P9 = 9,
            D19 = P9, // SPI_nATTN/PTI_DATA (SMT module only)
            D0 = 10, // DIO0, Commissioning button
            AD0 = D0,
            D1 = 11, // DIO1, SPI_nATTN (Through-hole module only)
            AD1 = D1,
            D2 = 12, // DIO2, SPI_SCLK (Through-hole module only)
            AD2 = D2,
            D3 = 13, // DIO3, SPI_SSEL (Through-hole module only)
            AD3 = D3,
            D4 = 14, // DIO4, SPI_MOSI (Through-hole module only)
            D5 = 15, // DIO5, Associate configuration
            D6 = 16, // DIO5, Associate configuration
            D7 = 17, // DIO5, Associate configuration
            D8 = 18, // DIO8, DTR/Slp_Rq
            NONE = 0xff
        };

        enum class pinFunction:uint8_t {
            UnmonitoredInput=0, Specific=1, AnalogInput=2, DigitalInput=3, DigitalOutputLow=4, DigitalOutputHigh=5, None=0xff
        };

        static std::string getFunctionName(pinFunction fnc);
        static std::string getName(XbeePort::pinID port);

        static std::vector<pinID> getPins(xbeeVersion hv);

    private:
        pinID pin;
        pinFunction fnc;
};

#endif // XBEEPORT_H_INCLUDED
