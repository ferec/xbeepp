#ifndef XBEE_LOCAL_H
#define XBEE_LOCAL_H

#include "SerialPort.h"
#include "XbeeFrameDiscovery.h"
#include "XbeeDiscoveryListener.h"

#include <vector>
#include <thread>
#include <mutex>

#define BUFLEN 64

class XbeeLocal:public Xbee
{
    friend class XbeeRemote;

    public:
        XbeeLocal();

        SerialPort& getSerialPort() { return port; }
        XbeeAddress& getAddress();

        void initialize();

        void uninit();

        virtual ~XbeeLocal();

        virtual void print();

        virtual bool isLocal(XbeeAddress &addr);
        virtual bool isDiscovered(XbeeAddress &addr);

        XbeeRemote* findDiscoveredDevice(XbeeAddress &addr);
        std::vector<XbeeRemote*> &getDiscoveredDevices() { return discovered; }
        void addDiscoveryListener(XbeeDiscoveryListener *l);
        void removeDiscoveryListener(XbeeDiscoveryListener *l);

        void discover();


    protected:
        static void* reader(void *arg);
//        void readerM();

        void readFrameData(XbeeFrame::frame *buffer);

        void writeFrame(XbeeFrame &frame);

        void printFrame(XbeeFrame &frm);
        void processFrame(XbeeFrame *frm);


        virtual void sendCommand(std::string cmd);
        virtual void sendCommand(std::string cmd, response_handler &hnd);

        virtual void sendCommand(std::string cmd, uint8_t param);
        virtual void sendCommand(std::string cmd, uint8_t param, response_handler &hnd);

        virtual uint64_t sendCommandWithResponseSync(XbeeCommand &cmd, XbeeCommand::returnType rt);
        virtual void sendCommandWithResponseSync(XbeeCommand &cmd, uint8_t buf[], size_t buflen);

        uint8_t nextFrameId() { return ++lastFrame; }

        void registerXbee(XbeeFrameDiscovery *dsc);
    private:
        SerialPort port;
        uint8_t lastFrame;

        pthread_t thReader;
        bool stop, doRemoteInit;

        void readAndProcessFrames();

        std::vector<XbeeRemote*> discovered;
        std::vector<XbeeDiscoveryListener*> discoveryListeners;

        std::mutex mSerial;

        void runPreinitCommand(std::string cmd, XbeeFrameCommandResponse &resp);
        uint64_t sendPreinitCommandWithResponseSync(std::string cmd, XbeeCommand::returnType);
        void sendPreinitCommandWithResponseSync(std::string cmd, uint8_t buf[], size_t buflen);
        void readLocalResponseFrameData(XbeeFrameCommandResponse &resp);

        virtual void queryValue(std::string value);
};

#endif // XBEE_LOCAL_H
