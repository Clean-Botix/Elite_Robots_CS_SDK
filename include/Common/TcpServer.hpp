#ifndef __TCP_SERVER_HPP__
#define __TCP_SERVER_HPP__

#include <atomic>
#include <boost/asio.hpp>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace ELITE {

class TcpServer : public std::enable_shared_from_this<TcpServer> {
   public:
    /**
     * @brief Start the backend thread of the TCP server, where both accept and read operations of the TcpServer class will be
     * completed
     * @note
     *      1. It must be called before instantiation of any TcpServer class, or the constructor of TcpServer will throw an
     * exception
     *      2. Currently called in the constructor of the EliteDriver class
     */
    static void start();

    /**
     * @brief Terminate the backend thread of the TCP server
     * @note
     *      1. Currently called in the destructor of the EliteDriver class
     */
    static void stop();

    // Read callback
    using ReceiveCallback = std::function<void(const uint8_t[], int)>;

    /**
     * @brief Construct a new Tcp Server object
     *
     * @param port Listen port
     * @param recv_buf_size
     * @note Ensure that the start() method has been called before instantiation
     */
    TcpServer(int port, int recv_buf_size);

    /**
     * @brief Destroy the Tcp Server object
     *
     */
    ~TcpServer();

    /**
     * @brief Set the Receive Callback
     *
     * @param cb receive callback
     */
    void setReceiveCallback(ReceiveCallback cb);

    /**
     * @brief Write data to client
     *
     * @param data data
     * @param size The number of bytes in the data
     * @return int Success send bytes
     */
    int writeClient(void* data, int size);

    /**
     * @brief Start listen port
     *
     */
    void startListen();

    /**
     * @brief Determine if there is a client connected
     *
     * @return true Connected
     * @return false Disconnected
     */
    bool isClientConnected();

   protected:
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
    // Each instance will copy a the s_io_comtext_ptr_ pointer
    std::shared_ptr<boost::asio::io_context> io_context_;

   private:
    // Save connected client. In this project, each server is only connected to one client.
    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
    std::vector<uint8_t> read_buffer_;
    ReceiveCallback receive_cb_;
    std::mutex socket_mutex_;

    // Boost io_context and backend thread.
    // All servers use the same io_comtext and thread.
    static std::unique_ptr<std::thread> s_server_thread_;
    static std::shared_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> s_work_guard_ptr_;
    static std::shared_ptr<boost::asio::io_context> s_io_context_ptr_;

    /**
     * @brief Async accept client connection and add async read task
     *
     */
    virtual void doAccept();

    /**
     * @brief Async receive
     *
     * @param sock Client socket
     */
    void doRead(std::shared_ptr<boost::asio::ip::tcp::socket> sock);

    /**
     * @brief Cancle client asnyc task and close client connection
     *
     * @param sock client socket
     * @param ec boost error code (Only close() function ec is available).
     */
    void closeSocket(std::shared_ptr<boost::asio::ip::tcp::socket> sock, boost::system::error_code& ec);
};

}  // namespace ELITE
#endif
