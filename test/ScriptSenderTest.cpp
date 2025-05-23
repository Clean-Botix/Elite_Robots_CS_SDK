#include "Control/ScriptSender.hpp"
#include "EliteException.hpp"
#include <gtest/gtest.h>
#include <thread>

using namespace ELITE;

#define PROGRAM_REQUEST ("request_program\n")
#define TEST_PORT 60002

class TcpClient {
public:
    boost::asio::io_context io_context;
    std::unique_ptr<boost::asio::ip::tcp::socket> socket_ptr;
    std::unique_ptr<boost::asio::ip::tcp::resolver> resolver_ptr;
    TcpClient(const std::string& ip, int port) {
        try {
            socket_ptr.reset(new boost::asio::ip::tcp::socket(io_context));
            resolver_ptr.reset(new boost::asio::ip::tcp::resolver(io_context));
            socket_ptr->open(boost::asio::ip::tcp::v4());
            boost::asio::socket_base::reuse_address sol_reuse_option(true);
            socket_ptr->set_option(sol_reuse_option);
            boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(ip), port);
            socket_ptr->async_connect(endpoint, [&](const boost::system::error_code& error) {
                if (error) {
                    throw EliteException(EliteException::Code::SOCKET_CONNECT_FAIL);
                }
            });
            io_context.run();

        } catch(const boost::system::system_error &error) {
            throw error;
        }
    }

    ~TcpClient() = default;
};


class ScriptSenderTest : public ::testing::Test {
protected:
    void SetUp() {
        TcpServer::start();
        script_sender_.reset(new ScriptSender(TEST_PORT, program_));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        tcp_client_.reset(new TcpClient("127.0.0.1", TEST_PORT));
    }

    void TearDown() {
        tcp_client_.reset();
        script_sender_.reset();
        TcpServer::stop();
    }

    std::unique_ptr<TcpClient> tcp_client_;
    std::unique_ptr<ScriptSender> script_sender_;
    std::string program_ = "print(\"success recv\")";
};

TEST_F(ScriptSenderTest, response_request) {
    
    std::unique_ptr<char[]> client_recv_program;
    client_recv_program.reset(new char[program_.length()]);

    tcp_client_->socket_ptr->non_blocking(false);

    EXPECT_TRUE(tcp_client_->socket_ptr->write_some(boost::asio::buffer(PROGRAM_REQUEST, sizeof(PROGRAM_REQUEST) - 1)) == (sizeof(PROGRAM_REQUEST) - 1));

    tcp_client_->socket_ptr->receive(boost::asio::buffer(client_recv_program.get(), program_.length()));

    ASSERT_EQ(client_recv_program.get(), program_);

}

TEST_F(ScriptSenderTest, mulit_clients) {
    
    std::unique_ptr<char[]> client_recv_program;
    client_recv_program.reset(new char[program_.length()]);

    std::unique_ptr<TcpClient> tcp_client_another = std::make_unique<TcpClient>("127.0.0.1", TEST_PORT);

    tcp_client_another->socket_ptr->non_blocking(false);

    EXPECT_TRUE(tcp_client_another->socket_ptr->write_some(boost::asio::buffer(PROGRAM_REQUEST, sizeof(PROGRAM_REQUEST) - 1)) == (sizeof(PROGRAM_REQUEST) - 1));

    tcp_client_another->socket_ptr->receive(boost::asio::buffer(client_recv_program.get(), program_.length()));

    ASSERT_EQ(client_recv_program.get(), program_);

}


int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}