//
//  mcbLogger.cpp
//  SoundSurfer
//
//  Created by Radif Sharafullin on 1/18/14.
//
//

#include "mcbLogger.h"
#include "mcbPlatformSupportFunctions.h"

namespace mcb{namespace PlatformSupport{namespace utils{
    static const int kMaxLogLen = 16*1024;

    void Logger::mcbLogFormatted(const std::string & message, unsigned int level, const std::string & category)const{
        Log::log(message, level, category);
    }
    void Logger::mcbLog(const std::string & category, const std::string & format, ...) const{
        char szBuf[kMaxLogLen+1] = {0};
        va_list ap;
        va_start(ap, format);
        vsnprintf(szBuf, kMaxLogLen, format.c_str(), ap);
        va_end(ap);
        mcbLogFormatted(szBuf, 0, category);
    }
    void Logger::mcbLog(const std::string & category, unsigned int level, const std::string & format, ...) const{
        char szBuf[kMaxLogLen+1] = {0};
        va_list ap;
        va_start(ap, format);
        vsnprintf(szBuf, kMaxLogLen, format.c_str(), ap);
        va_end(ap);
        mcbLogFormatted(szBuf, level, category);
    }
    void Logger::mcbLog(const std::string & format, ...) const{
        char szBuf[kMaxLogLen+1] = {0};
        va_list ap;
        va_start(ap, format);
        vsnprintf(szBuf, kMaxLogLen, format.c_str(), ap);
        va_end(ap);
        mcbLogFormatted(szBuf);
    }
    
    
    namespace Log{
        static unsigned int _logLevel(0);
        static bool _isRecordingLog(false);
        static log_entries_t _logEntries;
        
        
        LogEntry::LogEntry(const std::string & m_message, const std::string & m_category, const unsigned int m_level)
        :message(m_message)
        ,category(m_category)
        ,level(m_level)
        , timestamp(time(0))
        {}
        std::string LogEntry::stringValue() const{
            struct tm * timeinfo(localtime(&timestamp));
            char buffer[80];
            strftime(buffer,80,"%d-%m-%Y %I:%M:%S",timeinfo);
            if (category.empty())
                return "[" +std::string(buffer)+ "] level: "+Functions::t_to_string(level)+" "+message;
            return "[" +std::string(buffer)+ "] "+category+" level: "+Functions::t_to_string(level)+" "+message;
        }

        void setLogLevel(unsigned int level){
            _logLevel=level;
        }
        unsigned int logLevel(){
            return _logLevel;
        }

        void setRecordingLog(bool recordLog){
            _isRecordingLog=recordLog;
        }
        const log_entries_t & logEntries(){
            return _logEntries;
        }
        std::string logDump(){
            std::string retVal;
            for (const LogEntry & logEntry : _logEntries)
                retVal+=logEntry.stringValue();
            return retVal;
        }
        void eraseLogEntries(){
            log_entries_t newLogData;
            _logEntries=std::move(newLogData);
        }
        
        void log(const std::string & message, unsigned int level, const std::string & category){
            //we are moving it, it cannot be const
            LogEntry logEntry(message,category, level);
            if (_logLevel<=logEntry.level)
                std::cout << "mcbLog: "<< logEntry.stringValue() << std::endl;
            if (_isRecordingLog)
                _logEntries.emplace_back(std::move(logEntry));
        }
    }
}}}