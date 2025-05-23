/**
 * @file PrimaryPortInterface.hpp
 * @author yanxiaojia
 * @brief Robot primary port interface
 * @date 2024-08-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __ELITE__PRIMARY_PORT_INTERFACE_HPP__
#define __ELITE__PRIMARY_PORT_INTERFACE_HPP__

#include <Elite/PrimaryPackage.hpp>
#include <Elite/EliteOptions.hpp>
#include <memory>
#include <string>

namespace ELITE
{

/**
 * @brief Robot primary port interface
 * 
 */
class PrimaryPortInterface
{
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
public:
    static constexpr int PRIMARY_PORT = 30001;

    ELITE_EXPORT PrimaryPortInterface();
    ELITE_EXPORT ~PrimaryPortInterface();

    /**
     * @brief Connect to robot primary port.
     *  And spawn a background thread for message receiving and parsing.
     * @param ip The robot ip
     * @param port The port(30001)
     * @return true success
     * @return false fail
     */
    ELITE_EXPORT bool connect(const std::string& ip, int port = PRIMARY_PORT);

    /**
     * @brief Disconnect socket.
     *  And wait for the background thread to finish.
     */
    ELITE_EXPORT void disconnect();

    /**
     * @brief Sends a custom script program to the robot.
     * 
     * @param script Script code that shall be executed by the robot.
     * @return true success
     * @return false fail
     */
    ELITE_EXPORT bool sendScript(const std::string& script);

    /**
     * @brief Get primary sub-package data.
     * 
     * @param pkg Primary sub-package. 
     * @param timeout_ms Wait time
     * @return true success
     * @return false fail
     */
    ELITE_EXPORT bool getPackage(std::shared_ptr<PrimaryPackage> pkg, int timeout_ms);

    /**
     * @brief Get the local IP
     * 
     * @return std::string Local IP. If empty, connection had some errors.
     */
    ELITE_EXPORT std::string getLocalIP();

};

} // namespace ELITE


#endif