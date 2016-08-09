#include "XbeeAddress.h"

#include <string.h>

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

using namespace std;

XbeeAddress::XbeeAddress(uint32_t hi, uint32_t lo, uint16_t net)
{
    setAddress(hi, lo, net);
}

XbeeAddress::XbeeAddress():addr64{0},netAddr{0}
{
    //ctor
}

XbeeAddress::~XbeeAddress()
{
    //dtor
}

string XbeeAddress::toString()
{
    stringstream ss;

    ss << hex << setw(16) << setfill('0') << addr64;

    ss << "(" << hex << setw(4) << setfill('0') << netAddr << ")";
    return ss.str();
}

void XbeeAddress::setAddress(uint32_t hi, uint32_t lo, uint16_t net)
{
    addr64 = hi*0x100000000ul + lo;
    netAddr = net;
}

bool XbeeAddress::operator == (const XbeeAddress &ref) const
{
    return addr64 == ref.addr64 && netAddr == ref.netAddr;
}

void XbeeAddress::getAddress64(uint8_t *addr_buf)
{
    uint64_t adr = htobe64(addr64);
    memcpy(addr_buf, &adr, 8);
}

void XbeeAddress::getAddressNet(uint8_t *addr_buf)
{
    addr_buf[0] = netAddr/0x100;
    addr_buf[1] = netAddr & 0xff;
}
