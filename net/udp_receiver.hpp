#pragma once
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstdint>
#include <functional>
#include <stdexcept>

namespace fasttrade {

struct MarketTick {
    uint64_t timestamp_ns;
    uint32_t instrument_id;
    double   price;
    uint32_t quantity;
};

class UDPReceiver {
public:
    using TickHandler = std::function<void(const MarketTick&)>;

    UDPReceiver(uint16_t port, TickHandler handler) : handler_(handler) {
        sock_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock_fd_ < 0) throw std::runtime_error("socket() failed");
        int flags = fcntl(sock_fd_, F_GETFL, 0);
        fcntl(sock_fd_, F_SETFL, flags | O_NONBLOCK);
        sockaddr_in addr{};
        addr.sin_family      = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port        = htons(port);
        if (bind(sock_fd_, (sockaddr*)&addr, sizeof(addr)) < 0)
            throw std::runtime_error("bind() failed");
        epoll_fd_ = epoll_create1(0);
        if (epoll_fd_ < 0) throw std::runtime_error("epoll_create1() failed");
        epoll_event ev{};
        ev.events  = EPOLLIN;
        ev.data.fd = sock_fd_;
        epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, sock_fd_, &ev);
    }

    ~UDPReceiver() { close(sock_fd_); close(epoll_fd_); }

    void poll(int timeout_ms = 100) {
        epoll_event events[16];
        int n = epoll_wait(epoll_fd_, events, 16, timeout_ms);
        for (int i = 0; i < n; ++i)
            if (events[i].data.fd == sock_fd_) receive_tick();
    }

    void stop() { running_ = false; }

private:
    void receive_tick() {
        MarketTick tick{};
        sockaddr_in sender{};
        socklen_t sender_len = sizeof(sender);
        ssize_t bytes = recvfrom(sock_fd_, &tick, sizeof(tick), 0,
                                 (sockaddr*)&sender, &sender_len);
        if (bytes == (ssize_t)sizeof(tick)) handler_(tick);
    }

    int         sock_fd_  = -1;
    int         epoll_fd_ = -1;
    bool        running_  = false;
    TickHandler handler_;
};

} // namespace fasttrade
