#include <Elite/DashboardClient.hpp>
#include <Elite/DataType.hpp>
#include <Elite/EliteDriver.hpp>
#include <Elite/RtsiIOInterface.hpp>

#include <iostream>
#include <memory>

using namespace ELITE;

static std::unique_ptr<EliteDriver> s_driver;
static std::unique_ptr<DashboardClient> s_dashboard;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Must provide robot ip or local ip. Command like: \"./speedl_example robot_ip\" or \"./speedl_example "
                     "robot_ip local_ip\""
                  << std::endl;
        return 1;
    }
    std::string robot_ip = argv[1];
    std::string local_ip = "";
    if (argc >= 3) {
        local_ip = argv[2];
    }
    EliteDriverConfig config;
    config.robot_ip = robot_ip;
    config.script_file_path = "external_control.script";
    config.local_ip = local_ip;
    s_driver = std::make_unique<EliteDriver>(config);
    s_dashboard = std::make_unique<DashboardClient>();

    if (!s_dashboard->connect(argv[1])) {
        return 1;
    }
    std::cout << "Dashboard connected" << std::endl;

    if (!s_dashboard->powerOn()) {
        return 1;
    }
    std::cout << "Robot power on" << std::endl;

    if (!s_dashboard->brakeRelease()) {
        return 1;
    }
    std::cout << "Robot brake released" << std::endl;

    if (!s_dashboard->playProgram()) {
        return 1;
    }
    std::cout << "Program run" << std::endl;

    while (!s_driver->isRobotConnected()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    vector6d_t speedl_vector = {0, 0, -0.02, 0, 0, 0};
    s_driver->writeSpeedl(speedl_vector, 0);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    speedl_vector = {0, 0, 0.02, 0, 0, 0};
    s_driver->writeSpeedl(speedl_vector, 0);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    s_driver->stopControl();

    return 0;
}
