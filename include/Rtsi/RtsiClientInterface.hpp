/**
 * @file RtsiClientInterface.hpp
 * @author yanxiaojia
 * @brief The RTSI client interface
 * @date 2024-08-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __RTSI_CLIENT_INTERFACE_HPP__
#define __RTSI_CLIENT_INTERFACE_HPP__

#include <Elite/RtsiRecipe.hpp>
#include <Elite/VersionInfo.hpp>
#include <Elite/EliteOptions.hpp>
#include <Elite/RtsiConstants.hpp>

#include <memory>


namespace ELITE {

/**
 * @brief The RTSI client raw interface.
 * 
 */
class RtsiClientInterface
{
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
public:
    static constexpr uint16_t DEFAULT_PROTOCOL_VERSION = 1;

    ELITE_EXPORT RtsiClientInterface();
    ELITE_EXPORT virtual ~RtsiClientInterface();

    /**
     * @brief Connect to robot RTSI server
     * 
     * @param ip The robot IP
     * @param port RTSI port
     */
    ELITE_EXPORT virtual bool connect(const std::string& ip, int port = DEFAULT_RTSI_PORT);

    /**
     * @brief Disconnect
     * 
     */
    ELITE_EXPORT virtual void disconnect();

    /**
     * @brief Verify the protocol version.
     * 
     * @param version The version of RTSI
     * @return bool true if successful
     */
    ELITE_EXPORT bool negotiateProtocolVersion(uint16_t version = DEFAULT_PROTOCOL_VERSION);

    /**
     * @brief Get the Controller Version object
     * 
     * @return std::tuple<uint32_t, uint32_t, uint32_t, uint32_t> 
     * A tuple type where the data is, in order, major version, minor version, bugfix, and build. 
     */
    ELITE_EXPORT virtual VersionInfo getControllerVersion();

    /**
     * @brief Subscribe to output variables.
     * 
     * @param recipe_list The list of recipe. The variable names are explained in the document.
     * @param frequency Setup output frenqucy
     * @return RtsiRecipeSharedPtr The data recipe
     */
    ELITE_EXPORT RtsiRecipeSharedPtr setupOutputRecipe(const std::vector<std::string>& recipe_list, double frequency = 250);

    /**
     * @brief Subscribe to input variables.
     * 
     * @param recipe The list of recipe. The variable names are explained in the document.
     * @return RtsiRecipeSharedPtr The data recipe
     */
    ELITE_EXPORT RtsiRecipeSharedPtr setupInputRecipe(const std::vector<std::string>& recipe);

    /**
     * @brief Send start signal to server
     * 
     * @return true Start successfully
     * @return false Start fail
     */
    ELITE_EXPORT bool start();

    /**
     * @brief Send pause signal to server
     * 
     * @return true Pause successfully
     * @return false Pause fail
     */
    ELITE_EXPORT bool pause();

    /**
     * @brief Send an recipe to controller
     * 
     * @param recipe  The recipe sent to the controller.
     */
    ELITE_EXPORT void send(RtsiRecipeSharedPtr& recipe);

    /**
     * @brief Receive RTSI output recipes data
     * 
     * @param recipes The recipe you want to receive. Note that only one recipe will be received.
     * @param read_newest If want to parser the newest message
     * @return int The ID of recipe which is received. If -1, not match recipe
     */
    ELITE_EXPORT int receiveData(std::vector<RtsiRecipeSharedPtr>& recipes, bool read_newest = false);

    /**
     * @brief Receive RTSI output recipe data
     * 
     * @param recipe The recipe you want to receive.
     * @param read_newest If want to parser the newest message
     * @return true success
     * @return false false
     */
    bool receiveData(RtsiRecipeSharedPtr recipe, bool read_newest = false);

    /**
     * @brief Get connection state
     * 
     * @return true connected
     * @return false disconnect
     */
    ELITE_EXPORT bool isConnected();

    /**
     * @brief Is start to sync robot data
     * 
     * @return true started
     * @return false not started
     */
    ELITE_EXPORT bool isStarted();

    /**
     * @brief This function is used to determine have bytes that may be read without blocking.
     * 
     * @return true has bytes
     * @return false don't has
     */
    ELITE_EXPORT bool isReadAvailable();

};

}




#endif
