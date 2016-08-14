#ifndef XBEE_LOCAL_H
#define XBEE_LOCAL_H

#include "SerialPort.h"
#include "XbeeFrame.h"
#include "XbeeFrameCommandResponse.h"
#include "XbeeFrameDiscovery.h"
#include "Xbee.h"
#include "XbeeRemote.h"
#include "XbeeDiscoveryListener.h"

#include <vector>
#include <thread>
#include <mutex>


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

//        virtual void configurePortFunction(xbee_port port, xbee_port_function fnc);

        void discover();


    protected:
        static void* reader(void *arg);
//        void readerM();

        void readFrameData(XbeeFrame::frame *buffer);

        void writeFrame(XbeeFrame &frame);

//        void updateState();
        void printFrame(XbeeFrame &frm);
        void processFrame(XbeeFrame *frm);


//        virtual void sendCommandWithParameterAsync(std::string cmd, uint8_t param, response_handler &hnd);
        virtual void sendCommand(std::string cmd);
        virtual void sendCommand(std::string cmd, response_handler &hnd);

        virtual void sendCommand(std::string cmd, uint8_t param);
        virtual void sendCommand(std::string cmd, uint8_t param, response_handler &hnd);

        virtual uint64_t sendCommandWithResponseSync(std::string cmd, XbeeFrameCommand::returnType rt);

//        void sendPreinitCommand(std::string cmd);
//        void sendPreinitCommandWithParameter(std::string cmd, uint8_t param);

        uint8_t nextFrameId() { return ++lastFrame; }

        void registerXbee(XbeeFrameDiscovery *dsc);
    private:
        SerialPort port;
        uint8_t lastFrame;

        pthread_t thReader;
        bool stop;

        void readAndProcessFrames();

        std::vector<XbeeRemote*> discovered;
        std::vector<XbeeDiscoveryListener*> discoveryListeners;

//        std::thread bgThread;

        std::mutex mSerial;
//        std::map<uint64_t,XbeeRemote*> listeners;
        uint64_t sendPreinitCommandWithResponseSync(std::string cmd, XbeeFrameCommand::returnType);
};

#endif // XBEE_LOCAL_H
