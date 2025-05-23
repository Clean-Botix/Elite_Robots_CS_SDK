#include "Primary/PrimaryPort.hpp"
#include "Primary/RobotConfPackage.hpp"
#include "Elite/Log.hpp"

#include <gtest/gtest.h>
#include <string>
#include <memory>
#include <thread>

using namespace ELITE;
using namespace std::chrono;

static std::string s_robot_ip = "192.168.51.127";

TEST(PrimaryPortTest, multiple_connect) {
    std::unique_ptr<PrimaryPort> primary =  std::make_unique<PrimaryPort>();

    for (size_t i = 0; i < 10; i++) {
        std::this_thread::sleep_for(500ms);
        EXPECT_TRUE(primary->connect(s_robot_ip, 30001));
        std::shared_ptr<KinematicsInfo> ki = std::make_shared<KinematicsInfo>();
        EXPECT_TRUE(primary->getPackage(ki, 500));
    }

    primary->disconnect();
    std::this_thread::sleep_for(500ms);
}


TEST(PrimaryPortTest, connect_disconnect) {
    std::unique_ptr<PrimaryPort> primary =  std::make_unique<PrimaryPort>();

    for (size_t i = 0; i < 10; i++) {
        EXPECT_TRUE(primary->connect(s_robot_ip, 30001));
        std::shared_ptr<KinematicsInfo> ki = std::make_shared<KinematicsInfo>();
        EXPECT_TRUE(primary->getPackage(ki, 2000));
        primary->disconnect();
        std::this_thread::sleep_for(500ms);
    }
}

TEST(PrimaryPortTest, get_package) {
    std::unique_ptr<PrimaryPort> primary =  std::make_unique<PrimaryPort>();

    EXPECT_TRUE(primary->connect(s_robot_ip, 30001));

    std::shared_ptr<KinematicsInfo> template_ki = std::make_shared<KinematicsInfo>();
    EXPECT_TRUE(primary->getPackage(template_ki, 500));

    std::shared_ptr<KinematicsInfo> ki = std::make_shared<KinematicsInfo>();
    for (size_t i = 0; i < 100; i++) {
        EXPECT_TRUE(primary->getPackage(ki, 500));
        for (size_t i = 0; i < 6; i++) {
            EXPECT_EQ(template_ki->dh_a_[i], ki->dh_a_[i]);
            EXPECT_EQ(template_ki->dh_d_[i], ki->dh_d_[i]);
            EXPECT_EQ(template_ki->dh_alpha_[i], ki->dh_alpha_[i]);
        }
    }
    primary->disconnect();
}


int main(int argc, char** argv) {
    setLogLevel(LogLevel::ELI_DEBUG);
    if(argc >= 2) {
        s_robot_ip = argv[1];
    }
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
