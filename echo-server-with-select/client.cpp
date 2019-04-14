#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "common.h"

typedef enum {
    SUCCESS = 0,
    ERROR_CREATE_SOCKET = -1,
    ERROR_CONNECT_SOCKET = -2,
    ERROR_CONNECT_SERVER = -3
} ret_code;


int connect_server(int& server_sock) {
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if ((server_sock = socket(AF_INET, SOCK_STREAM, SOCKET_DEFAULT_PROTOCOL)) == 0) {
        log_err("create socket error", true);
        return ERROR_CREATE_SOCKET;
    }
    if (connect(server_sock, reinterpret_cast<const struct sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        log_err("connect socket error", true);
        return ERROR_CONNECT_SOCKET;
    }
    return SUCCESS;
}


int main() {
    int server_sock = 0;
    if (connect_server(server_sock) < 0) {
        log_err("connect server error");
        return ERROR_CONNECT_SERVER;
    }

    std::vector<char> buf;
    std::string buf_str;
    getline(std::cin, buf_str);
    buf.insert(buf.end(), buf_str.begin(), buf_str.end());

    if (write_sync(server_sock, buf) < 0) {
        log_err("write_sync error");
    }
    shutdown(server_sock, SHUT_WR);

    buf.clear();
    if (read_sync(server_sock, buf) < 0) {
        log_err("read_sync error");
    }
    buf.insert(buf.end(), 0);

    std::cout << buf.data() << std::endl;
    close(server_sock);
    return 0;
}