#include "XbeeCommandResponse.h"

#include "XbeeFrameCommandResponse.h"
#include "XbeeFrameRemoteCommandResponse.h"

XbeeCommandResponse::XbeeCommandResponse(XbeeFrameCommandResponse *resp):local(true),cmd(resp->getCommand()),status(resp->getStatus()),type(XbeeFrameCommand::returnType::NONE)
{
    if (resp->hasByteData())
    {
        value.byteValue = resp->getByteValue();
        type = XbeeFrameCommand::returnType::BYTE;
        return;
    }
    if (resp->hasShortData())
    {
        value.shortValue = resp->getShortValue();
        type = XbeeFrameCommand::returnType::SHORT;
        return;
    }
    if (resp->hasWordData())
    {
        value.wordValue = resp->getWordValue();
        type = XbeeFrameCommand::returnType::WORD;
        return;

    }
    if (resp->hasLongData())
    {
        value.longValue = resp->getLongValue();
        type = XbeeFrameCommand::returnType::LONG;
        return;
    }

/*    if (resp->hasRawData())
        resp->getRawValue(raw, XBEE_MAX_RESPONSE_DATA);*/
}

XbeeCommandResponse::XbeeCommandResponse(XbeeFrameRemoteCommandResponse *resp):local(false),cmd(resp->getCommand()),status(resp->getStatus()),type(XbeeFrameCommand::returnType::NONE)
{
    if (resp->hasByteData())
        value.byteValue = resp->getByteValue();
    if (resp->hasShortData())
        value.shortValue = resp->getShortValue();
    if (resp->hasWordData())
        value.wordValue = resp->getWordValue();
    if (resp->hasLongData())
        value.longValue = resp->getLongValue();

/*    if (resp->hasRawData())
        resp->getRawValue(raw, XBEE_MAX_RESPONSE_DATA);*/
}

XbeeCommandResponse::~XbeeCommandResponse()
{
    //dtor
}

Xbee::xbee_payload_at_cmd_status XbeeCommandResponse::getStatus()
{
    return status;
}
