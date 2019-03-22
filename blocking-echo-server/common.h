//
// Created by merlin on 19-3-22.
//
#include <iostream>
#include <vector>

#ifndef TEST_COMMON_H
#define TEST_COMMON_H

const int SERVER_PORT = 8000;
const int SOCKET_DEFAULT_PROTOCOL = 0;

const int MAX_READ_LEN = 1024;

void log(const char* msg) {
    std::cout << msg << std::endl;
}

ssize_t read_sync(const int fd, std::vector<char>& out) {
    ssize_t read_len = 0;
    ssize_t read_sum = 0;
    char buf[MAX_READ_LEN] = {0};
    while (read_len = read(fd, buf, MAX_READ_LEN)) {
        if (read_len < 0) {
            log("read error");
            return -1;
        }
        read_sum += read_len;
        out.insert(out.end(), buf, buf + read_len);
    }
    return read_sum;
}

int write_sync(const int fd, const std::vector<char>& out) {
    const char *out_data = out.data();
    ssize_t i = 0;
    while (i < out.size()) {
        ssize_t write_len = write(fd, out_data + i, out.size() - i);
        if (write_len < 0) {
            log("write error");
            return -1;
        }
        i += write_len;
    }
    return 0;
}

#endif //TEST_COMMON_H
