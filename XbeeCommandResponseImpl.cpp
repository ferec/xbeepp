#include "XbeeCommandResponseImpl.h"

#include "XbeeFrameCommandResponse.h"

#include <stdexcept>
#include <iostream>
#include <sstream>

#include <string.h>

using namespace std;

XbeeCommandResponseImpl::XbeeCommandResponseImpl(XbeeCommandResponse &resp):sCmd(resp.getCommandString()),status(resp.getStatus()),value(resp.getValue()),
    type(resp.getReturnType()),dataSize(resp.getResponseDataSize())
{
/*    XbeeFrameCommandResponse *r = dynamic_cast<XbeeFrameCommandResponse*>(&resp);

    if (r!=nullptr)
        cout << "XbeeCommandResponseImpl construct " << (int)r->getFrameId() << " " << r->getCommandString() << endl;*/

    if (resp.getReturnType() == XbeeCommand::returnType::RAW)
    {
        raw = new uint8_t(dataSize);
        resp.getRawValue(raw, dataSize);
    }
    else
        raw = nullptr;
}

XbeeCommandResponseImpl::~XbeeCommandResponseImpl()
{
    delete raw;
    raw = nullptr;
}

const XbeeCommandResponse::status XbeeCommandResponseImpl::getStatus()
{
    return status;
}

size_t XbeeCommandResponseImpl::getRawValue(uint8_t *buf, size_t buflen)
{
    if (raw == nullptr)
        runtime_error("no raw data");

    if (dataSize > buflen)
        throw runtime_error("XbeeCommandResponseImpl:buffer too small");
    memcpy(buf, raw, dataSize);
    return dataSize;
}

uint8_t XbeeCommandResponseImpl::getByteValue()
{
    if (type != XbeeCommand::returnType::BYTE)
    {
        stringstream ss;
        ss << "There is no byte value (response type=" << (int)getReturnType() << ") for command " << getCommandString();
        throw invalid_argument(ss.str());
    }
    return value;
}

uint16_t XbeeCommandResponseImpl::getShortValue()
{
    if (type != XbeeCommand::returnType::SHORT)
    {
        stringstream ss;
        ss << "There is no short value (response type=" << (int)getReturnType() << ") for command " << getCommandString();
        throw invalid_argument(ss.str());
    }
    return value;
}

uint32_t XbeeCommandResponseImpl::getWordValue()
{
    if (type != XbeeCommand::returnType::WORD)
    {
        stringstream ss;
        ss << "There is no word value (response type=" << (int)getReturnType() << ") for command " << getCommandString();
        throw invalid_argument(ss.str());
    }
    return value;
}

uint64_t XbeeCommandResponseImpl::getLongValue()
{
    if (type != XbeeCommand::returnType::LONG)
    {
        stringstream ss;
        ss << "There is no long value (response type=" << (int)getReturnType() << ") for command " << getCommandString();
        throw invalid_argument(ss.str());
    }
    return value;
}
