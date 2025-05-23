#include <gtest/gtest.h>
#include <string>
#include <cstdint>
#include <chrono>

#include "Elite/RemoteUpgrade.hpp"
#include "Elite/Logger.hpp"

using namespace ELITE;

static std::string s_robot_ip;
static std::string s_upgrade_file;
static std::string s_robot_ssh_pw;

TEST(remoteUpgradeTest, upgrade_control_software) {
    EXPECT_TRUE(ELITE::UPGRADE::upgradeControlSoftware(s_robot_ip, s_upgrade_file, s_robot_ssh_pw));
}


int main(int argc, char** argv) {
    if (argc < 4 || argv[1] == nullptr || argv[2] == nullptr || argv[3] == nullptr) {
        std::cout << "cmd format:\n  RemoteUpgradeTest <robot ip> <upgrade file> <robot ssh password>" << std::endl;
        return 1;
    }
    ELITE::setLogLevel(ELITE::LogLevel::ELI_DEBUG);
    s_robot_ip = argv[1];
    s_upgrade_file = argv[2];
    s_robot_ssh_pw = argv[3];
    
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
