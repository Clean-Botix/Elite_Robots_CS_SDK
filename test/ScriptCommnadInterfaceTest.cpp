#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <memory>
#include <thread>

#include "ScriptCommandInterface.hpp"
#include "ControlCommon.hpp"

using namespace ELITE;
using namespace std::chrono;

#define SCRIPT_COMMAND_INTERFACE_TEST_PORT 50004

#define ARRAY_EQUAL_ASSERT(a, b)  \
    for (size_t i = 0; i < ScriptCommandInterface::SCRIPT_COMMAND_DATA_SIZE; i++) { \
        ASSERT_EQ(a[i], b[i]); \
    } \

enum Cmd{
    ZERO_FTSENSOR = 0,
    SET_PAYLOAD = 1,
    SET_TOOL_VOLTAGE = 2,
    START_FORCE_MODE = 3,
    END_FORCE_MODE = 4,
};


class TcpClient
{
public:
    boost::asio::io_context io_context;
    std::unique_ptr<boost::asio::ip::tcp::socket> socket_ptr;
    std::unique_ptr<boost::asio::ip::tcp::resolver> resolver_ptr;
    TcpClient() = default;
    
    TcpClient(const std::string& ip, int port) {
        connect(ip, port);
    }

    ~TcpClient() = default;

    void connect(const std::string& ip, int port) {
        try {
            socket_ptr.reset(new boost::asio::ip::tcp::socket(io_context));
            resolver_ptr.reset(new boost::asio::ip::tcp::resolver(io_context));
            socket_ptr->open(boost::asio::ip::tcp::v4());
            boost::asio::ip::tcp::no_delay no_delay_option(true);
            socket_ptr->set_option(no_delay_option);
            boost::asio::socket_base::reuse_address sol_reuse_option(true);
            socket_ptr->set_option(sol_reuse_option);
#if defined(__linux) || defined(linux) || defined(__linux__)
            boost::asio::detail::socket_option::boolean<IPPROTO_TCP, TCP_QUICKACK> quickack(true);
            socket_ptr->set_option(quickack);
#endif
            boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(ip), port);
            socket_ptr->async_connect(endpoint, [&](const boost::system::error_code& error) {
                if (error) {
                    throw boost::system::system_error(error);
                }
            });
            io_context.run();
        
        } catch(const boost::system::system_error &error) {
            throw error;
        }
    }
    
};

