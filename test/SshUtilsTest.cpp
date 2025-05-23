#include <gtest/gtest.h>
#include <string>
#include <cstdint>
#include <chrono>
#include <fstream>

#include "Common/SshUtils.hpp"
#include "Elite/Logger.hpp"

using namespace ELITE;

static std::string s_user;
static std::string s_ip;
static std::string s_ssh_pw;

TEST(sshUtilsTest, ssh_utils_test) {
    std::string cmd_output = 
        SSH_UTILS::executeCommand(
            s_ip, 
            s_user, 
            s_ssh_pw, 
            "printenv PATH");
    EXPECT_NE(cmd_output.find("/usr/bin"), cmd_output.npos);
}

TEST(sshUtilsTest, ssh_file_test) {
    std::ofstream o_file("./sdk_test.txt", std::ios::out | std::ios::trunc);
    o_file << "abcd" << std::endl;
    o_file.close();
    
    EXPECT_TRUE(SSH_UTILS::uploadFile(s_ip, s_user, s_ssh_pw, "/tmp/sdk_test.txt", "./sdk_test.txt", nullptr));
    EXPECT_TRUE(SSH_UTILS::downloadFile(s_ip, s_user, s_ssh_pw, "/tmp/sdk_test.txt", "./sdk_test_dl.txt", nullptr));

    std::ifstream i_file("./sdk_test_dl.txt");
    std::string text;
    i_file >> text;
    EXPECT_EQ(text, "abcd");
}

int main(int argc, char** argv) {
    if (argc < 4 || argv[1] == nullptr || argv[2] == nullptr || argv[3] == nullptr) {
        std::cout << "cmd format:\n  SshUtilsTest <user> <ip> <password>" << std::endl;
        return 1;
    }
    ELITE::setLogLevel(ELITE::LogLevel::ELI_DEBUG);
    s_user = argv[1];
    s_ip = argv[2];
    s_ssh_pw = argv[3];
    
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
