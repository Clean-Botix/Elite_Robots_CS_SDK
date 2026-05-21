#ifndef __ELITE_REVERSE_PORT_HPP__
#define __ELITE_REVERSE_PORT_HPP__

#include "TcpServer.hpp"
#include <memory>
#include <string>

namespace ELITE
{

class ReversePort {
protected:
    std::shared_ptr<TcpServer> server_;

    int write(void* data, int size) {
        return server_->writeClient(data, size);
    }

public:
    ReversePort(int port, int buffer_size = 4, std::string label = "") {
        server_ = std::make_shared<TcpServer>(port, buffer_size, std::move(label));
    }
    ~ReversePort() = default;

    bool isRobotConnect() {
        return server_->isClientConnected();
    }
};


} // namespace ELITE








#endif
