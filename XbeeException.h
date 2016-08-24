#ifndef XBEEEXCEPTION_H
#define XBEEEXCEPTION_H

#include <string>
#include <stdexcept>

class XbeeException : public std::runtime_error
{
    public:
        XbeeException(std::string what);
        virtual ~XbeeException() noexcept(true);
    protected:
    private:
};

#endif // XBEEEXCEPTION_H
