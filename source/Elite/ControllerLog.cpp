#include "Elite/ControllerLog.hpp"
#include "Elite/Log.hpp"
#include "Common/SshUtils.hpp"

#include <cstdlib>
#include <algorithm>

namespace ELITE {
bool ControllerLog::downloadSystemLog(const std::string &robot_ip,
                                      const std::string &password,
                                      const std::string &path, 
                                      std::function<void (int f_z, int r_z, const char *err)> progress_cb) {
    
    std::string command = "bash -lc 'printenv RT_ROBOT_DATA_PATH'";
    std::string remote_path = SSH_UTILS::executeCommand(robot_ip, "root", password, command);
    // Erase '\n'
    remote_path.erase(std::remove(remote_path.begin(), remote_path.end(), '\n'), remote_path.end());
    remote_path += "log/log_history.csv";
    ELITE_LOG_DEBUG("Remote path: %s", remote_path.c_str());
    return SSH_UTILS::downloadFile(robot_ip, "root", password, remote_path, path, progress_cb);
}

} // namespace ELITE