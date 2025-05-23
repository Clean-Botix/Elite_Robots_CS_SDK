#include "TcpServer.hpp"
#include <iostream>
#include "EliteException.hpp"
#include "Log.hpp"

namespace ELITE {

TcpServer::TcpServer(int port, int recv_buf_size) : read_buffer_(recv_buf_size) {
    if (!s_io_context_ptr_) {
        throw EliteException(EliteException::Code::TCP_SERVER_CONTEXT_NULL);
    }
    io_context_ = s_io_context_ptr_;

    acceptor_ = std::make_unique<boost::asio::ip::tcp::acceptor>(
        *io_context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port), true);
    acceptor_->listen(1);
}

TcpServer::~TcpServer() {
    std::lock_guard<std::mutex> lock(socket_mutex_);
    if (acceptor_ && acceptor_->is_open()) {
        boost::system::error_code ec;
        acceptor_->cancel(ec);
        acceptor_->close(ec);
        acceptor_.reset();
    }
    if (socket_) {
        boost::system::error_code ec;
        closeSocket(socket_, ec);
        socket_.reset();
    }
}

void TcpServer::setReceiveCallback(ReceiveCallback cb) { receive_cb_ = std::move(cb); }

void TcpServer::startListen() { doAccept(); }

void TcpServer::doAccept() {
    std::lock_guard<std::mutex> lock(socket_mutex_);
    if (!acceptor_) {
        return;
    }
    auto new_socket = std::make_shared<boost::asio::ip::tcp::socket>(*io_context_);
    std::weak_ptr<TcpServer> weak_self = shared_from_this();
    // Accept call back
    auto accept_cb = [weak_self, new_socket](boost::system::error_code ec) {
        boost::system::error_code ignore_ec;
        if (auto self = weak_self.lock()) {
            if (!ec) {
                std::lock_guard<std::mutex> lock(self->socket_mutex_);
                // Close old connection
                if (self->socket_ && self->socket_->is_open()) {
                    auto local_point = self->socket_->local_endpoint(ignore_ec);
                    auto remote_point = self->socket_->remote_endpoint(ignore_ec);
                    self->closeSocket(self->socket_, ignore_ec);
                    ELITE_LOG_INFO("TCP port %d has new connection and close old client: %s:%d %s", local_point.port(),
                                   remote_point.address().to_string().c_str(), remote_point.port(),
                                   boost::system::system_error(ignore_ec).what());
                }
                // Socket set option
                new_socket->set_option(boost::asio::socket_base::reuse_address(true), ignore_ec);
                new_socket->set_option(boost::asio::ip::tcp::no_delay(true), ignore_ec);
                new_socket->set_option(boost::asio::socket_base::keep_alive(true), ignore_ec);
                // Update alive socket
                self->socket_ = new_socket;
                auto local_point = self->socket_->local_endpoint(ignore_ec);
                auto remote_point = self->socket_->remote_endpoint(ignore_ec);
                ELITE_LOG_INFO("TCP port %d accept client: %s:%d %s", local_point.port(),
                               remote_point.address().to_string().c_str(), remote_point.port(),
                               boost::system::system_error(ec).what());
                // Start async read
                self->doRead(new_socket);
            } else {
                std::lock_guard<std::mutex> lock(self->socket_mutex_);
                // Close old connection
                if (self->socket_ && self->socket_->is_open()) {
                    auto local_point = self->socket_->local_endpoint(ignore_ec);
                    auto remote_point = self->socket_->remote_endpoint(ignore_ec);
                    self->closeSocket(self->socket_, ignore_ec);
                    ELITE_LOG_ERROR("TCP port %d accept new connection fail(%s), and close old connection %s:%d %s",
                                    local_point.port(), boost::system::system_error(ec).what(),
                                    remote_point.address().to_string().c_str(), remote_point.port(),
                                    boost::system::system_error(ignore_ec).what());
                }
                self->socket_.reset();
            }
            self->doAccept();
        }
    };

    acceptor_->async_accept(*new_socket, accept_cb);
}

void TcpServer::doRead(std::shared_ptr<boost::asio::ip::tcp::socket> sock) {
    std::weak_ptr<TcpServer> weak_self = shared_from_this();
    auto read_cb = [weak_self, sock](boost::system::error_code ec, std::size_t n) {
        if (auto self = weak_self.lock()) {
            if (!ec) {
                if (self->receive_cb_) {
                    self->receive_cb_(self->read_buffer_.data(), n);
                }
                // Continue read
                self->doRead(sock);
            } else {
                if (sock->is_open()) {
                    boost::system::error_code ignore_ec;
                    auto local_point = sock->local_endpoint(ignore_ec);
                    auto remote_point = sock->remote_endpoint(ignore_ec);
                    self->closeSocket(sock, ignore_ec);
                    ELITE_LOG_INFO("TCP port %d close client: %s:%d %s. Reason: %s", local_point.port(),
                                   remote_point.address().to_string().c_str(), remote_point.port(),
                                   boost::system::system_error(ignore_ec).what(), boost::system::system_error(ec).what());
                }
            }
        }
    };
    boost::asio::async_read(*sock, boost::asio::buffer(read_buffer_), read_cb);
}

void TcpServer::start() {
    if (s_server_thread_) {
        return;
    }
    if (!s_io_context_ptr_) {
        s_io_context_ptr_ = std::make_shared<boost::asio::io_context>();
    }
    s_work_guard_ptr_.reset(new boost::asio::executor_work_guard<boost::asio::io_context::executor_type>(
        boost::asio::make_work_guard(*s_io_context_ptr_)));
    s_server_thread_.reset(new std::thread([]() {
        try {
            if (s_io_context_ptr_->stopped()) {
                s_io_context_ptr_->restart();
            }
            s_io_context_ptr_->run();
            ELITE_LOG_INFO("TCP server exit thread");
        } catch (const boost::system::system_error& e) {
            ELITE_LOG_FATAL("TCP server thread error: %s", e.what());
        }
    }));
}

void TcpServer::stop() {
    s_work_guard_ptr_->reset();
    s_io_context_ptr_->stop();
    if (s_server_thread_ && s_server_thread_->joinable()) {
        s_server_thread_->join();
    }
    s_work_guard_ptr_.reset();
    s_server_thread_.reset();
    s_io_context_ptr_.reset();
}

int TcpServer::writeClient(void* data, int size) {
    std::lock_guard<std::mutex> lock(socket_mutex_);
    if (socket_) {
        return boost::asio::write(*socket_, boost::asio::buffer(data, size));
    }
    return -1;
}

bool TcpServer::isClientConnected() {
    std::lock_guard<std::mutex> lock(socket_mutex_);
    if (socket_) {
        return socket_->is_open();
    }
    return false;
}

void TcpServer::closeSocket(std::shared_ptr<boost::asio::ip::tcp::socket> sock, boost::system::error_code& ec) {
    if (sock->is_open()) {
        sock->cancel(ec);
        sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        sock->close(ec);
    }
}

std::unique_ptr<std::thread> TcpServer::s_server_thread_;
std::shared_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> TcpServer::s_work_guard_ptr_;
std::shared_ptr<boost::asio::io_context> TcpServer::s_io_context_ptr_;

}  // namespace ELITE
