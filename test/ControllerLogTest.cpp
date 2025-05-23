#include <gtest/gtest.h>
#include <string>
#include <cstdint>
#include <chrono>
#include <iostream>

#include "Elite/ControllerLog.hpp"
#include "Elite/Log.hpp"

using namespace ELITE;

static std::string s_robot_ip;
static std::string s_robot_ssh_pw;

TEST(controllerLogTest, download_system_log) {
    const char* download_err = nullptr;
    bool result = ELITE::ControllerLog::downloadSystemLog(
        s_robot_ip, 
        s_robot_ssh_pw, 
        "./log_history.csv", 
        [](int f_z, int r_z, const char* err) {
            std::cout << "\rDownloaded: " << r_z << "/" << f_z << " bytes" << std::flush;
        }
    );
    EXPECT_TRUE(result);
    EXPECT_TRUE(download_err == nullptr);
}


int main(int argc, char** argv) {
    if (argc < 3 || argv[1] == nullptr || argv[2] == nullptr ) {
        std::cout << "cmd format:\n  ControllerLogTest <robot ip> <robot ssh password>" << std::endl;
        return 1;
    }
    ELITE::setLogLevel(ELITE::LogLevel::ELI_DEBUG);
    s_robot_ip = argv[1];
    s_robot_ssh_pw = argv[2];
    
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
