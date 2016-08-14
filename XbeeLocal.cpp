#include "XbeeLocal.h"
#include "XbeeRemote.h"
#include "XbeeFrameDataSample.h"
#include "XbeeCommandResponse.h"

#include "XbeeLogger.h"

#include <stdexcept>
#include <iostream>
#include <sstream>

#include <string.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

XbeeLocal::XbeeLocal():lastFrame(0),stop(false)
{
    thReader = PTHREAD_ONCE_INIT;
}

XbeeLocal::~XbeeLocal()
{
}

void XbeeLocal::writeFrame(XbeeFrame &frame)
{
    lock_guard<mutex> blWr(mSerial);

    port.writeData(reinterpret_cast<unsigned char*>(&frame.frm), frame.getFullSize());
    frame.print();
}

void XbeeLocal::readFrameData(XbeeFrame::frame *buffer)
{
//    cout << "readFrameData start" << endl;
    lock_guard<mutex> blRd(mSerial);
    XbeeFrame::frame *header = reinterpret_cast<XbeeFrame::frame*>(buffer);
    port.wait4Char(XBEE_PACKET_START);

//    cout << "readFrameData buffer:" << (void*)buffer << endl;

    uint8_t have=0;

    do
    {
//        port.readData(reinterpret_cast<uint8_t*>(&frame.frm.length_msb)+have, 3-have);
        port.readData(reinterpret_cast<uint8_t*>(&header->length_msb)+have, 3-have);

        if ((uint8_t)header->type == XBEE_PACKET_START)
        {
            have = 0;
            continue;
        }

        if (header->length_msb == XBEE_PACKET_START)
        {
            header->length_msb = header->length_lsb;
            header->length_lsb = static_cast<uint8_t>(header->type);
            have = 2;
        }
    } while(have == 3);

    port.readData(header->data, XbeeFrame::getDataSize(*header));
//    cout << "readFrameData finish" << endl;
}

void XbeeLocal::readAndProcessFrames()
{
    try
    {
//        XbeeFrame f;

        XbeeFrame::frame buffer;

        readFrameData(&buffer);

        if (stop)
            return;

//        cout << "readAndProcessFrames 1" << endl;
        XbeeFrame *f = XbeeFrame::createFrame(&buffer);

//        hex_dump(&buffer, 0x20);

//        cout << "f:" << f << endl;
//        cout << "frm:" << f->frm << endl;
//        hex_dump(f, 0x10);
//        cout << "readAndProcessFrames 2:" << f << endl;
//        f->print();

//        cout << "readAndProcessFrames 3" << endl;

        if (stop)
            return;

        f->validate();

        if (stop)
            return;

        processFrame(f);
//        cout << "readAndProcessFrames 4" << endl;

        if (stop)
            return;

        delete f;
    } catch (exception &ex)
    {
        XbeeLogger::GetInstance().doLog(string("reader:") + ex.what(), XbeeLogger::Severity::Error, "XbeeLocal");
//        cerr << "reader:" << ex.what() << endl;
    }
}

void* XbeeLocal::reader(void *arg)
//void XbeeLocal::readerM()
{
//    cout << "thread" << endl;
//    cout << "INFO:reader thread started" << endl;
    XbeeLocal *xbee = static_cast<XbeeLocal*>(arg);
//    XbeeLocal *xbee = this;


    usleep(1000000);

    while(!xbee->stop)
    {
        xbee->readAndProcessFrames();
    }

    XbeeLogger::GetInstance().doLog("xbee reader thread stopped", XbeeLogger::Severity::Error, "XbeeLocal");

//    cout << "INFO:reader thread finished" << endl;

    return NULL;
}

