#ifndef XBEEREMOTE_H
#define XBEEREMOTE_H

#include "Xbee.h"
#include "XbeeFrame.h"
#include "XbeeFrameRemoteCommandResponse.h"
//#include "XbeeLocal.h"

#include <map>

class XbeeLocal;

class XbeeRemote : public Xbee
{
    public:
        XbeeRemote(XbeeAddress addr, XbeeLocal &local);
        virtual ~XbeeRemote();

//        void initialize();
//        virtual void responseReceived(XbeeFrameRemoteCommandResponse *frm);

//        virtual void configurePortFunction(xbee_port port, xbee_port_function fnc);

    protected:
        void sendCommand(std::string cmd, uint8_t param);
        void sendCommand(std::string cmd, response_handler &hnd);
        void sendCommand(std::string cmd, uint8_t param, response_handler &hnd);

//        virtual uint64_t sendCommandWithResponseSync(std::string cmd, XbeeFrameCommand::returnType rt);

//        static void portConfigured(xbee_port port, XbeeCommandResponse &resp);
        static void portConfigured(Xbee *xbee, XbeeCommandResponse &resp);

    private:
        XbeeLocal &localXbee;
};

#endif // XBEEREMOTE_H
