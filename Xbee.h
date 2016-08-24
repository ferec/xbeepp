#ifndef XBEE_H
#define XBEE_H

#include "XbeeAddress.h"
#include "XbeeCommandResponse.h"

#include <bitset>
#include <map>
#include <mutex>

#define XBEE_MAX_PORTS 20
#define XBEE_ANALOG_PORTS 4

//class XbeeFrameRemoteCommandResponse;
//class XbeeCommandResponse;

class Xbee
{
    public:
        Xbee();
        Xbee(XbeeAddress &addr);
        virtual ~Xbee();

        void setName(std::string n) { name = n; };
        std::string &getName() { return name; }

        virtual void initialize();
        virtual bool isInitialized() { return initialized; }

        void updateState();

        XbeeAddress& getAddress();

//        uint8_t getValueAP() { return valueAP; }

        XbeePort::pinFunction getPortFunction(XbeePort::pinID port);
        virtual void configurePortFunction(XbeePort::pinID port, XbeePort::pinFunction fnc);

        virtual void print();

        bool getDigitalValue(uint8_t index);
        void setDigitalValue(uint8_t index, bool value);

        uint16_t getAnalogValue(uint8_t index);
        void setAnalogValue(uint8_t index, uint16_t value);

        virtual std::string getHWVersionString();

        virtual bool isHWProVersion();
        virtual std::string getHWType();

//        void responseReceived(XbeeFrameRemoteCommandResponse *frm);
        void callHandler(uint8_t frmId, XbeeCommandResponse &resp);

    protected:
        void setAddress(uint32_t hi, uint32_t lo, uint16_t net);
//        void setPortConfig(xbee_port port, uint8_t cfg);
        virtual void setValueAP(uint8_t ap) { valueAP = ap; }
        uint16_t getValueVR() { return valueVR; }
        uint16_t getValueID() { return valueID; }
        uint8_t getValueAO() { return valueAO; }
        uint16_t getValueHV() { return valueHV; }

        void setValueAO(uint8_t ao) { valueAO = ao; }
        void setValueID(uint16_t id) { valueID = id; }
        void setValueVR(uint16_t vr) { valueVR = vr; }
        void setValueHV(uint16_t hv) { valueHV = hv; }

        virtual void setPortFunction(XbeePort::pinID port, XbeePort::pinFunction fnc);
        void updatePortConfig(XbeePort::pinID p);

        static void setResponse(Xbee *xbee, XbeeCommand &cmd, XbeeCommandResponse &resp);
        typedef void (*response_callback)(Xbee *xbee, XbeeCommand &cmd, XbeeCommandResponse &resp);

        struct response_handler {
            response_callback cb;
            XbeeCommand cmd;
        };

        void setHandler(uint8_t frmId, response_handler &hnd);

        virtual void sendCommand(std::string cmd, uint8_t param) = 0;
        virtual void sendCommand(std::string cmd, response_handler &hnd) = 0;
        virtual void sendCommand(std::string cmd, uint8_t param, response_handler &hnd) = 0;
        virtual uint64_t sendCommandWithResponseSync(XbeeCommand &cmd, XbeeCommand::returnType rt);
        virtual uint64_t sendCommandWithResponseSync(XbeeCommand &cmd, uint8_t param, XbeeCommand::returnType rt);
        virtual void sendCommandWithResponseSync(XbeeCommand &cmd, uint8_t buf[], size_t buflen);


    private:
        std::string name;
        XbeeAddress addr;

        uint8_t valueAP, valueAO;
        uint16_t valueID, valueVR, valueHV;

        XbeePort::pinFunction portFunction[XBEE_MAX_PORTS];
        std::bitset<XBEE_MAX_PORTS> digitalValue;
        uint16_t analogValue[XBEE_ANALOG_PORTS];

        bool initialized;

        std::mutex mCallback, mResponse;
        std::map<uint8_t,response_handler> callbackHandlers;

        XbeeCommand *lastCommand;
        XbeeCommandResponse *lastResponse;

        uint64_t waitforResponseWithCleanup();
        void waitforResponse();
        void cleanupResponse();

        virtual void queryValue(std::string value) = 0;
};

#endif // XBEE_H
