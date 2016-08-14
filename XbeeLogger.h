#ifndef XBEELOGGER_H
#define XBEELOGGER_H

#include <string>
//#include <iostream>

//typedef void (*t_logFnc)(std::string msg, std::string origin);

static const std::string SeverityLabel[] = {"Error", "Warning", "Info", "Debug"};

class XbeeLogger
{
    public:
        enum class Severity:uint8_t { Error, Warning, Info, Debug };


        static XbeeLogger& GetInstance();
        static void setLogger(XbeeLogger &log);

        virtual void doLog(const std::string &msg, Severity severity, const std::string &origin);

/*        template <typename T>
        XbeeLogger& operator << (const T &data) const
        {
            std::cout << data;
            return *this;
        }*/

//        void setLogger(t_logFnc l) { logger = l; }
        void setSeverity(Severity sev) { sevLimit = sev; }

    protected:
        XbeeLogger();

    private:
        XbeeLogger(XbeeLogger const&);              // Don't Implement
        void operator=(XbeeLogger const&); // Don't implement

//        t_logFnc logger;
        Severity sevLimit;

        static XbeeLogger *logger;
        static bool initialized;
};

#endif // XBEELOGGER_H
