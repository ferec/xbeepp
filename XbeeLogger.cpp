#include "XbeeLogger.h"
#include "../andamCommon/include/timeUtils.h"

#include <iostream>
#include <chrono>
#include <iomanip>

using namespace std;

XbeeLogger::XbeeLogger():sevLimit(XbeeLogger::Severity::Warning)
{
}

XbeeLogger *XbeeLogger::logger = NULL; //Logger::GetDefaultInstance(); // = nullptr;

XbeeLogger& XbeeLogger::GetInstance()
{
    static XbeeLogger instance;
    if (logger == nullptr)
        return instance;
    return *logger;
}

void XbeeLogger::doLog(const string &msg, Severity severity, const string &origin)
{
    uint8_t sev = static_cast<uint8_t>(severity);
    if (severity <= sevLimit && sev >= 0 && sev <= 3)
    {
        cout << getTimestampString() << ": " << SeverityLabel[sev] << " (" << origin << ")" << ":" << msg << endl;
    }
}

/*Logger& Logger::operator<< (const string &msg)
{
    logger(msg, "");
    return *this;
}*/

void XbeeLogger::setLogger(XbeeLogger &log)
{
//    cout << "setting logger " << &log << endl;
    logger = &log;
}
