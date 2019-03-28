#include <iostream>
#include <list>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "common.h"


typedef enum {
    SUCCESS = 0,
    ERROR_CREATE_SOCKET = -1,
    ERROR_SET_SOCKET = -2,
    ERROR_BIND_SOCKET = -3,
    ERROR_LISTEN = -4
} ret_code;


int create_server(int& server_fd, const struct sockaddr_in& address) {
    if ((server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, SOCKET_DEFAULT_PROTOCOL)) == 0) {
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


typedef struct {
    int client_fd;
    bool read_complete;
    std::vector<char> out;
} client_socket;


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

    std::list<client_socket> client_socket_list;

    while (true) {
        int accept_fd = 0;
        int addr_len = sizeof(address);
        accept_fd = accept4(listen_fd, reinterpret_cast<struct sockaddr *>(&address),
                             reinterpret_cast<socklen_t *>(&addr_len), SOCK_NONBLOCK);

        if (accept_fd > 0) {
            client_socket client;
            client.client_fd = accept_fd;
            client.read_complete = false;
            client.out = std::vector<char>();
            client_socket_list.push_back(client);
        }
        else if (errno != EAGAIN && errno != EWOULDBLOCK) {
            log("accept error");
        }

        for (auto i = client_socket_list.begin(); i != client_socket_list.end(); i++) {
            if (!i->read_complete) {
                process_ret ret = read_async(i->client_fd, i->out);
                if (ret == COMPLETE) {
                    i->read_complete = true;
                } else if (ret == ERROR_EAGAIN) {
                    continue;
                }
            }
            process_ret ret = write_async(i->client_fd, i->out);
            if (ret == COMPLETE) {
                close(i->client_fd);
                i = client_socket_list.erase(i);
                i--;
            }
        }
    }
    close(listen_fd);
    return 0;
}