void XbeeLocal::registerXbee(XbeeFrameDiscovery *dsc)
{
    if (dsc == NULL)
        throw runtime_error("Cant register NULL listener");

    XbeeAddress addr = dsc->getAddress();
//    cout << "registering " << addr.toString() << endl;
//    cout << "isLocal " << isLocal(addr) << endl;
    XbeeLogger::GetInstance().doLog("registering1 remote Xbee " + addr.toString(), XbeeLogger::Severity::Info, "XbeeLocal");

    if (!isLocal(addr) && !isDiscovered(addr))
    {
        XbeeLogger::GetInstance().doLog("registering2 remote Xbee " + addr.toString(), XbeeLogger::Severity::Info, "XbeeLocal");
        XbeeRemote *rxbee = new XbeeRemote(addr, *this);
        XbeeLogger::GetInstance().doLog("initializing remote Xbee " + addr.toString(), XbeeLogger::Severity::Info, "XbeeLocal");
        rxbee->initialize();
        discovered.push_back(rxbee);

        cout << "discovered.size:" << discovered.size() << endl;

        for (auto it=discoveryListeners.begin(); it!=discoveryListeners.end(); it++)
            (*it)->xbeeDiscovered(rxbee);
    }
}

bool XbeeLocal::isDiscovered(XbeeAddress &addr)
{
    return findDiscoveredDevice(addr) != NULL;
}

XbeeRemote* XbeeLocal::findDiscoveredDevice(XbeeAddress &addr)
{
    for(auto it=discovered.begin();it != discovered.end();it++)
        if ((*it)->getAddress() == addr)
            return *it;
    return NULL;
}

bool XbeeLocal::isLocal(XbeeAddress &addr)
{
    return addr == getAddress();
}

void XbeeLocal::processFrame(XbeeFrame *frm)
{
//    cout << "processing frame " << frm->getTypeName() << endl;

    XbeeFrameDiscovery *dsc = dynamic_cast<XbeeFrameDiscovery*>(frm);

    if (dsc != NULL)
    {
        registerXbee(dsc);
        dsc->print();
        return;
    }

    XbeeFrameCommandResponse *resp = dynamic_cast<XbeeFrameCommandResponse*>(frm);

    if (resp != NULL)
    {
//        responseReceived(resp);
        callHandler(resp->getFrameId(), XbeeCommandResponse(resp));
//        XbeeLogger::GetInstance().doLog("TODO: Bind response to request!", XbeeLogger::Severity::Error, "XbeeLocal");
//        cout << "TODO: Bind response to request!" << endl;
//        resp->print();
        return;
    }

    XbeeFrameRemoteCommandResponse *rresp = dynamic_cast<XbeeFrameRemoteCommandResponse*>(frm);

    if (rresp != NULL)
    {
        XbeeAddress adr = rresp->getAddress();

        XbeeRemote *remote = findDiscoveredDevice(adr);

        if (remote != NULL)
            remote->callHandler(rresp->getFrameId(), XbeeCommandResponse(rresp));

        return;
    }

    XbeeFrameDataSample *sample = dynamic_cast<XbeeFrameDataSample*>(frm);
    if (sample != NULL)
    {
//        hex_dump(&sample->frm.data, frm->getFullSize());
//        sample->print();
//        cout << "Data sample received from " << sample->getAddress().toString() << endl;

    }

    frm->print();
}

void XbeeLocal::initialize()
{
//    cout << "INFO:start initialize" << endl;
    try
    {
        port.initialize();

//        cout << "INFO:port initialized" << endl;

//        updateState();
    } catch (exception &ex)
    {
        XbeeLogger::GetInstance().doLog(string("serial init error:") + ex.what(), XbeeLogger::Severity::Error, "XbeeLocal");
//        Logger::GetInstance() << "serial init error:" << ex.what() << endl;
//        cerr << (string("serial init error:") + ex.what()) << endl;
        throw;
    }

    uint32_t sh = sendCommandWithResponseSync(XBEE_CMD_SH, XbeeFrameCommand::returnType::WORD);
    uint32_t sl = sendCommandWithResponseSync(XBEE_CMD_SL, XbeeFrameCommand::returnType::WORD);
    uint32_t my = sendCommandWithResponseSync(XBEE_CMD_MY, XbeeFrameCommand::returnType::SHORT);
    setAddress(sh, sl, my);

    Xbee::initialize();

//    cout << "INFO:state updated" << endl;
    pthread_create(&thReader, NULL, reader, this);

//    bgThread = thread(&XbeeLocal::readerM, this);
//    cout << "thread created" << endl;

    discover();

//    cout << "discover started" << endl;

//    cout << "INFO:finished initialize" << endl;
}

