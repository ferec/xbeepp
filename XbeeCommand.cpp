#include "XbeeCommand.h"

#include "XbeeException.h"

using namespace std;

XbeeCommand::XbeeCommand():XbeeCommand(string()) //cmd(),port(XbeePort::xbee_port::NONE),portFunction(XbeePort::xbee_port_function::None)
{
}

XbeeCommand::XbeeCommand(const string cmd):cmd(cmd),port(XbeePort::pinID::NONE),portFunction(XbeePort::pinFunction::None)
{
}

XbeeCommand::XbeeCommand(const XbeePort::pinID port, const XbeePort::pinFunction pf):cmd(getCommandForPort(port)),port(port),portFunction(pf)
{

}

XbeeCommand::~XbeeCommand()
{
    //dtor
}

string XbeeCommand::getCommandForPort(XbeePort::pinID port)
{
    typedef XbeePort::pinID pt;

    switch(port)
    {
    case pt::P0:
        return CMD(P0);
    case pt::P1:
        return CMD(P1);
    case pt::P2:
        return CMD(P2);
    case pt::P3:
        return CMD(P3);
    case pt::P4:
        return CMD(P4);
    case pt::P5:
        return CMD(P5);
    case pt::P6:
        return CMD(P6);
    case pt::P7:
        return CMD(P7);
    case pt::P8:
        return CMD(P8);
    case pt::P9:
        return CMD(P9);
    case pt::D0:
        return CMD(D0);
    case pt::D1:
        return CMD(D1);
    case pt::D2:
        return CMD(D2);
    case pt::D3:
        return CMD(D3);
    case pt::D4:
        return CMD(D4);
    case pt::D5:
        return CMD(D5);
    case pt::D8:
        return CMD(D8);
    case pt::NONE:
        throw invalid_argument("No port available for this command");
    }
    throw invalid_argument("Xbee port does not exists");
}

XbeePort::pinID XbeeCommand::getPortForCommand(std::string cmd)
{
    if (cmd == CMD(P0))
        return XbeePort::pinID::P0;
    if (cmd == CMD(P1))
        return XbeePort::pinID::P1;
    if (cmd == CMD(P2))
        return XbeePort::pinID::P2;
    if (cmd == CMD(P3))
        return XbeePort::pinID::P3;
    if (cmd == CMD(P4))
        return XbeePort::pinID::P4;
    if (cmd == CMD(P5))
        return XbeePort::pinID::P5;
    if (cmd == CMD(P6))
        return XbeePort::pinID::P6;
    if (cmd == CMD(P7))
        return XbeePort::pinID::P7;
    if (cmd == CMD(P8))
        return XbeePort::pinID::P8;
    if (cmd == CMD(P9))
        return XbeePort::pinID::P9;

    if (cmd == CMD(D0))
        return XbeePort::pinID::D0;
    if (cmd == CMD(D1))
        return XbeePort::pinID::D1;
    if (cmd == CMD(D2))
        return XbeePort::pinID::D2;
    if (cmd == CMD(D3))
        return XbeePort::pinID::D3;
    if (cmd == CMD(D4))
        return XbeePort::pinID::D4;
    if (cmd == CMD(D5))
        return XbeePort::pinID::D5;
    if (cmd == CMD(D8))
        return XbeePort::pinID::D8;

    throw XbeeException("not a port command " + cmd);
}
