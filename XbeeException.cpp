#include "XbeeException.h"

using namespace std;

XbeeException::XbeeException(string what):runtime_error(what)
{
    //ctor
}

XbeeException::~XbeeException() noexcept(true)
{
    //dtor
}