TEST(ScriptCommandInterfaceTest, SendAndReceive) {
    TcpServer::start();
    std::unique_ptr<ScriptCommandInterface> script_cmd;
    script_cmd.reset(new ScriptCommandInterface(SCRIPT_COMMAND_INTERFACE_TEST_PORT));
    
    std::unique_ptr<TcpClient> client;
    client.reset(new TcpClient());
    EXPECT_NO_THROW(client->connect("127.0.0.1", SCRIPT_COMMAND_INTERFACE_TEST_PORT));

    while (!script_cmd->isRobotConnect()) {
        std::this_thread::sleep_for(4ms);
    }

    int32_t buffer[ScriptCommandInterface::SCRIPT_COMMAND_DATA_SIZE];

    // zeroFTSensor() interface test
    script_cmd->zeroFTSensor();
    memset(buffer, 0, sizeof(buffer));
    int recv_len = client->socket_ptr->read_some(boost::asio::buffer(buffer));
    ASSERT_EQ(recv_len / sizeof(int32_t), ScriptCommandInterface::SCRIPT_COMMAND_DATA_SIZE);
    int32_t zero_ft_sensor_buffer[ScriptCommandInterface::SCRIPT_COMMAND_DATA_SIZE] = {0};
    zero_ft_sensor_buffer[0] = htonl(Cmd::ZERO_FTSENSOR);
    ARRAY_EQUAL_ASSERT(buffer, zero_ft_sensor_buffer);
    
    // setToolVoltage() interface test
    script_cmd->setToolVoltage(ToolVoltage::V_12);
    memset(buffer, 0, sizeof(buffer));
    recv_len = client->socket_ptr->read_some(boost::asio::buffer(buffer));
    ASSERT_EQ(recv_len / sizeof(int32_t), ScriptCommandInterface::SCRIPT_COMMAND_DATA_SIZE);
    int32_t set_tool_voltage_buffer[ScriptCommandInterface::SCRIPT_COMMAND_DATA_SIZE] = {0};
    set_tool_voltage_buffer[0] = htonl(Cmd::SET_TOOL_VOLTAGE);
    set_tool_voltage_buffer[1] = htonl((int32_t)ToolVoltage::V_12 * CONTROL::COMMON_ZOOM_RATIO);
    ARRAY_EQUAL_ASSERT(buffer, set_tool_voltage_buffer);

    // setPayload() interface test
    script_cmd->setPayload(1, {1, 1, 1});
    memset(buffer, 0, sizeof(buffer));
    recv_len = client->socket_ptr->read_some(boost::asio::buffer(buffer));
    ASSERT_EQ(recv_len / sizeof(int32_t), ScriptCommandInterface::SCRIPT_COMMAND_DATA_SIZE);
    double set_payload_buffer[ScriptCommandInterface::SCRIPT_COMMAND_DATA_SIZE] = {0};
    set_payload_buffer[0] = htonl(buffer[0]);
    set_payload_buffer[1] = htonl(buffer[1]) / (double)CONTROL::COMMON_ZOOM_RATIO;
    set_payload_buffer[2] = htonl(buffer[2]) / (double)CONTROL::COMMON_ZOOM_RATIO;
    set_payload_buffer[3] = htonl(buffer[3]) / (double)CONTROL::COMMON_ZOOM_RATIO;
    set_payload_buffer[4] = htonl(buffer[4]) / (double)CONTROL::COMMON_ZOOM_RATIO;
    double set_payload_buffer_target[ScriptCommandInterface::SCRIPT_COMMAND_DATA_SIZE] = {0};
    set_payload_buffer_target[0] = Cmd::SET_PAYLOAD;
    set_payload_buffer_target[1] = 1;
    set_payload_buffer_target[2] = 1;
    set_payload_buffer_target[3] = 1;
    set_payload_buffer_target[4] = 1;
    ARRAY_EQUAL_ASSERT(set_payload_buffer, set_payload_buffer_target);

    // startForceMode() interface test
    vector6d_t force_test_d_data{1, 2, 3, 4, 5, 6};
    vector6int32_t force_test_i_data{1, 2, 3, 4, 5, 6};
    script_cmd->startForceMode(force_test_d_data, force_test_i_data, force_test_d_data, ForceMode::MOTION, force_test_d_data);
    memset(buffer, 0, sizeof(buffer));
    recv_len = client->socket_ptr->read_some(boost::asio::buffer(buffer));
    ASSERT_EQ(recv_len / sizeof(int32_t), ScriptCommandInterface::SCRIPT_COMMAND_DATA_SIZE);
    double start_force_mode_buffer[ScriptCommandInterface::SCRIPT_COMMAND_DATA_SIZE] = {0};
    start_force_mode_buffer[0] = htonl(buffer[0]);
    for (size_t i = 1; i <= 6; i++) {
        start_force_mode_buffer[i] = htonl(buffer[i]) / (double)CONTROL::COMMON_ZOOM_RATIO;
    }
    for (size_t i = 7; i <= 12; i++) {
        start_force_mode_buffer[i] = htonl(buffer[i]);
    }
    for (size_t i = 13; i <= 18; i++) {
        start_force_mode_buffer[i] = htonl(buffer[i]) / (double)CONTROL::COMMON_ZOOM_RATIO;
    }
    start_force_mode_buffer[19] = htonl(buffer[19]);
    for (size_t i = 20; i < 26; i++) {
        start_force_mode_buffer[i] = htonl(buffer[i]) / (double)CONTROL::COMMON_ZOOM_RATIO;
    }
    double start_force_mode_buffer_target[ScriptCommandInterface::SCRIPT_COMMAND_DATA_SIZE] = {0};
    start_force_mode_buffer_target[0] = Cmd::START_FORCE_MODE;
    double *start_force_mode_buffer_target_ptr = &start_force_mode_buffer_target[1];
    for (auto& ft : force_test_d_data) {
        *start_force_mode_buffer_target_ptr = ft;
        start_force_mode_buffer_target_ptr++;
    }
    for (auto& ft : force_test_i_data) {
        *start_force_mode_buffer_target_ptr = ft;
        start_force_mode_buffer_target_ptr++;
    }
    for (auto& ft : force_test_d_data) {
        *start_force_mode_buffer_target_ptr = ft;
        start_force_mode_buffer_target_ptr++;
    }
    *start_force_mode_buffer_target_ptr = (double)ForceMode::MOTION;
    start_force_mode_buffer_target_ptr++;
    for (auto& ft : force_test_d_data) {
        *start_force_mode_buffer_target_ptr = ft;
        start_force_mode_buffer_target_ptr++;
    }
    ARRAY_EQUAL_ASSERT(start_force_mode_buffer, start_force_mode_buffer_target);

    script_cmd->endForceMode();
    memset(buffer, 0, sizeof(buffer));
    recv_len = client->socket_ptr->read_some(boost::asio::buffer(buffer));
    ASSERT_EQ(recv_len / sizeof(int32_t), ScriptCommandInterface::SCRIPT_COMMAND_DATA_SIZE);
    int32_t end_force_mode_buffer[ScriptCommandInterface::SCRIPT_COMMAND_DATA_SIZE] = {0};
    end_force_mode_buffer[0] = htonl(Cmd::END_FORCE_MODE);
    ARRAY_EQUAL_ASSERT(buffer, end_force_mode_buffer);

    TcpServer::stop();
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}