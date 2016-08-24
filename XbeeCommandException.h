#ifndef XBEECOMMANDEXCEPTION_H
#define XBEECOMMANDEXCEPTION_H

#include "XbeeCommandResponse.h"
#include "XbeeException.h"

class XbeeCommandException : public XbeeException
{
    public:
        XbeeCommandException(XbeeCommandResponse::status s);
        virtual ~XbeeCommandException() noexcept(true);
    protected:
    private:
};

#endif // XBEECOMMANDEXCEPTION_H
