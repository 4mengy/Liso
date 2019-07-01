//
// Created by Merlin on 2019-05-20.
//

#include <cassert>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "LogFile.h"

namespace Liso {

    LogFile::LogFile(const std::string& file_name) :
    fd(::open(file_name.c_str(), O_CREAT | O_APPEND | O_WRONLY, 0644)) {
        assert(fd > 0);
    }

    LogFile::~LogFile() {
        ::close(fd);
    }

    void LogFile::append(const char* log_line, int len) {
        // linux 3.14及以上版本多个进程、线程同时写一个普通文件时，可以保证原子
        ::write(fd, log_line, len);
    }
}
