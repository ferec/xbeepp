#ifndef XBEECOMMANDRESPONSE_H
#define XBEECOMMANDRESPONSE_H

#include "XbeeCommand.h"

#define XBEE_MAX_RESPONSE_DATA 256

class XbeeFrameCommandResponse;

class XbeeCommandResponse
{
    public:

        enum class status:uint8_t
        {
            OK=0, ERROR=1, INVALID_CMD=2, INVALID_PARAM=3
        };

        virtual ~XbeeCommandResponse();

//        virtual const bool isLocal() = 0;
        virtual const status getStatus() = 0;
        static std::string getStatusName(status stat);
        virtual const std::string getCommandString() = 0;

        virtual const uint64_t getValue();
        virtual const XbeeCommand::returnType getReturnType() = 0;

        virtual size_t getRawValue(uint8_t *buf, size_t buflen) = 0;
        virtual size_t getResponseDataSize() = 0;

        virtual uint8_t getByteValue() = 0;
        virtual uint16_t getShortValue() = 0;
        virtual uint32_t getWordValue() = 0;
        virtual uint64_t getLongValue() = 0;

    protected:

    private:

};

#endif // XBEECOMMANDRESPONSE_H
