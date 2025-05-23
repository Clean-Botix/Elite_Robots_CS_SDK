#include <gtest/gtest.h>
#include <string>
#include <thread>
#include "Common/TcpServer.hpp"
#include "boost/asio.hpp"
#include <iostream>

using namespace std::chrono;
using namespace ELITE;

#define SERVER_TEST_PORT (50001)

class TcpClient
{
public:
    boost::asio::io_context io_context;
    std::unique_ptr<boost::asio::ip::tcp::socket> socket_ptr;
    std::unique_ptr<boost::asio::ip::tcp::resolver> resolver_ptr;
    TcpClient() = default;
    
    TcpClient(const std::string& ip, int port) {
        connect(ip, port);
    }

    ~TcpClient() = default;

    void connect(const std::string& ip, int port) {
        try {
            socket_ptr.reset(new boost::asio::ip::tcp::socket(io_context));
            resolver_ptr.reset(new boost::asio::ip::tcp::resolver(io_context));
            socket_ptr->open(boost::asio::ip::tcp::v4());
            boost::asio::ip::tcp::no_delay no_delay_option(true);
            socket_ptr->set_option(no_delay_option);
            boost::asio::socket_base::reuse_address sol_reuse_option(true);
            socket_ptr->set_option(sol_reuse_option);
#if defined(__linux) || defined(linux) || defined(__linux__)
            boost::asio::detail::socket_option::boolean<IPPROTO_TCP, TCP_QUICKACK> quickack(true);
            socket_ptr->set_option(quickack);
#endif
            boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(ip), port);
            socket_ptr->async_connect(endpoint, [&](const boost::system::error_code& error) {
                if (error) {
                    throw boost::system::system_error(error);
                }
            });
            io_context.run();
        
        } catch(const boost::system::system_error &error) {
            throw error;
        }
    }
    
};



TEST(TCP_SERVER, TCP_SERVER_TEST) {
    TcpServer::start();
    std::shared_ptr<TcpServer> server = std::make_shared<TcpServer>(SERVER_TEST_PORT, 4);
    server->startListen();
    // Wait listen
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    TcpClient client("127.0.0.1", SERVER_TEST_PORT);
    // Wait connection
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_TRUE(server->isClientConnected());

    int send_data = 12345;
    bool receive_flag = false;
    server->setReceiveCallback([&](const uint8_t data[], int nb) {
        receive_flag = true;
        EXPECT_EQ(nb, 4);
        EXPECT_EQ(*(int*)data, send_data);
    });

    client.socket_ptr->send(boost::asio::buffer(&send_data, sizeof(send_data)));
    // Wait send
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    EXPECT_TRUE(receive_flag);
    // Clear flag
    receive_flag = false;

    TcpServer::stop();
}

TEST(TCP_SERVER, TCP_SERVER_MULIT_CONNECT) {
    const std::string client_send_string = "client_send_string\n";
    TcpServer::start();
    std::shared_ptr<TcpServer> server = std::make_shared<TcpServer>(SERVER_TEST_PORT, client_send_string.length());
    server->startListen();
    // Wait listen
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    TcpClient client1;
    TcpClient client2;

    client1.connect("127.0.0.1", SERVER_TEST_PORT);
    // Wait connected
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_TRUE(server->isClientConnected());
    bool receive_flag = false;
    server->setReceiveCallback([&](const uint8_t data[], int nb) {
        receive_flag = true;
        EXPECT_EQ(nb, client_send_string.length());

        std::string str((const char *)data);
        EXPECT_EQ(str, client_send_string);
    });

    EXPECT_EQ(client1.socket_ptr->write_some(boost::asio::buffer(client_send_string)), client_send_string.length());
    // Wait for recv
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    EXPECT_TRUE(receive_flag);
    // Clear flag
    receive_flag = false;
    
    // New connection
    client2.connect("127.0.0.1", SERVER_TEST_PORT);

    // Wait connected
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_TRUE(server->isClientConnected());

    EXPECT_EQ(client2.socket_ptr->write_some(boost::asio::buffer(client_send_string)), client_send_string.length());
    // Wait for recv
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    EXPECT_TRUE(receive_flag);
    // Clear flag
    receive_flag = false;

    client1.socket_ptr->close();
    client2.socket_ptr->close();

    // Wait close arrive
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    EXPECT_FALSE(server->isClientConnected());

    // Wait close
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // After client close, reconnect
    client1.connect("127.0.0.1", SERVER_TEST_PORT);
    // Wait connected
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    ASSERT_EQ(client1.socket_ptr->write_some(boost::asio::buffer(client_send_string)), client_send_string.length());
    // Wait for recv
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // Stop TCP context
    TcpServer::stop();
}

TEST(TCP_SERVER, TCP_MULIT_SERVERS) {
    const std::string client_send_string = "client_send_string\n";   

    TcpServer::start();

    std::vector<std::shared_ptr<TcpServer>> servers;
    for (size_t i = 0; i < 5; i++) {
        servers.push_back(std::make_shared<TcpServer>(SERVER_TEST_PORT + i, client_send_string.length()));
        servers[i]->startListen();
    }
    // Wait listen
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::vector<std::unique_ptr<TcpClient>> clients;
    for (size_t i = 0; i < 5; i++) {
        clients.push_back(std::make_unique<TcpClient>());
        clients[i]->connect("127.0.0.1", SERVER_TEST_PORT + i);
    }
    // Wait connected
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    for (size_t i = 0; i < 5; i++) {
        ASSERT_TRUE(servers[i]->isClientConnected());

        servers[i]->setReceiveCallback([&](const uint8_t data[], int nb) {
            EXPECT_EQ(nb, client_send_string.length());
    
            std::string str((const char *)data);
            EXPECT_EQ(str, client_send_string);
        });

        ASSERT_EQ(clients[i]->socket_ptr->write_some(boost::asio::buffer(client_send_string)), client_send_string.length());
    }
    // Wait for recv
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    for (size_t i = 0; i < 5; i++) {
        clients[i]->socket_ptr->close();
    }

    TcpServer::stop();
}


int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}