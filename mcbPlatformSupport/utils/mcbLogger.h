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

namespace mcb{namespace PlatformSupport{namespace utils{
    class Logger{
    protected:
        //override this to add class name as category (eg. if category.empty(), use the class name, call super)
        /*
         virtual void mcbLogFormatted(const std::string & message, unsigned int level=0, const std::string & category="")const{
         if (category.empty())
         utils::Logger::mcbLogFormatted(message, level, __FILE__);
         else
         utils::Logger::mcbLogFormatted(message, level, category);
         }
         */
        
        virtual void mcbLogFormatted(const std::string & message, unsigned int level=0, const std::string & category="")const;
        
        //not recommended to override
        virtual void mcbLog(const std::string & format, ...) const;
        virtual void mcbLog(const std::string & category, const std::string & format, ...) const;
        virtual void mcbLog(const std::string & category, unsigned int level, const std::string & format, ...) const;
    };
    
    //log level conventions:
    //0 - 3 - dev;
    //4 - iNF - prod
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
        
        void setLogLevel(unsigned int level);//to disable log, just set the level too high
        unsigned int logLevel();
        
        void setRecordingLog(bool recordLog);//default is false
        bool isLogRecorded();
        
        const log_entries_t & logEntries();
        std::string logDump();
        void eraseLogEntries();
        
        void log(const std::string & message, unsigned int level=0, const std::string & category="");
    }
}}}
#endif /* defined(__SoundSurfer__mcbLogger__) */
