//
//  mcbLogger.h
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/18/14.
//
//

#ifndef __SoundSurfer__mcbLogger__
#define __SoundSurfer__mcbLogger__

#include <iostream>
#include <vector>
#include <math.h>

namespace mcb{namespace PlatformSupport{
    
    //log level conventions:
    //0 - 3 - dev;
    //4 - iNF - prod
    //777 - Analytics
    enum {LogLevelDeveloper=0, LogLevelProduction=4, LogLevelAnalytics=777};
    
    class Logger{
    protected:
        //for example:
        //_logPrefix="\n\n-----<MyClassNAme>------\n";
        //_logSuffix("\n------------------------\n\n");
        std::string _logPrefix, _logSuffix;
        
        //override this to add implement your own logger for a particular class
        virtual void mcbLogFormatted(const std::string & message, unsigned int level=LogLevelDeveloper, const std::string & category="")const;
        
        //not recommended to override
        virtual void mcbLog(const std::string & format, ...) const;
        virtual void mcbLog(unsigned int level, const std::string & category, const std::string & format, ...) const;
    };
    
    namespace Log{
        struct LogEntry{
            LogEntry(const std::string & message, const std::string & category, const unsigned int level);
            const time_t timestamp;
            const std::string message;
            const std::string category;
            const unsigned int level;
            std::string stringValue() const;
        };
        typedef std::vector<const LogEntry> log_entries_t;
        typedef std::function<void(const LogEntry & logEntry)> log_analytics_handler_t;

        void setLogLevel(unsigned int level);//to disable log, just set the level too high
        unsigned int logLevel();

        /*
            @log dump
         */
        void setRecordingLog(bool recordLog);//default is false
        bool isLogRecorded();
        
        const log_entries_t & logEntries();
        std::string logDump(bool reversed=true, size_t maxLength=-1, unsigned int minLogLevel=0,  unsigned int maxLogLevel=HUGE_VAL);
        static std::string logDumpForLogLevel(unsigned int logLevel, bool reversed=true, size_t maxLength=-1){return logDump(reversed, maxLength, logLevel, logLevel);}
        static std::string logDumpForAnalytics(unsigned int logLevel=LogLevelAnalytics, bool reversed=true, size_t maxLength=-1){return logDump(reversed, maxLength, logLevel, logLevel);}
        void eraseLogEntries();
        
        /*
         @Analytics
         */
        //will be called every time the log is made, it is up to implementer to decide which log level is necessary for analytics
        void setAnalyticsHandler(const log_analytics_handler_t &handler);
        
        void log(const std::string & message, unsigned int level=LogLevelDeveloper, const std::string & category="");

        void mcbLog(const std::string & format, ...);
        void mcbLog(unsigned int level, const std::string & category, const std::string & format, ...);

    }
}}
#endif /* defined(__SoundSurfer__mcbLogger__) */
