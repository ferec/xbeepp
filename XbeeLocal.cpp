#include "XbeeLocal.h"
#include "XbeeRemote.h"
#include "XbeeFrameDataSample.h"
#include "XbeeFrameCommand.h"
#include "XbeeCommandResponse.h"
#include "XbeeException.h"

#include "XbeeLogger.h"

#include <stdexcept>
#include <iostream>
#include <sstream>

#include <string.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

XbeeLocal::XbeeLocal():lastFrame(0),stop(false),doRemoteInit(false)
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
//    cout << "starting readFrameData" << endl;
    lock_guard<mutex> blRd(mSerial);
    XbeeFrame::frame *header = reinterpret_cast<XbeeFrame::frame*>(buffer);
    port.wait4Char(XBEE_PACKET_START);

    uint8_t have=0;

    do
    {
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
}

void XbeeLocal::readAndProcessFrames()
{
    try
    {
        XbeeFrame::frame buffer;

        readFrameData(&buffer);

        if (stop)
            return;

        XbeeFrame *f = XbeeFrame::createFrame(&buffer);

        if (stop)
            return;

        f->validate();

        if (stop)
            return;

        processFrame(f);

        if (stop)
            return;

        delete f;
    } catch (exception &ex)
    {
        XbeeLogger::GetInstance().doLog(string("reader:") + ex.what(), XbeeLogger::Severity::Error, "XbeeLocal");
    }
}

void* XbeeLocal::reader(void *arg)
{
    XbeeLocal *xbee = static_cast<XbeeLocal*>(arg);

    usleep(1000000);

    while(!xbee->stop)
    {
        xbee->readAndProcessFrames();
    }

    XbeeLogger::GetInstance().doLog("xbee reader thread stopped", xbee->stop?(XbeeLogger::Severity::Info):(XbeeLogger::Severity::Error), "XbeeLocal");

    return NULL;
}

void XbeeLocal::registerXbee(XbeeFrameDiscovery *dsc)
{
    if (dsc == NULL)
        throw runtime_error("Cant register NULL listener");

    XbeeAddress addr = dsc->getAddress();\

    if (!isLocal(addr) && !isDiscovered(addr))
    {
        XbeeRemote *rxbee = new XbeeRemote(addr, *this);
        rxbee->setName(dsc->getNetworkID());
        XbeeLogger::GetInstance().doLog("initializing remote Xbee " + addr.toString(), XbeeLogger::Severity::Debug, "XbeeLocal");
        rxbee->initialize();
        discovered.push_back(rxbee);

//        cout << "discovered.size:" << discovered.size() << endl;

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
        callHandler(resp->getFrameId(), *resp);
        return;
    }

    XbeeFrameRemoteCommandResponse *rresp = dynamic_cast<XbeeFrameRemoteCommandResponse*>(frm);

    if (rresp != NULL)
    {
        XbeeAddress adr = rresp->getAddress();

        XbeeRemote *remote = findDiscoveredDevice(adr);

        if (remote != NULL)
            remote->callHandler(rresp->getFrameId(), *rresp);

        return;
    }

    XbeeFrameDataSample *sample = dynamic_cast<XbeeFrameDataSample*>(frm);
    if (sample != NULL)
    {
    }

    frm->print();
}

void XbeeLocal::initialize()
{
    try
    {
        port.initialize();
    } catch (exception &ex)
    {
        XbeeLogger::GetInstance().doLog(string("serial init error:") + ex.what(), XbeeLogger::Severity::Error, "XbeeLocal");
        throw;
    }

//    XbeeCommand cmdSH(XBEE_CMD_SH), cmdSL(XBEE_CMD_SL), cmdMY(XBEE_CMD_MY);

    typedef XbeeCommand::returnType rt;
    uint32_t sh = sendPreinitCommandWithResponseSync(XBEE_CMD_SH, rt::WORD);
    uint32_t sl = sendPreinitCommandWithResponseSync(XBEE_CMD_SL, rt::WORD);
    uint32_t my = sendPreinitCommandWithResponseSync(XBEE_CMD_MY, rt::SHORT);

    setAddress(sh, sl, my);

//    XbeeCommand cmdNI(XBEE_CMD_NI);

    char buf[BUFLEN];
    sendPreinitCommandWithResponseSync(XBEE_CMD_NI, reinterpret_cast<uint8_t*>(buf), BUFLEN);

    setName(buf);

    Xbee::initialize();

    pthread_create(&thReader, NULL, reader, this);

    discover();

//    usleep(1500*1000);
}

