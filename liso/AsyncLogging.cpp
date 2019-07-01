//
// Created by Merlin on 2019-05-19.
//

#include <chrono>
#include "AsyncLogging.h"

struct TimeCache {
    std::time_t last_second;
    std::time_t last_minute;
    char str[30];
};

thread_local struct TimeCache time_cache = {0};

namespace Liso {
    AsyncLogging::AsyncLogging(Liso::LogHandler* log_handler_) :
    log_handler(log_handler_),
    worker_thread(),
    running(false),
    buf(new LargeBuf()),
    buf_next(new LargeBuf())
    {}

    AsyncLogging::~AsyncLogging() {
        stop();
    }

    void AsyncLogging::ERROR(const char *msg) {
        char tmp[1000] = {0};
        strcat(tmp, time_str());
        strcat(tmp + strlen(tmp), "\tERROR\t");
        strcat(tmp + strlen(tmp), msg);
        strcat(tmp + strlen(tmp), "\n");
        log_append(tmp, strlen(tmp));
    }

    void AsyncLogging::INFO(const char *msg) {
        char tmp[1000] = {0};
        strcat(tmp, time_str());
        strcat(tmp + strlen(tmp), "\tINFO\t");
        strcat(tmp + strlen(tmp), msg);
        strcat(tmp + strlen(tmp), "\n");
        log_append(tmp, strlen(tmp));
    }

    void AsyncLogging::start() {
        if (!worker_thread.joinable()) {
            running = true;
            thread t(&AsyncLogging::thread_func, this);
            worker_thread = std::move(t);
        }
    }

    void AsyncLogging::stop() {
        if (worker_thread.joinable()) {
            running = false;
            worker_thread.join();
        }
    }

    void AsyncLogging::log_append(const char* msg, int len) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (buf->avaible(len)) {
            buf->put(msg, len);
        }
        else if (buf_next->avaible(len)) {
            buf_next->put(msg, len);
            buf_array.push_back(std::move(buf));
            buf = std::move(buf_next);
            buf_next = unique_ptr<LargeBuf>(new LargeBuf());
            cond_.notify_one();
        }
    }

    void AsyncLogging::thread_func() {

        while (running) {
            std::vector<unique_ptr<LargeBuf>> buf_to_write;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cond_.wait_for(lock, std::chrono::seconds(2));
                if (!buf->empty()) {
                    buf_array.push_back(std::move(buf));
                    buf = unique_ptr<LargeBuf>(new LargeBuf());
                }
                buf_to_write.swap(buf_array);
            }

            for (auto i = buf_to_write.begin(); i != buf_to_write.end(); i++) {
                log_handler->append((*i)->data(), (*i)->length());
            }
        }
    }

    const char* AsyncLogging::time_str() {
        static const char* tb[60] = {
            ":01", ":02", ":03", ":04", ":05", ":06", ":07", ":08", ":09", ":10",
            ":11", ":12", ":13", ":14", ":15", ":16", ":17", ":18", ":19", ":20",
            ":21", ":22", ":23", ":24", ":25", ":26", ":27", ":28", ":29", ":30",
            ":31", ":32", ":33", ":34", ":35", ":36", ":37", ":38", ":39", ":40",
            ":41", ":42", ":43", ":44", ":45", ":46", ":47", ":48", ":49", ":50",
            ":51", ":52", ":53", ":54", ":55", ":56", ":57", ":58", ":59", ":00"
        };
        std::time_t curr_time = time(nullptr);
        std::time_t curr_minute = curr_time / 60;
        if (curr_time != time_cache.last_second && curr_minute == time_cache.last_minute) {
            time_cache.last_second = curr_time;
            strcpy(time_cache.str + 16, tb[curr_time % 60]);
        }
        else if (curr_minute != time_cache.last_minute) {
            time_cache.last_second = curr_time;
            time_cache.last_minute = curr_minute;
            struct tm *local_time = localtime(&curr_time);
            strftime(time_cache.str, sizeof(time_cache.str), "%Y-%m-%d %H:%M:%S", local_time);
        }
        return time_cache.str;
    }
}
