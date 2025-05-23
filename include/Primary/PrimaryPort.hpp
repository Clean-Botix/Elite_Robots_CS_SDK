#ifndef __ELITE__PRIMARY_PORT_HPP__
#define __ELITE__PRIMARY_PORT_HPP__

#include "PrimaryPackage.hpp"
#include "DataType.hpp"

#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>
#include <mutex>

namespace ELITE
{

class PrimaryPort
{
private:
    // The primary port package head length
    static constexpr int HEAD_LENGTH = 5;
    // The type of 'RobotState' package
    static constexpr int ROBOT_STATE_MSG_TYPE = 16;

    std::mutex socket_mutex_;
    boost::asio::io_context io_context_;
    std::unique_ptr<boost::asio::ip::tcp::socket> socket_ptr_;
    
    // The buffer of package head
    std::vector<uint8_t> message_head_;
    // The buffer of package body
    std::vector<uint8_t> message_body_;

    // When getPackage() be called, insert a sub-package data to get
    std::unordered_map<int, std::shared_ptr<PrimaryPackage>> parser_sub_msg_;
    std::unique_ptr<std::thread> socket_async_thread_;
    std::mutex mutex_;
    bool socket_async_thread_alive_;
    
    /**
     * @brief The background thread.
     *  Receive and parser package.
     */
    void socketAsyncLoop(const std::string& ip, int port);

    /**
     * @brief Receive and parser package.
     * 
     */
    bool parserMessage();

    /**
     * @brief Receive and parser package body.
     *  Only parser 'RobotState' package
     * @param type 
     * @param len 
     */
    bool parserMessageBody(int type, int package_len);

    /**
     * @brief Connect to robot primary port.
     * 
     * @param ip The robot ip
     * @param port The port(30001 or 30002)
     * @return true 
     * @return false 
     */
    bool socketConnect(const std::string& ip, int port);

    /**
     * @brief Close connection socket
     * 
     */
    void socketDisconnect();

public:
    PrimaryPort();
    ~PrimaryPort();

    /**
     * @brief Connect to robot primary port.
     *  And spawn a background thread for message receiving and parsing.
     * @param ip The robot ip
     * @param port The port(30001 or 30002)
     * @return true success
     * @return false fail
     * @note 
     *      1. Warning: Repeated calls to this function without intermediate disconnect() will force-close the active connection.
     *      2. Usage constraint: Call rate must be ≤ 2Hz (once per 500ms minimum interval).
     */
    bool connect(const std::string& ip, int port);

    /**
     * @brief Disconnect socket.
     *  And wait for the background thread to finish.
     * @note After calling this function, a delay of around 500ms should be added prior to calling connect().
     */
    void disconnect();

    /**
     * @brief Sends a custom script program to the robot.
     * 
     * @param script Script code that shall be executed by the robot.
     * @return true success
     * @return false fail
     */
    bool sendScript(const std::string& script);

    /**
     * @brief Get primary sub-package data.
     * 
     * @param pkg Primary sub-package. 
     * @param timeout_ms Wait time
     * @return true success
     * @return false fail
     */
    bool getPackage(std::shared_ptr<PrimaryPackage> pkg, int timeout_ms);

    /**
     * @brief Get the local IP
     * 
     * @return std::string Local IP. If empty, connection had some errors.
     */
    std::string getLocalIP();
};

} // namespace ELITE

#endif