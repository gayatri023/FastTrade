#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include "net/udp_receiver.hpp"

namespace fasttrade {

class TickSender {
public:
    TickSender(const char* ip, uint16_t port) {
        sock_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock_fd_ < 0) throw std::runtime_error("socket() failed");
        addr_.sin_family = AF_INET;
        addr_.sin_port   = htons(port);
        inet_pton(AF_INET, ip, &addr_.sin_addr);
    }

    ~TickSender() { close(sock_fd_); }

    void send_tick(const MarketTick& tick) {
        sendto(sock_fd_, &tick, sizeof(tick), 0,
               (sockaddr*)&addr_, sizeof(addr_));
    }

private:
    int         sock_fd_ = -1;
    sockaddr_in addr_{};
};

} // namespace fasttrade
