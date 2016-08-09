#ifndef XBEE_LOCAL_H
#define XBEE_LOCAL_H

#include "SerialPort.h"
#include "XbeeFrame.h"
#include "XbeeFrameCommandResponse.h"
#include "XbeeFrameDiscovery.h"
#include "Xbee.h"
#include "XbeeRemote.h"

#include <vector>
#include <thread>

#define XBEE_CMD_AP "AP"
#define XBEE_CMD_AO "AO"
#define XBEE_CMD_ID "ID"
#define XBEE_CMD_VR "VR"
#define XBEE_CMD_SH "SH"
#define XBEE_CMD_SL "SL"
#define XBEE_CMD_MY "MY"
#define XBEE_CMD_ND "ND"

//class XbeeRemote;

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

//        static void hex_dump( const void *address, uint16_t length);

        virtual void print();

        virtual bool isLocal(XbeeAddress &addr);
        virtual bool isDiscovered(XbeeAddress &addr);

        XbeeRemote* findDiscoveredDevice(XbeeAddress &addr);
        std::vector<XbeeRemote*> &getDiscoveredDevices() { return discovered; }

        virtual void configurePortFunction(xbee_port port, xbee_port_function fnc);

        void discover();


    protected:
        static void* reader(void *arg);
//        void readerM();

        void readFrameData(XbeeFrame::frame *buffer);

        void writeFrame(XbeeFrame &frame);

        void updateState();
        void updatePortConfig(std::string cmd, xbee_port p);
        void printFrame(XbeeFrame &frm);
        void processFrame(XbeeFrame *frm);

        void sendCommand(std::string cmd);
        void sendCommandWithParameter(std::string cmd, uint8_t param);
        virtual void sendCommandWithParameterAsync(std::string cmd, uint8_t param, response_handler &hnd);

        uint64_t commandWithResponse(std::string cmd, XbeeFrameCommandResponse::returnType);

        void registerXbee(XbeeFrameDiscovery *dsc);
    private:
        SerialPort port;

        pthread_t thReader;
        bool initialized;

        void readAndProcessFrames();

        std::vector<XbeeRemote*> discovered;

//        std::thread bgThread;

        std::mutex mSerial;
//        std::map<uint64_t,XbeeRemote*> listeners;
};

#endif // XBEE_LOCAL_H
