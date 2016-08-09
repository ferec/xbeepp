#include "XbeeLocal.h"
#include "XbeeRemote.h"
#include "XbeeFrameCommand.h"
#include "XbeeFrameDataSample.h"
#include "XbeeFrameRemoteCommandResponse.h"

#include "utils.h"

#include <stdexcept>
#include <iostream>
#include <sstream>

#include <string.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

XbeeLocal::XbeeLocal():initialized(false)
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

//        cout << "readAndProcessFrames 1" << endl;
        XbeeFrame *f = XbeeFrame::createFrame(&buffer);

//        hex_dump(&buffer, 0x20);

//        cout << "f:" << f << endl;
//        cout << "frm:" << f->frm << endl;
//        hex_dump(f, 0x10);
//        cout << "readAndProcessFrames 2:" << f << endl;
//        f->print();

//        cout << "readAndProcessFrames 3" << endl;
        f->validate();

        processFrame(f);
//        cout << "readAndProcessFrames 4" << endl;

        delete f;
    } catch (exception &ex)
    {
        cerr << "reader:" << ex.what() << endl;
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

    while(true)
    {
        xbee->readAndProcessFrames();
    }

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

    if (!isLocal(addr) && !isDiscovered(addr))
    {
        discovered.push_back(new XbeeRemote(addr, *this));
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
        dsc->print(DEBUG);
        return;
    }

    XbeeFrameCommandResponse *resp = dynamic_cast<XbeeFrameCommandResponse*>(frm);

    if (resp != NULL)
    {
        cout << "TODO: Bind response to request!" << endl;
        resp->print(DEBUG);
        return;
    }

    XbeeFrameRemoteCommandResponse *rresp = dynamic_cast<XbeeFrameRemoteCommandResponse*>(frm);

    if (rresp != NULL)
    {
        XbeeAddress adr = rresp->getAddress();

        XbeeRemote *remote = findDiscoveredDevice(adr);

        if (remote != NULL)
            remote->responseReceived(rresp);

        return;
    }

    XbeeFrameDataSample *sample = dynamic_cast<XbeeFrameDataSample*>(frm);
    if (sample != NULL)
    {
//        hex_dump(&sample->frm.data, frm->getFullSize());
//        sample->print();
//        cout << "Data sample received from " << sample->getAddress().toString() << endl;

    }

    frm->print(DEBUG);
}

void XbeeLocal::initialize()
{
//    cout << "INFO:start initialize" << endl;
    try
    {
        port.initialize();

//        cout << "INFO:port initialized" << endl;

        updateState();
    } catch (exception &ex)
    {
        cerr << "serial init error:" << ex.what() << endl;
        throw;
    }

    initialized = true;

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

void XbeeLocal::sendCommandWithParameterAsync(std::string cmd, uint8_t param, response_handler &hnd)
{
    throw runtime_error("not implemented");
}

void XbeeLocal::sendCommandWithParameter(string cmd, uint8_t param)
{
    XbeeFrameCommand req(cmd, param, nextFrameId());
    writeFrame(req);
}

uint64_t XbeeLocal::commandWithResponse(string cmd, XbeeFrameCommandResponse::returnType rt)
{
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

        if (isInitialized())
            throw runtime_error("should be handled in async mode");

        readFrameData(&resp.frm);
//        cout << "frm:" << &resp.frm << endl;

        resp.print(DEBUG);
        resp.validate();

//        hex_dump(reinterpret_cast<unsigned char*>(&resp.frm), resp.getFullSize());
    } while (resp.getFrameId() != req.getFrameId());

    stringstream ss;

    switch(rt)
    {
    case XbeeFrameCommandResponse::returnType::NONE:
        return 0;
    case XbeeFrameCommandResponse::returnType::BYTE:
        return resp.getByteValue();
    case XbeeFrameCommandResponse::returnType::SHORT:
        return resp.getShortValue();
    case XbeeFrameCommandResponse::returnType::WORD:
        return resp.getWordValue();
    case XbeeFrameCommandResponse::returnType::LONG:
        return resp.getLongValue();
    default:
        ss << "Invalid return type " << (int)rt;
        throw invalid_argument(ss.str());
    }
}

void XbeeLocal::updateState()
{
    setValueAP(commandWithResponse(XBEE_CMD_AP, XbeeFrameCommandResponse::returnType::BYTE));
    setValueAO(commandWithResponse(XBEE_CMD_AO, XbeeFrameCommandResponse::returnType::BYTE));
    setValueID(commandWithResponse(XBEE_CMD_ID, XbeeFrameCommandResponse::returnType::LONG));
    setValueVR(commandWithResponse(XBEE_CMD_VR, XbeeFrameCommandResponse::returnType::SHORT));

    uint32_t sh = commandWithResponse(XBEE_CMD_SH, XbeeFrameCommandResponse::returnType::WORD);
    uint32_t sl = commandWithResponse(XBEE_CMD_SL, XbeeFrameCommandResponse::returnType::WORD);
    uint32_t my = commandWithResponse(XBEE_CMD_MY, XbeeFrameCommandResponse::returnType::SHORT);
    setAddress(sh, sl, my);

    updatePortConfig(XBEE_CMD_P0, xbee_port::P0);
    updatePortConfig(XBEE_CMD_P1, xbee_port::P1);
    updatePortConfig(XBEE_CMD_P2, xbee_port::P2);
    updatePortConfig(XBEE_CMD_D0, xbee_port::D0);
    updatePortConfig(XBEE_CMD_D1, xbee_port::D1);
    updatePortConfig(XBEE_CMD_D2, xbee_port::D2);
    updatePortConfig(XBEE_CMD_D3, xbee_port::D3);
    updatePortConfig(XBEE_CMD_D4, xbee_port::D4);
    updatePortConfig(XBEE_CMD_D5, xbee_port::D5);

/*   These ports are not available on XBee24-ZB

    updatePortConfig(XBEE_CMD_P3, xbee_port::P3);
    updatePortConfig(XBEE_CMD_P4, xbee_port::P4);
    updatePortConfig(XBEE_CMD_P5, xbee_port::P5);
    updatePortConfig(XBEE_CMD_P6, xbee_port::P6);
    updatePortConfig(XBEE_CMD_P7, xbee_port::P7);
    updatePortConfig(XBEE_CMD_P8, xbee_port::P8);
    updatePortConfig(XBEE_CMD_P9, xbee_port::P9);
    updatePortConfig(XBEE_CMD_D8, xbee_port::D8);*/
}

void XbeeLocal::updatePortConfig(string cmd, xbee_port p)
{
    try
    {
        uint64_t resp = commandWithResponse(cmd, XbeeFrameCommandResponse::returnType::BYTE);
        setPortFunction(p, static_cast<xbee_port_function>(resp));
    } catch (invalid_argument &iaex)
    {
        stringstream ss;

        cerr << "Cant get port configuration:" << getPortName(p) << endl;
    }
}

XbeeAddress& XbeeLocal::getAddress()
{
    if (!initialized)
        throw runtime_error("Not initialized");
    return Xbee::getAddress();
}

void XbeeLocal::print(bool debug)
{
    Xbee::print(debug);
    if (discovered.size() > 0)
    {
        cout << "discovered devices:" << endl;
        for (auto it=discovered.begin();it!=discovered.end(); it++)
            (*it)->print(debug);
    }
}

void XbeeLocal::configurePortFunction(xbee_port port, xbee_port_function fnc)
{
    sendCommandWithParameter(getCommandForPort(port), static_cast<uint8_t>(fnc));
    setPortFunction(port, fnc);
}
