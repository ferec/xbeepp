#ifndef XBEEFRAMEDISCOVERY_H
#define XBEEFRAMEDISCOVERY_H

#include "XbeeFrameCommandResponse.h"
#include "XbeeAddress.h"


class XbeeFrameDiscovery : public XbeeFrameCommandResponse
{
    public:
        XbeeFrameDiscovery(XbeeFrame::frame *frmData);
        virtual ~XbeeFrameDiscovery();

        virtual void print();

        struct discovery_data_1 {
            uint8_t addr_net[2];
            uint8_t addr[8];
            char ni[0];
        };

/*        enum class deviceType:uint8_t {
            Coordinator=0, Router=1, EndDevice=2
        };*/

        struct discovery_data_2 {
            uint8_t addr_parent[2];
            uint8_t dev_type;
            uint8_t status;
            uint8_t profile_id[2];
            uint8_t manufacturer[2];
        };

        XbeeAddress getAddress();
        std::string getNetworkID();
        uint16_t getAddrParent();
        xbeeNetRole getNetRole();
        std::string getNetRoleName();
        virtual const XbeeCommandResponse::status getStatus();
        uint16_t getProfileId();
        uint16_t getManufacturer();

    protected:
    private:
        const discovery_data_1 *disc1;
        const discovery_data_2 *disc2;
};

#endif // XBEEFRAMEDISCOVERY_H
