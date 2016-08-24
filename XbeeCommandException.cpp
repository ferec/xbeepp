#include "XbeeCommandException.h"

using namespace std;

XbeeCommandException::XbeeCommandException(XbeeCommandResponse::status st):XbeeException(XbeeCommandResponse::getStatusName(st))
{
    //ctor
}

XbeeCommandException::~XbeeCommandException() noexcept(true)
{
    //dtor
}