void XbeeLocal::uninit()
{
//    bgThread.join();
    stop = true;
    pthread_join(thReader, NULL);
}

void XbeeLocal::discover()
{
    sendCommand(XBEE_CMD_ND);
}

void XbeeLocal::sendCommand(string cmd)
{
    XbeeFrameCommand req(cmd, nextFrameId());
    writeFrame(req);
}

void XbeeLocal::sendCommand(std::string cmd, response_handler &hnd)
{
    uint8_t frmId = nextFrameId();
    XbeeFrameCommand req(cmd, frmId);

    writeFrame(req);

    if (hnd.cb != NULL)
        setHandler(frmId, hnd);
}

void XbeeLocal::sendCommand(string cmd, uint8_t param)
{
    XbeeFrameCommand req(cmd, param, nextFrameId());
    writeFrame(req);
}

void XbeeLocal::sendCommand(std::string cmd, uint8_t param, response_handler &hnd)
{
    uint8_t frmId = nextFrameId();
    XbeeFrameCommand req(cmd, param, frmId);

    writeFrame(req);

    if (hnd.cb != NULL)
        setHandler(frmId, hnd);
}

uint64_t XbeeLocal::sendCommandWithResponseSync(std::string cmd, XbeeFrameCommand::returnType rt)
{
    if (!isInitialized())
        return sendPreinitCommandWithResponseSync(cmd, rt);

    return Xbee::sendCommandWithResponseSync(cmd, rt);
}

uint64_t XbeeLocal::sendPreinitCommandWithResponseSync(string cmd, XbeeFrameCommand::returnType rt)
{
    if (isInitialized())
        throw runtime_error("must not be called after initialization");

    XbeeFrameCommandResponse resp;
    XbeeFrameCommand req(cmd, nextFrameId());
    //req.print();

    time_t last = 0;

    do
    {
        if (time(0) - last > 3)
        {
            // resend request every 3 seconds if correlation answer not received
            writeFrame(req);
            last = time(0);
        }

        readFrameData(&resp.frm);
//        cout << "frm:" << &resp.frm << endl;

        resp.print();
        resp.validate();

//        hex_dump(reinterpret_cast<unsigned char*>(&resp.frm), resp.getFullSize());
    } while (resp.getFrameId() != req.getFrameId());

    stringstream ss;

    switch(rt)
    {
    case XbeeFrameCommand::returnType::NONE:
        return 0;
    case XbeeFrameCommand::returnType::BYTE:
        return resp.getByteValue();
    case XbeeFrameCommand::returnType::SHORT:
        return resp.getShortValue();
    case XbeeFrameCommand::returnType::WORD:
        return resp.getWordValue();
    case XbeeFrameCommand::returnType::LONG:
        return resp.getLongValue();
    default:
        ss << "Invalid return type " << (int)rt;
        throw invalid_argument(ss.str());
    }
}

XbeeAddress& XbeeLocal::getAddress()
{
    if (!isInitialized())
        throw runtime_error("Not initialized");
    return Xbee::getAddress();
}

void XbeeLocal::print()
{
    XbeeLogger &log = XbeeLogger::GetInstance();

    Xbee::print();
    if (discovered.size() > 0)
    {

        log.doLog("discovered devices:", XbeeLogger::Severity::Info, "XbeeLocal");
        for (auto it=discovered.begin();it!=discovered.end(); it++)
            (*it)->print();
    }
}

void XbeeLocal::addDiscoveryListener(XbeeDiscoveryListener *l)
{
    discoveryListeners.push_back(l);
}
