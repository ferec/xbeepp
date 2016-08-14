#ifndef XBEECOMMANDRESPONSE_H
#define XBEECOMMANDRESPONSE_H

#include "Xbee.h"

#define XBEE_MAX_RESPONSE_DATA 256

class XbeeFrameCommandResponse;

class XbeeCommandResponse
{
    public:
        XbeeCommandResponse(XbeeFrameCommandResponse *resp);
        XbeeCommandResponse(XbeeFrameRemoteCommandResponse *resp);
        virtual ~XbeeCommandResponse();

        bool isLocal() { return local; }
        Xbee::xbee_payload_at_cmd_status getStatus();
        Xbee::xbee_port getPort() { return port; }
        std::string& getCommand() { return cmd; }
    protected:
    private:
        bool local;
        std::string cmd;
        Xbee::xbee_payload_at_cmd_status status;


        union {
        uint8_t byteValue;
        uint16_t shortValue;
        uint32_t wordValue;
        uint64_t longValue;
        } value;

//        uint8_t raw[XBEE_MAX_RESPONSE_DATA];

        Xbee::xbee_port port;
        Xbee::xbee_port_function fnc;

        XbeeFrameCommand::returnType type;
};

#endif // XBEECOMMANDRESPONSE_H
