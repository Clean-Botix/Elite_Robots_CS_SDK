#include <iostream>
#include <cstdio>
#include <string>

#include "Common/SshUtils.hpp"
#include "Elite/Logger.hpp"

using namespace ELITE::SSH_UTILS;

namespace ELITE
{

namespace UPGRADE
{

bool upgradeControlSoftware(std::string ip, std::string file, std::string password) {
	auto upload_error_cb = [&](int f_z, int r_z, const char* err) {
		if (err) {
			ELITE_LOG_ERROR("Upload update file fail %d/%d. Reason: %s ", r_z, f_z, err);
		}
	};
	// Upload update package
	if (!uploadFile(ip, "root", password, "/tmp/CS_UPDATE.eup", file, upload_error_cb)) {
		return false;
	}

	// Add executable permissions to the upgrade package.
	std::string cmd = "chmod +x /tmp/CS_UPDATE.eup";
	std::string cmd_out = executeCommand(ip, "root", password, cmd);
	ELITE_LOG_DEBUG("Execute cmd: %s\n Output:%s", cmd.c_str(), cmd_out.c_str());

	// Execute the upgrade package in the bash environment.
	cmd = "bash -lc '/tmp/CS_UPDATE.eup --app'";
	cmd_out = executeCommand(ip, "root", password, cmd);
	ELITE_LOG_DEBUG("Execute cmd: %s\n Output:%s", cmd.c_str(), cmd_out.c_str());
	return true;
}

} // namespace UPGRADE


} // namespace ELITE


