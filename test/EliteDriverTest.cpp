#include "Elite/EliteDriver.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <chrono>
#include <thread>

using namespace ELITE;
using namespace std::chrono;

static std::string s_robot_ip = "192.168.51.127";
static std::string s_local_ip = "192.168.51.95";

TEST(EliteDriverTest, ConstructorsAndDestructors) {
    EliteDriverConfig config;
    config.local_ip = s_local_ip;
    config.robot_ip = s_robot_ip;
    config.script_file_path = "external_control.script";
    config.headless_mode = true;
    for (size_t i = 0; i < 100; i++) {
        try {
            auto driver = std::make_unique<EliteDriver>(config);
            if (!driver) {
                continue;
            }
            while (!driver->isRobotConnected()) {
                std::this_thread::sleep_for(4ms);
            }
            std::this_thread::sleep_for(50ms);
            EXPECT_TRUE(driver->stopControl());
            driver.reset();
        } catch(const std::exception& e) {
            continue;
        }
    }
}


int main(int argc, char** argv) {
    if(argc >= 3) {
        s_robot_ip = argv[1];
        s_local_ip = argv[2];
    }
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}