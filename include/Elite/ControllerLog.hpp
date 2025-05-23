#ifndef ___ELITE_CONTROLLER_LOG_HPP__
#define ___ELITE_CONTROLLER_LOG_HPP__

#include <Elite/EliteOptions.hpp>
#include <functional>
#include <string>

namespace ELITE {
class ControllerLog {
   private:
   public:
    /**
     * @brief Download system log from robot
     *
     * @param robot_ip Robot ip address
     * @param password Robot ssh password
     * @param path Save path
     * @param progress_cb Download progress callback function.
     *      f_z: File size.
     *      r_z: Downloaded size.
     *      err: Error information (nullptr when there is no error)
     * @return true success
     * @return false fail
     *      1. On Linux, if `libssh` is not installed, you need to ensure that the computer running the SDK has the `scp`, `ssh`,
     * and `sshpass` commands available.
     *      2. In Windows, if libssh is not installed, then this interface will not be available.
     */
    ELITE_EXPORT static bool downloadSystemLog(const std::string &robot_ip, const std::string &password, const std::string &path,
                                               std::function<void(int f_z, int r_z, const char *err)> progress_cb);
    ControllerLog() {}
    ~ControllerLog() {}
};

}  // namespace ELITE

#endif
