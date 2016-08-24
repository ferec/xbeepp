#ifndef XBEEADDRESS_H
#define XBEEADDRESS_H

#define HEXBASE "0123456789abcdef"

#include <inttypes.h>

#include <string>

class XbeeAddress
{
    public:
        XbeeAddress();
        XbeeAddress(uint32_t hi, uint32_t lo, uint16_t net);

        bool operator == (const XbeeAddress &Ref) const;

        std::string toString();

        void setAddress(const uint32_t hi, const uint32_t lo, const uint16_t net);
        void getAddress64(uint8_t *addr_buf);
        uint64_t getAddressLong() { return addr64; }
        void getAddressNet(uint8_t *addr_buf);

        virtual ~XbeeAddress();
    protected:
    private:
        uint64_t addr64;
        uint16_t netAddr;
};

#endif // XBEEADDRESS_H
