#ifndef XBEEREMOTE_H
#define XBEEREMOTE_H

#include "Xbee.h"
#include "XbeeFrameRemoteCommandResponse.h"
#include "XbeeCommand.h"

#include <map>

class XbeeLocal;

class XbeeRemote : public Xbee
{
    public:
        XbeeRemote(XbeeAddress addr, XbeeLocal &local);
        virtual ~XbeeRemote();

        virtual bool isInitialized();
        virtual void configurePortFunction(XbeePort::pinID port, XbeePort::pinFunction fnc);

    protected:
        void sendCommand(std::string cmd, uint8_t param);
        void sendCommand(std::string cmd, response_handler &hnd);
        void sendCommand(std::string cmd, uint8_t param, response_handler &hnd);

        static void portConfigured(Xbee *xbee, XbeeCommand &cmd, XbeeCommandResponse &resp);

        static void setResponseByCommand(Xbee *xbee, XbeeCommand &cmd, XbeeCommandResponse &resp);

        void handledRequest() { if (missingRequests > 0) missingRequests--; }
    private:
        XbeeLocal &localXbee;
        unsigned int missingRequests;

        virtual void queryValue(std::string cmd);
        virtual void queryValue(XbeePort::pinID pin);
};

#endif // XBEEREMOTE_H
