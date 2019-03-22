#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "common.h"


typedef enum {
    SUCCESS = 0,
    ERROR_CREATE_SOCKET = -1,
    ERROR_SET_SOCKET = -2,
    ERROR_BIND_SOCKET = -3,
    ERROR_LISTEN = -4,
    ERROR_ACCEPT = -5,
    ERROR_READ_ERROR = -6
} ret_code;


int create_server(int& server_fd, const struct sockaddr_in& address) {
    if ((server_fd = socket(AF_INET, SOCK_STREAM, SOCKET_DEFAULT_PROTOCOL)) == 0) {
        log("create socket failed");
        return ERROR_CREATE_SOCKET;
    }

    int opt_val = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt_val, sizeof(opt_val))) {
        log("set socket error");
        return ERROR_SET_SOCKET;
    }

    if (bind(server_fd, reinterpret_cast<const struct sockaddr*>(&address), sizeof(address)) < 0) {
        log("bind error");
        return ERROR_BIND_SOCKET;
    }
    return SUCCESS;
}


int main(int argc, const char *argv[]) {
    int listen_fd;

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SERVER_PORT);

    int ret = create_server(listen_fd, address);
    if (ret != SUCCESS) {
        return ret;
    }

    if (listen(listen_fd, 3) < SUCCESS) {
        log("listen error");
        return ERROR_LISTEN;
    }

    while (true) {
        int accept_fd = 0;
        int addr_len = sizeof(address);
        if ((accept_fd = accept(listen_fd, reinterpret_cast<struct sockaddr *>(&address),
                                reinterpret_cast<socklen_t *>(&addr_len))) < 0) {
            log("accept error");
            return ERROR_ACCEPT;
        }

        std::vector<char> out;
        if (read_sync(accept_fd, out) < 0) {
            log("read_sync error");
        }

        if (write_sync(accept_fd, out) < 0) {
            log("write_sync error");
        }
        close(accept_fd);
    }
    close(listen_fd);
    return 0;
}
