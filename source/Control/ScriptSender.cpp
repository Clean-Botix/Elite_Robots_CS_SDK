#include "ScriptSender.hpp"
#include "ControlCommon.hpp"
#include "EliteException.hpp"
#include "Log.hpp"
#include <boost/asio.hpp>

using namespace ELITE;



ScriptSender::ScriptSender(int port, const std::string& program) : program_(program), TcpServer(port, 0) {
    doAccept();
}


ScriptSender::~ScriptSender() {
    
}

void ScriptSender::doAccept() {
    // Accept call back
    auto accept_cb = [this](boost::system::error_code ec, boost::asio::ip::tcp::socket sock) {
        auto new_socket = std::make_shared<boost::asio::ip::tcp::socket>(std::move(sock));
        responseRequest(new_socket);
        ScriptSender::doAccept();
    };
    acceptor_->listen(1);
    acceptor_->async_accept(*io_context_, accept_cb);
}

void ScriptSender::responseRequest(std::shared_ptr<boost::asio::ip::tcp::socket> sock) {
    boost::asio::async_read_until(
        *sock,
        recv_request_buffer_,
        '\n',
        [&, sock](boost::system::error_code ec, std::size_t len) {
            if (ec) {
                if (sock->is_open()) {
                    ELITE_LOG_INFO("Connection to script sender interface dropped: %s", boost::system::system_error(ec).what());
                }
            } else {
                ELITE_LOG_INFO("Robot request external control script.");
                std::string request;
                std::istream response_stream(&recv_request_buffer_);
                std::getline(response_stream, request);
                if (request == PROGRAM_REQUEST_) {
                    boost::system::error_code wec;
                    sock->write_some(boost::asio::buffer(program_), wec);
                    if (wec) {
                        ELITE_LOG_ERROR("Script sender send script fail: %s", boost::system::system_error(wec).what());
                        return;
                    }
                }
                responseRequest(sock);
            }
            
        }
    );
}
