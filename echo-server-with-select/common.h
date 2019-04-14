//
// Created by merlin on 19-3-22.
//

#ifndef ECHO_SERVER_WITH_SELECT_COMMON_H
#define ECHO_SERVER_WITH_SELECT_COMMON_H

#include <vector>
#include <string>

const int SERVER_PORT = 8000;
const int SOCKET_DEFAULT_PROTOCOL = 0;

const int MAX_READ_LEN = 1024;

typedef enum {
    COMPLETE = 0,
    ERROR_READ = -1,
    ERROR_WRITE = -2,
    ERROR_EAGAIN = -3
} process_ret;


void log_err(const std::string& msg, const bool errno_info = false);

void log_info(const std::string& msg);

ssize_t read_sync(const int fd, std::vector<char>& out);


process_ret read_async(const int fd, std::vector<char>& out);


int write_sync(const int fd, const std::vector<char>& out);


process_ret write_async(const int fd, std::vector<char>& out);

#endif //ECHO_SERVER_WITH_SELECT_COMMON_H
