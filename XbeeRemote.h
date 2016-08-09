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

        void responseReceived(XbeeFrameRemoteCommandResponse *frm);

        virtual void configurePortFunction(xbee_port port, xbee_port_function fnc);

    protected:
        void sendCommandWithParameter(std::string cmd, uint8_t param);
        void sendCommandWithParameterAsync(std::string cmd, uint8_t param, response_handler &hnd);


        static void portConfigured(xbee_port port, xbee_port_function fnc, xbee_payload_at_cmd_status result);

    private:
        XbeeLocal &localXbee;

        std::map<uint8_t,response_handler> handlers;
};

#endif // XBEEREMOTE_H
