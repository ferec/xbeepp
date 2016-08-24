#ifndef XBEECOMMANDRESPONSEIMPL_H
#define XBEECOMMANDRESPONSEIMPL_H

#include <string>

#include "XbeeCommandResponse.h"

class XbeeCommandResponseImpl: public XbeeCommandResponse
{
    public:
        XbeeCommandResponseImpl(XbeeCommandResponse &resp);
        virtual ~XbeeCommandResponseImpl();

//        virtual const bool isLocal() { return local; }
        virtual const std::string getCommandString() { return sCmd; }
        virtual const XbeeCommandResponse::status getStatus();

        virtual const uint64_t getValue() { return value; }
        virtual size_t getRawValue(uint8_t *buf, size_t buflen);
        virtual size_t getResponseDataSize() { return dataSize; }
        virtual const XbeeCommand::returnType getReturnType() { return type; }
    protected:
        virtual uint8_t getByteValue();
        virtual uint16_t getShortValue();
        virtual uint32_t getWordValue();
        virtual uint64_t getLongValue();
    private:
//        const bool local;
        const std::string sCmd;
        const XbeeCommandResponse::status status;

        const uint64_t value;

        const XbeeCommand::returnType type;

        uint8_t *raw;
        const size_t dataSize;
};

#endif // XBEECOMMANDRESPONSEIMPL_H
