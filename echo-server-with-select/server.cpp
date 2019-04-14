#include <iostream>
#include <list>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <sys/select.h>

#include "common.h"


typedef enum {
    SUCCESS = 0,
    ERROR_CREATE_SOCKET = -1,
    ERROR_SET_SOCKET = -2,
    ERROR_BIND_SOCKET = -3,
    ERROR_LISTEN = -4
} ret_code;


int create_server(int& server_fd, const struct sockaddr_in& address) {
    if ((server_fd = socket(AF_INET, SOCK_STREAM, SOCKET_DEFAULT_PROTOCOL)) == 0) {
        log_err("create socket failed", true);
        return ERROR_CREATE_SOCKET;
    }

    int opt_val = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt_val, sizeof(opt_val))) {
        log_err("set socket error", true);
        return ERROR_SET_SOCKET;
    }

    if (bind(server_fd, reinterpret_cast<const struct sockaddr*>(&address), sizeof(address)) < 0) {
        log_err("bind error", true);
        return ERROR_BIND_SOCKET;
    }
    return SUCCESS;
}


typedef struct {
    int fd;
    bool read_complete;
    bool is_listen_fd;
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
        log_err("listen error", true);
        return ERROR_LISTEN;
    }

    std::list<client_socket> client_socket_list;
    client_socket client;
    client.fd = listen_fd;
    client.read_complete = false;
    client.is_listen_fd = true;
    client.out = std::vector<char>();
    client_socket_list.push_back(client);

    while (true) {
        fd_set read_fds, write_fds;
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        int max_fd = 0;
        for (auto i = client_socket_list.begin(); i != client_socket_list.end(); i++) {
            if (i->is_listen_fd || !i->read_complete) {
                FD_SET(i->fd, &read_fds);
            }
            else {
                FD_SET(i->fd, &write_fds);
            }
            max_fd = std::max(max_fd, i->fd);
        }

        select(max_fd + 1, &read_fds, &write_fds, NULL, NULL);

        for (auto i = client_socket_list.begin(); i != client_socket_list.end(); i++) {
            if (FD_ISSET(i->fd, &read_fds) && !i->read_complete) {
                if (i->is_listen_fd) {
                    int addr_len = sizeof(address);
                    int accept_fd = accept(listen_fd, reinterpret_cast<struct sockaddr *>(&address),
                                       reinterpret_cast<socklen_t *>(&addr_len));
                    if (accept_fd > 0) {
                        client_socket client;
                        client.fd = accept_fd;
                        client.read_complete = false;
                        client.is_listen_fd = false;
                        client.out = std::vector<char>();
                        client_socket_list.insert(i, client);
                        continue;
                    }
                    else {
                        log_err("accept error", true);
                    }
                }
                else {
                    char buf[MAX_READ_LEN] = {0};
                    int read_len = read(i->fd, buf, MAX_READ_LEN);
                    if (read_len < 0) {
                        log_err("read error", true);
                    }
                    else if (read_len == 0) {
                        i->read_complete = true;
                    }
                    else {
                        i->out.insert(i->out.end(), buf, buf + read_len);
                    }
                }
            }
            else if (FD_ISSET(i->fd, &write_fds)) {
                const char *out_data = i->out.data();
                ssize_t write_len = write(i->fd, out_data, i->out.size());
                if (write_len < 0) {
                    log_err("write error", true);
                }
                else if (write_len < i->out.size()) {
                    i->out = std::vector<char>(i->out.begin() + write_len, i->out.end());
                }
                else {
                    close(i->fd);
                    i = client_socket_list.erase(i);
                    i--;
                }
            }
        }
    }
    close(listen_fd);
    return 0;
}
