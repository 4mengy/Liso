//
// Created by Merlin on 2019-05-19.
//

#ifndef LISO_ASYNCLOGGING_H
#define LISO_ASYNCLOGGING_H

#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <condition_variable>
#include "LogHandler.h"
#include "FixedBuf.h"


#define BUF_SIZE    1024 * 1024 * 100

namespace Liso::AsyncLoggingNS {
    using std::string;
    using std::thread;
    using Liso::FixedBuf;
    using std::unique_ptr;
    using Liso::NoneCopyable;

    class AsyncLogging : NoneCopyable {
    public:
        typedef FixedBuf<BUF_SIZE> LargeBuf;

        AsyncLogging(LogHandler* log_handler_);
        ~AsyncLogging();

        void ERROR(const char *msg);
        void INFO(const char *msg);
        void start();

    private:
        void log_append(const char* msg, int len);
        void stop();
        static const char* time_str();
        void thread_func();

        std::atomic<bool> running;
        std::mutex mutex_;
        std::condition_variable cond_;
        LogHandler* log_handler;
        thread worker_thread;
        unique_ptr<LargeBuf> buf;
        unique_ptr<LargeBuf> buf_next;
        std::vector<unique_ptr<LargeBuf>> buf_array;
    };
}

namespace Liso {
    using AsyncLoggingNS::AsyncLogging;
}


#endif //LISO_ASYNCLOGGING_H
