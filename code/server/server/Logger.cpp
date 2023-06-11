#include "Server/Logger.hpp"

namespace NRenderer
{
    Logger::Logger()
        : msgs      ()
        , mtx     ()
    {
        msgs.reserve(100);
    }
    void Logger::log(const string &msg, LogType type)
    {
        mtx.lock();
        auto t = chrono::system_clock::now();
        auto time = chrono::system_clock::to_time_t(t);
        string timeStr{ctime(&time)};
        timeStr.pop_back();
        string prefix = "[ " + timeStr + " ] ";
        msgs.push_back({type, (prefix + msg)});
        mtx.unlock();
    }
    void Logger::log(const string &msg)
    {
        this->log(msg, LogType::NORMAL);
    }
    void Logger::warning(const string &msg)
    {
        this->log(msg, LogType::WARNING);
    }
    void Logger::error(const string &msg)
    {
        this->log(msg, LogType::ERROR);
    }
    void Logger::success(const string &msg)
    {
        this->log(msg, LogType::SUCCESS);
    }
    void Logger::clear()
    {
        mtx.lock();
        msgs.clear();
        mtx.unlock();
    }
    Logger::LogMessages Logger::get() {
        int maxNums = 50;
        mtx.lock();
        LogMessages logMsg;
        if (msgs.size() < 50) {
            logMsg.nums = msgs.size();
            if (logMsg.nums != 0) {
                logMsg.msgs = &msgs[0];
            }
            else {
                logMsg.msgs = nullptr;
            }
        }
        else {
            logMsg.nums = 50;
            logMsg.msgs = &msgs[msgs.size() - 50];
        }
        mtx.unlock();
        return logMsg;
    }
}

NRenderer::Logger& getLogger() {
    static NRenderer::Logger logger{};
    return logger;
}