#ifndef XBEECOMMAND_H
#define XBEECOMMAND_H

#include <string>

#define XBEE_CMD_AP "AP"
#define XBEE_CMD_AO "AO"
#define XBEE_CMD_ID "ID"
#define XBEE_CMD_VR "VR"
#define XBEE_CMD_HV "HV"
#define XBEE_CMD_SH "SH"
#define XBEE_CMD_SL "SL"
#define XBEE_CMD_DH "DH"
#define XBEE_CMD_DL "DL"
#define XBEE_CMD_MY "MY"
#define XBEE_CMD_ND "ND"
#define XBEE_CMD_NI "NI"
#define XBEE_CMD_PR "PR"
#define XBEE_CMD_IR "IR"
#define XBEE_CMD_IC "IC"
#define XBEE_CMD_CE "CE"

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
#define XBEE_CMD_D6 "D6"
#define XBEE_CMD_D7 "D7"
#define XBEE_CMD_D8 "D8"

#define CMD(ID) XBEE_CMD_ ## ID


#include "XbeePort.h"

class XbeeCommand
{
    public:
        enum class returnType:uint8_t
        {
            NONE=0, BYTE=1, SHORT=2, WORD=4, LONG=8, RAW=9
        };


        XbeeCommand();
        XbeeCommand(const std::string cmd);
        XbeeCommand(const XbeePort::pinID port, const XbeePort::pinFunction pf = XbeePort::pinFunction::None);
        virtual ~XbeeCommand();

        virtual const std::string getCommand() { return cmd; }
        virtual const XbeePort::pinID getPort() { return port; }
        virtual const XbeePort::pinFunction getPortFunction() { return portFunction; }

        static std::string getCommandForPort(XbeePort::pinID port);
        static XbeePort::pinID getPortForCommand(std::string cmd);

    protected:

    private:
        std::string cmd;
        XbeePort::pinID port;
        XbeePort::pinFunction portFunction;
};

#endif // XBEECOMMAND_H
