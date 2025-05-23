#ifndef __ELITE__REMOTE_UPGRADE_HPP__
#define __ELITE__REMOTE_UPGRADE_HPP__

#include <Elite/EliteOptions.hpp>
#include <string>

namespace ELITE {

namespace UPGRADE {
/**
 * @brief Upgrade the robot control software
 *
 * @param ip Robot ip
 * @param file Upgrade file
 * @param password Robot controller ssh password
 * @return true success
 * @return false fail
 * @note
 *      1. On Linux, if `libssh` is not installed, you need to ensure that the computer running the SDK has the `scp`, `ssh`, and
 * `sshpass` commands available.
 *      2. In Windows, if libssh is not installed, then this interface will not be available.
 */
ELITE_EXPORT bool upgradeControlSoftware(std::string ip, std::string file, std::string password);

}  // namespace UPGRADE
}  // namespace ELITE

#endif