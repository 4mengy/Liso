//
// Created by merlin on 19-4-14.
//

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include "common.h"

using std::string;

void log_err(const string& msg, const bool errno_info) {
    string tmp;
    if (errno_info) {
        tmp = msg + ". error no: " + std::to_string(errno) + ", " + string(strerror(errno));
    } else {
        tmp = msg;
    }
    perror(tmp.c_str());
}

void log_info(const string& msg) {
    std::cout << msg << std::endl;
}

ssize_t read_sync(const int fd, std::vector<char>& out) {
    ssize_t read_len = 0;
    ssize_t read_sum = 0;
    char buf[MAX_READ_LEN] = {0};
    while (read_len = read(fd, buf, MAX_READ_LEN)) {
        if (read_len < 0) {
            log_err("read error", true);
            return -1;
        }
        read_sum += read_len;
        out.insert(out.end(), buf, buf + read_len);
    }
    return read_sum;
}


process_ret read_async(const int fd, std::vector<char>& out) {
    ssize_t read_len = 0;
    char buf[MAX_READ_LEN] = {0};
    while (true) {
        read_len = read(fd, buf, MAX_READ_LEN);
        if (read_len == 0) {
            return COMPLETE;
        }
        else if (read_len < 0 && errno == EAGAIN) {
            return ERROR_EAGAIN;
        }
        else if (read_len < 0) {
            log_err("read error", true);
            return ERROR_READ;
        }
        out.insert(out.end(), buf, buf + read_len);
    }
}


int write_sync(const int fd, const std::vector<char>& out) {
    const char *out_data = out.data();
    ssize_t i = 0;
    while (i < out.size()) {
        ssize_t write_len = write(fd, out_data + i, out.size() - i);
        if (write_len < 0) {
            log_err("write error", true);
            return -1;
        }
        i += write_len;
    }
    return 0;
}


process_ret write_async(const int fd, std::vector<char>& out) {
    while (true) {
        if (out.empty()) {
            return COMPLETE;
        }
        const char *out_data = out.data();
        ssize_t write_len = write(fd, out_data, out.size());
        if (write_len < 0 && errno == EAGAIN) {
            return ERROR_EAGAIN;
        }
        else if (write_len < 0) {
            log_err("write error", true);
            return ERROR_WRITE;
        }
        out.erase(out.begin(), out.begin() + write_len);
    }
}
