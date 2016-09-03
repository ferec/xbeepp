#include "XbeeFrameCommandResponse.h"
#include "XbeeFrameDiscovery.h"

#include "XbeeLocal.h"
#include "XbeeLogger.h"

//#include <iostream>
#include <sstream>
#include <stdexcept>

#include <string.h>
#include <stddef.h>

using namespace std;

XbeeFrameCommandResponse::XbeeFrameCommandResponse(XbeeFrame::frame *frmData):XbeeFrame(frmData)
{
    frm_data = reinterpret_cast<frame_response*>(frm.data);
}

XbeeFrameCommandResponse::XbeeFrameCommandResponse():XbeeFrame(XbeeApiType::AT_RESPONSE, 4)
{
    frm_data = reinterpret_cast<frame_response*>(frm.data);
}

XbeeFrameCommandResponse::~XbeeFrameCommandResponse()
{
    //dtor
}

/*uint8_t XbeeFrameCommandResponse::getCrc()
{
    return frm.data[getDataSize()-1];
}*/

uint8_t XbeeFrameCommandResponse::getByteValue()
{
    if (getReturnType() != XbeeCommand::returnType::BYTE)
    {
        stringstream ss;
        ss << "There is no byte response (value length=" << getResponseDataSize() << ") for command " << frm_data->cmd[0] << frm_data->cmd[1];
        throw invalid_argument(ss.str());
    }
    return frm_data->value[0];
}

uint16_t XbeeFrameCommandResponse::getShortValue()
{
    if (getReturnType() != XbeeCommand::returnType::SHORT)
    {
        stringstream ss;
        ss << "There is no short response (value length=" << getResponseDataSize() << ") for command " << frm_data->cmd[0] << frm_data->cmd[1];
        throw invalid_argument(ss.str());
    }
    return be16toh(reinterpret_cast<uint16_t*>(frm_data->value)[0]);
}

uint32_t XbeeFrameCommandResponse::getWordValue()
{
    if (getReturnType() != XbeeCommand::returnType::WORD)
    {
        stringstream ss;
        ss << "There is no word response (value length=" << getResponseDataSize() << ") for command " << frm_data->cmd[0] << frm_data->cmd[1];
        throw invalid_argument(ss.str());
    }
    return be32toh(reinterpret_cast<uint32_t*>(frm_data->value)[0]);
}

uint64_t XbeeFrameCommandResponse::getLongValue()
{
   if (getReturnType() != XbeeCommand::returnType::LONG)
     {
        stringstream ss;
        ss << "There is no long response (value length=" << getResponseDataSize() << ") for command " << frm_data->cmd[0] << frm_data->cmd[1];
        throw invalid_argument(ss.str());
    }
    return be64toh(reinterpret_cast<uint64_t*>(frm_data->value)[0]);
}

size_t XbeeFrameCommandResponse::getResponseDataSize()
{
    return getDataSize() - offsetof(struct frame_response, value) - 1;
//    return getDataSize()-5;
}

size_t XbeeFrameCommandResponse::getRawValue(uint8_t *buffer, size_t maxlen)
{
    size_t len = getResponseDataSize();
    if (len > maxlen)
        throw runtime_error("getRawValue:buffer too small");

    memcpy(buffer, frm_data->value, len);
    return len;
}

void XbeeFrameCommandResponse::print()
{
    string s;
    XbeeFrame::print();

    XbeeLogger &log = XbeeLogger::GetInstance();

    stringstream ss;
    ss << "Response to " << frm_data->cmd[0] << frm_data->cmd[1] << " with result " << XbeeCommandResponse::getStatusName(getStatus()) << " (Frame ID:" << hex << (int)frm_data->frame_id << ")";

    try
    {
        typedef XbeeCommand::returnType rt;

        switch (getReturnType())
        {
        case rt::NONE:
            ss << "no value" << (unsigned int)getByteValue();
            break;
        case rt::BYTE:
            ss << " Value(byte):" << (unsigned int)getByteValue();
            break;
        case rt::SHORT:
            ss << " Value(short):" << (unsigned int)getShortValue();
            break;
        case rt::WORD:
            ss << " Value(4byte):" << (unsigned int)getWordValue();
            break;
        case rt::LONG:
            ss << " Value(8byte):" << (unsigned int)getLongValue();
            break;
        case rt::RAW:
            s.assign(reinterpret_cast<char*>(frm_data->value), getResponseDataSize());
            ss << " raw data:" << s;

//            log.doLog(ss.str(), XbeeLogger::Severity::Info, "XbeeFrameCommandResponse");
            break;
        default:
            ss << "invalid type";
        }

        log.doLog(ss.str(), XbeeLogger::Severity::Info, "XbeeFrameCommandResponse");

        ss.clear();
        ss.str(string());

        ss << "Frame ID:" << hex << (int)frm_data->frame_id << endl;
        ss << "Command:" << frm_data->cmd[0] << frm_data->cmd[1] << endl;
        ss << "Status:" << XbeeCommandResponse::getStatusName(getStatus()) << endl;

        log.doLog(ss.str(), XbeeLogger::Severity::Debug, "XbeeFrameCommandResponse");
    } catch (exception &e) {
        log.doLog(string("print:") + e.what(), XbeeLogger::Severity::Info, "XbeeFrameCommandResponse");
    }

}

const XbeeCommandResponse::status XbeeFrameCommandResponse::getStatus()
{
    return frm_data->status;
}

bool XbeeFrameCommandResponse::hasRawData()
{
    return strncmp(frm_data->cmd, XBEE_CMD_ND, 2) == 0 || strncmp(frm_data->cmd, XBEE_CMD_NI, 2) == 0;
}

XbeeFrameCommandResponse* XbeeFrameCommandResponse::createFrame(XbeeFrame::frame *frmData)
{
    frame_response *fr = reinterpret_cast<frame_response*>(frmData->data);
    if (strncmp(fr->cmd, XBEE_CMD_ND, 2) == 0)
        return new XbeeFrameDiscovery(frmData);
    return new XbeeFrameCommandResponse(frmData);
}

const string XbeeFrameCommandResponse::getCommandString()
{
    return string(frm_data->cmd, 2);
}

const XbeeCommand::returnType XbeeFrameCommandResponse::getReturnType()
{
    typedef XbeeCommand::returnType rt;
    if (hasRawData())
        return rt::RAW;

    switch (getResponseDataSize())
        {
        case 0:
            return rt::NONE;
        case 1:
            return rt::BYTE;
        case 2:
            return rt::SHORT;
        case 4:
            return rt::WORD;
        case 8:
            return rt::LONG;
        }

    stringstream ss;
    ss << "invalid return type (length=" << getResponseDataSize() << ")";
    throw runtime_error(ss.str());
}

/*const uint64_t XbeeFrameCommandResponse::getValue()
{
    if (hasRawData())
        throw runtime_error("XbeeFrameCommandResponse:raw value cannot be returned by getValue, try getRawValue");

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
            throw runtime_error("XbeeFrameRemoteCommandResponse:unknown type returned");
        }

    throw runtime_error("XbeeFrameRemoteCommandResponse:invalid return type");
}
*/
