#include "XbeeFrame.h"

#include "XbeeFrameCommandResponse.h"
#include "XbeeFrameRemoteCommandResponse.h"
#include "XbeeFrameDataSample.h"

#include <stdexcept>
#include <sstream>
#include <iostream>

#include <string.h>
#include <stdlib.h>

#include "utils.h"

using namespace std;

XbeeFrame::XbeeFrame(XbeeFrame::frame *frmData)
{
    if (frmData == NULL)
        abort();

    memcpy(&frm, frmData, getFullSize(*frmData));

    frm.init = XBEE_PACKET_START;
}

XbeeFrame::XbeeFrame(XbeeApiType _type, uint16_t size)
{
    frm.init = XBEE_PACKET_START;
    frm.type = _type;
    frm.length_msb = size / 0x100;
    frm.length_lsb = size & 0xff;
}

XbeeFrame::~XbeeFrame()
{
    //dtor
}

uint16_t XbeeFrame::getFullSize(frame &frmData)
{
    return getDataSize(frmData)+4;
}

uint16_t XbeeFrame::getFullSize()
{
    return getDataSize()+4;
}

uint16_t XbeeFrame::getDataSize()
{
/*    uint16_t size = frm.length_msb*0xf00 + frm.length_lsb;
    if (size > XBEE_FRAME_MAXSIZE)
        throw runtime_error("Frame too large");*/
    return getDataSize(frm);
}

uint16_t XbeeFrame::getDataSize(frame &frmData)
{
    uint16_t size = frmData.length_msb*0x100 + frmData.length_lsb;
    if (size > XBEE_FRAME_MAXSIZE)
        throw runtime_error("Frame too large");
    return size;
}

XbeeFrame::XbeeApiType XbeeFrame::getType()
{
    return frm.type;
}

string XbeeFrame::getTypeName()
{
    stringstream ss;
    switch(frm.type) {
        case XbeeApiType::AT_CMD:
            return "AT command";
        case XbeeApiType::AT_RESPONSE:
            return "AT response";
        case XbeeApiType::REMOTE_AT_CMD:
            return "Remote AT command";
        case XbeeApiType::REMOTE_AT_RESPONSE:
            return "Remote AT response";
        case XbeeApiType::DATA_SAMPLE_RX:
            return "Data sample IO";
        default:
            ss << "Unknown:" << (int)frm.type << endl;
            return ss.str();
    }
}

void XbeeFrame::print(bool debug)
{
    if (debug)
    {
        cout << "Frame:" << getTypeName() << endl;
        cout << "Data size:" << getDataSize() << endl;
        cout << "Full size:" << getFullSize() << endl;
        cout << "Calculated CRC:" << hex << (int)calculateCrc() << endl;
        cout << "CRC:" << hex << (int)getCrc() << endl;
    }

//    cout << "print frm:" << &frm << endl;
//    hex_dump(&frm, getFullSize());
}

uint8_t XbeeFrame::checksum ( const void *bytes, uint_fast8_t length,
	uint_fast8_t initial)
{
	uint8_t i;
	uint8_t checksum;
	const char *p;

	checksum = initial;
	for (p = (const char *)bytes, i = length; i; ++p, --i)
	{
		checksum -= *p;
	}

	return checksum;
}

uint8_t XbeeFrame::calculateCrc()
{
    return checksum(&frm.type, getDataSize(), 0xff);
}

/*uint8_t XbeeFrame::getCrc()
{
    return -1;
//    throw runtime_error("should not be called");
}*/

void XbeeFrame::validate()
{
    if (getFullSize() > XBEE_FRAME_MAXSIZE)
        abort();

    uint8_t crc1 = getCrc();
    uint8_t crcc = calculateCrc();
    if (crc1 != crcc)
    {
        cerr << "crc1:0x" << hex << (int)crc1 << " crc2:" << (int)crcc << endl;
        throw runtime_error("Frame not valid by CRC");
    }
}

XbeeFrame* XbeeFrame::createFrame(XbeeFrame::frame *frmData)
{
    if (frmData == NULL)
        abort();

//    XbeeFrame *newFrm = NULL;
    switch(frmData->type)
    {
    case XbeeApiType::AT_RESPONSE:
        return XbeeFrameCommandResponse::createFrame(frmData);
    case XbeeApiType::REMOTE_AT_RESPONSE:
        return new XbeeFrameRemoteCommandResponse(frmData);
    case XbeeApiType::DATA_SAMPLE_RX:
        return new XbeeFrameDataSample(frmData);
    default:
        stringstream ss;
        ss << "Unknown frame type (API type=" << hex << static_cast<int>(frmData->type) << ")";
        throw runtime_error(ss.str());

    }

}

uint8_t XbeeFrame::getCrc()
{
    return frm.data[getDataSize()-1];
}