void XbeeLocal::uninit()
{
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

    if (hnd.cb != NULL)
        setHandler(frmId, hnd);

    writeFrame(req);
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

    if (hnd.cb != NULL)
        setHandler(frmId, hnd);

    writeFrame(req);
}

uint64_t XbeeLocal::sendCommandWithResponseSync(XbeeCommand &cmd, XbeeCommand::returnType rt)
{
    if (!isInitialized())
        return sendPreinitCommandWithResponseSync(cmd.getCommand(), rt);

    return Xbee::sendCommandWithResponseSync(cmd, rt);
}

void XbeeLocal::sendCommandWithResponseSync(XbeeCommand &cmd, uint8_t buf[], size_t buflen)
{
    if (!isInitialized())
        return sendPreinitCommandWithResponseSync(cmd.getCommand(), buf, buflen);

    return Xbee::sendCommandWithResponseSync(cmd, buf, buflen);
}

void XbeeLocal::sendPreinitCommandWithResponseSync(string cmd, uint8_t buf[], size_t buflen)
{
    if (isInitialized())
        throw runtime_error("must not be called after initialization");

    XbeeFrameCommandResponse resp;
    runPreinitCommand(cmd, resp);

    size_t len = resp.getRawValue(buf, buflen-1);

    cout << "raw size:" << len << endl;
    buf[len] = 0;
    return ;
}

void XbeeLocal::readLocalResponseFrameData(XbeeFrameCommandResponse &resp)
{
    int i=5;

    do
    {
        readFrameData(&resp.frm);

        if (resp.getType() != XbeeFrame::XbeeApiType::AT_RESPONSE)
            XbeeLogger::GetInstance().doLog("dropping frame of type " + resp.getTypeName(), XbeeLogger::Severity::Info, "XbeeLocal");

    } while (i-- && resp.getType() != XbeeFrame::XbeeApiType::AT_RESPONSE);

}

void XbeeLocal::runPreinitCommand(string cmd, XbeeFrameCommandResponse &resp)
{
    XbeeFrameCommand req(cmd, nextFrameId());

    time_t last = 0;

    do
    {
        if (time(0) - last > 3)
        {
            // resend request every 3 seconds if correlation answer not received
            writeFrame(req);
            last = time(0);
        }

        readLocalResponseFrameData(resp);

        resp.print();
        resp.validate();

//        hex_dump(reinterpret_cast<unsigned char*>(&resp.frm), resp.getFullSize());
    } while (resp.getFrameId() != req.getFrameId());
}

uint64_t XbeeLocal::sendPreinitCommandWithResponseSync(string cmd, XbeeCommand::returnType rt)
{
    if (isInitialized())
        throw runtime_error("must not be called after initialization");

    XbeeFrameCommandResponse resp;

    runPreinitCommand(cmd, resp);

//    stringstream ss;

    return resp.getValue();

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

void XbeeLocal::queryValue(std::string value)
{
    typedef XbeeCommand::returnType rt;

    if (value == XBEE_CMD_AP)
        setValueAP(sendPreinitCommandWithResponseSync(value, rt::BYTE));
    if (value == XBEE_CMD_AO)
        setValueAO(sendPreinitCommandWithResponseSync(value, rt::BYTE));
    if (value == XBEE_CMD_ID)
        setValueID(sendPreinitCommandWithResponseSync(value, rt::LONG));
    if (value == XBEE_CMD_VR)
        setValueVR(sendPreinitCommandWithResponseSync(value, rt::SHORT));
    if (value == XBEE_CMD_HV)
        setValueHV(sendPreinitCommandWithResponseSync(value, rt::SHORT));


    try
    {
        XbeePort::pinID pin = XbeeCommand::getPortForCommand(value);
        setPortFunction(pin, static_cast<XbeePort::pinFunction>(sendPreinitCommandWithResponseSync(value, rt::BYTE)));
    } catch (XbeeException &ex) {
        XbeeLogger::GetInstance().doLog(string("setResponseByCommand:") + ex.what(), XbeeLogger::Severity::Info, "XbeeLocal");
    }
}
