#ifndef XBEE_H
#define XBEE_H

#include "XbeeAddress.h"
#include "XbeeFrameCommand.h"

#include <bitset>
#include <map>
#include <mutex>

#define XBEE_MAX_PORTS 20
#define XBEE_ANALOG_PORTS 4

#define XBEE_CMD_AP "AP"
#define XBEE_CMD_AO "AO"
#define XBEE_CMD_ID "ID"
#define XBEE_CMD_VR "VR"
#define XBEE_CMD_HV "HV"
#define XBEE_CMD_SH "SH"
#define XBEE_CMD_SL "SL"
#define XBEE_CMD_MY "MY"
#define XBEE_CMD_ND "ND"

#define XBEE_CMD_P0 "P0"
#define XBEE_CMD_P1 "P1"
#define XBEE_CMD_P2 "P2"
#define XBEE_CMD_P3 "P3"
#define XBEE_CMD_P4 "P4"
#define XBEE_CMD_P5 "P5"
#define XBEE_CMD_P6 "P6"
#define XBEE_CMD_P7 "P7"
#define XBEE_CMD_P8 "P8"
#define XBEE_CMD_P9 "P9"
#define XBEE_CMD_D0 "D0"
#define XBEE_CMD_D1 "D1"
#define XBEE_CMD_D2 "D2"
#define XBEE_CMD_D3 "D3"
#define XBEE_CMD_D4 "D4"
#define XBEE_CMD_D5 "D5"
#define XBEE_CMD_D8 "D8"

#define CMD(ID) XBEE_CMD_ ## ID

class XbeeFrameRemoteCommandResponse;
class XbeeCommandResponse;

class Xbee
{
    public:
        Xbee();
        Xbee(XbeeAddress &addr);
        virtual ~Xbee();

        enum class xbee_port:uint8_t {
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
            D8 = 18 // DIO8, DTR/Slp_Rq
        };

        enum class xbee_port_function:uint8_t {
            UnmonitoredInput=0, Specific=1, AnalogInput=2, DigitalInput=3, DigitalOutputLow=4, DigitalOutputHigh=5
        };

        enum class xbee_payload_at_cmd_status:uint8_t
        {
            OK=0, ERROR=1, INVALID_CMD=2, INVALID_PARAM=3
        };


        virtual void initialize();
        bool isInitialized() { return initialized; }

        void updateState();

        static std::string getStatusName(xbee_payload_at_cmd_status stat);

        static std::string getPortName(xbee_port port);
        static std::string getPortFunctionName(xbee_port_function fnc);

        static std::string getTime();

        XbeeAddress& getAddress();


        uint8_t getValueAP() { return valueAP; }

        void setValueAO(uint8_t ao) { valueAO = ao; }

        void setValueID(uint16_t id) { valueID = id; }

        void setValueVR(uint16_t vr) { valueVR = vr; }
        void setValueHV(uint16_t hv) { valueHV = hv; }

        xbee_port_function getPortFunction(xbee_port port);

        virtual void configurePortFunction(xbee_port port, xbee_port_function fnc);

        std::string getCommandForPort(xbee_port port);

        virtual void print();

        bool getDigitalValue(uint8_t index);
        void setDigitalValue(uint8_t index, bool value);

        uint16_t getAnalogValue(uint8_t index);
        void setAnalogValue(uint8_t index, uint16_t value);

        virtual std::string getHWVersionString();

        virtual bool isHWProVersion();
        virtual std::string getHWType();

//        void responseReceived(XbeeFrameRemoteCommandResponse *frm);
        void callHandler(uint8_t frmId, XbeeCommandResponse resp);

    protected:
        void setAddress(uint32_t hi, uint32_t lo, uint16_t net);
//        void setPortConfig(xbee_port port, uint8_t cfg);
        void setValueAP(uint8_t ap) { valueAP = ap; }
        uint16_t getValueVR() { return valueVR; }
        uint16_t getValueID() { return valueID; }
        uint8_t getValueAO() { return valueAO; }
        uint16_t getValueHV() { return valueHV; }

        virtual void setPortFunction(xbee_port port, xbee_port_function fnc);

        void updatePortConfig(std::string cmd, xbee_port p);

        typedef void (*response_callback)(Xbee *xbee, XbeeCommandResponse &resp);

        struct response_handler {
            response_callback cb;
//            xbee_port port;
//            xbee_port_function fnc;
        };

        void setHandler(uint8_t frmId, response_handler &hnd);

        virtual void sendCommand(std::string cmd, uint8_t param) = 0;
        virtual void sendCommand(std::string cmd, response_handler &hnd) = 0;
        virtual void sendCommand(std::string cmd, uint8_t param, response_handler &hnd) = 0;
        virtual uint64_t sendCommandWithResponseSync(std::string cmd, XbeeFrameCommand::returnType rt);
        virtual uint64_t sendCommandWithResponseSync(std::string cmd, uint8_t param, XbeeFrameCommand::returnType rt);

        static void setResponse(Xbee *xbee, XbeeCommandResponse &resp);

    private:
        XbeeAddress addr;

        uint8_t valueAP, valueAO;
        uint16_t valueID, valueVR, valueHV;

        xbee_port_function portFunction[XBEE_MAX_PORTS];
        std::bitset<XBEE_MAX_PORTS> digitalValue;
        uint16_t analogValue[XBEE_ANALOG_PORTS];

        bool initialized;

        std::mutex mCallback, mResponse;
        std::map<uint8_t,response_handler> callbackHandlers;

        XbeeCommandResponse *lastResponse;
};

#endif // XBEE_H
