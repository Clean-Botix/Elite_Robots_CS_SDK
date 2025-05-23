#ifndef __SCRIPT_SENDER_HPP__
#define __SCRIPT_SENDER_HPP__

#include "TcpServer.hpp"

#include <boost/asio.hpp>
#include <string>
#include <memory>

namespace ELITE
{

class ScriptSender : protected TcpServer {
private:
    const std::string PROGRAM_REQUEST_ = std::string("request_program");
    const std::string& program_;
    boost::asio::streambuf recv_request_buffer_;

    void responseRequest(std::shared_ptr<boost::asio::ip::tcp::socket> sock);

    virtual void doAccept() override;

public:
    ScriptSender(int port, const std::string& program);
    ~ScriptSender();
};








} // namespace ELITE






#endif
