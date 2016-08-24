#include "XbeeCommandResponse.h"

#include <string>
#include <sstream>
#include <stdexcept>

using namespace std;

XbeeCommandResponse::~XbeeCommandResponse()
{
    //dtor
}

string XbeeCommandResponse::getStatusName(status stat)
{
    typedef status ps;
    stringstream ss;
    switch(stat)
    {
    case ps::OK:
        return "Ok";
    case ps::ERROR:
        return "Error";
    case ps::INVALID_CMD:
        return "Invalid command";
    case ps::INVALID_PARAM:
        return "Invalid parameter";
    default:
            ss << "Unknown:" << (int)stat;
            return ss.str();
    }

}

const uint64_t XbeeCommandResponse::getValue()
{
    if (getReturnType() == XbeeCommand::returnType::RAW)
        throw runtime_error("XbeeCommandResponse:raw value cannot be returned by getValue, try getRawValue");

    typedef XbeeCommand::returnType rt;

    switch (getReturnType())
        {
        case rt::NONE:
            return 0;
        case rt::BYTE:
            return getByteValue();
        case rt::SHORT:
            return getShortValue();
        case rt::WORD:
            return getWordValue();
        case rt::LONG:
            return getLongValue();
        default:
            throw runtime_error("XbeeCommandResponse:unknown type returned");
        }

    throw runtime_error("XbeeCommandResponse:invalid return type");
}
