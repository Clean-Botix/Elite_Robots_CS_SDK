#include "EliteDriver.hpp"
#include "EliteException.hpp"
#include "ReverseInterface.hpp"
#include "TrajectoryInterface.hpp"
#include "ScriptSender.hpp"
#include "ScriptCommandInterface.hpp"
#include "ControlCommon.hpp"
#include "ControlMode.hpp"
#include "PrimaryPortInterface.hpp"
#include "Log.hpp"
#include <boost/asio.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace ELITE;

static const std::string SERVER_IP_REPLACE = "{{SERVER_IP_REPLACE}}";
static const std::string REVERSE_PORT_REPLACE = "{{REVERSE_PORT_REPLACE}}";
static const std::string SCRIPT_COMMAND_PORT_REPLACE = "{{SCRIPT_COMMAND_PORT_REPLACE}}";
static const std::string TRAJECTORY_SERVER_PORT_REPLACE = "{{TRAJECTORY_SERVER_PORT_REPLACE}}";
static const std::string SERVO_J_REPLACE = "{{SERVO_J_REPLACE}}";
static const std::string POS_ZOOM_RATIO_REPLACE = "{{POS_ZOOM_RATIO_REPLACE}}";
static const std::string TIME_ZOOM_RATIO_REPLACE = "{{TIME_ZOOM_RATIO_REPLACE}}";
static const std::string COMMON_ZOOM_RATIO_REPLACE = "{{COMMON_ZOOM_RATIO_REPLACE}}";
static const std::string REVERSE_DATA_SIZE_REPLACE = "{{REVERSE_DATA_SIZE_REPLACE}}";
static const std::string TRAJECTORY_DATA_SIZE_REPLACE = "{{TRAJECTORY_DATA_SIZE_REPLACE}}";
static const std::string SCRIPT_COMMAND_DATA_SIZE_REPLACE = "{{SCRIPT_COMMAND_DATA_SIZE_REPLACE}}";

class EliteDriver::Impl {
public:
    Impl() = delete;
    explicit Impl(const std::string& robot_ip, const std::string& local_ip, int script_sender_port) 
        : robot_ip_(robot_ip),
          local_ip_(local_ip),
          script_sender_port_(script_sender_port) {
    }

    std::string readScriptFile(const std::string& file);
    void scriptParamWrite(std::string& file_string, int reverse_port, int trajectory_port, \
                          int script_command_port, float servoj_time, float servoj_lookhead_time, 
                          int servoj_gain);
    std::string robot_script_;
    std::string control_script_;
    std::string robot_ip_;
    std::string local_ip_;
    int script_sender_port_;
    std::unique_ptr<ReverseInterface> reverse_server_;
    std::unique_ptr<TrajectoryInterface> trajectory_server_;
    std::unique_ptr<ScriptSender> script_sender_;
    std::unique_ptr<ScriptCommandInterface> script_command_server_;
    std::unique_ptr<PrimaryPortInterface> primary_port_;
    bool headless_mode_;
};


std::string EliteDriver::Impl::readScriptFile(const std::string& filepath) {
    std::ifstream ifs;
    ifs.open(filepath);
    if (!ifs) {
        std::stringstream ss;
        ss << "Elite script file '" << filepath << "' doesn't exists.";
        throw EliteException(EliteException::Code::FILE_OPEN_FAIL, ss.str().c_str());
        return std::string();
    }
    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    ifs.close();
    return content;
}

void EliteDriver::Impl::scriptParamWrite(std::string& file_string, int reverse_port, int trajectory_port, 
                                         int script_command_port, float servoj_time, float servoj_lookhead_time, 
                                         int servoj_gain) {

    while (file_string.find(SERVER_IP_REPLACE) != std::string::npos) {
        file_string.replace(file_string.find(SERVER_IP_REPLACE), 
                            SERVER_IP_REPLACE.length(), 
                            local_ip_);
    }
    
    while (file_string.find(TRAJECTORY_SERVER_PORT_REPLACE) != std::string::npos) {
        file_string.replace(file_string.find(TRAJECTORY_SERVER_PORT_REPLACE), 
                            TRAJECTORY_SERVER_PORT_REPLACE.length(), 
                            std::to_string(trajectory_port));
    }
    
    while (file_string.find(REVERSE_PORT_REPLACE) != std::string::npos) {
        file_string.replace(file_string.find(REVERSE_PORT_REPLACE), 
                            REVERSE_PORT_REPLACE.length(), 
                            std::to_string(reverse_port));
    }

    while (file_string.find(SCRIPT_COMMAND_PORT_REPLACE) != std::string::npos) {
        file_string.replace(file_string.find(SCRIPT_COMMAND_PORT_REPLACE), 
                            SCRIPT_COMMAND_PORT_REPLACE.length(), 
                            std::to_string(script_command_port));
    }
    
    std::ostringstream servoj_replace_str;
    servoj_replace_str<< "t = " << servoj_time << ", lookahead_time = " << servoj_lookhead_time << ", gain=" << servoj_gain;
    while (file_string.find(SERVO_J_REPLACE) != std::string::npos) {
        file_string.replace(file_string.find(SERVO_J_REPLACE), 
                            SERVO_J_REPLACE.length(), 
                            servoj_replace_str.str());
    }
    
    while (file_string.find(POS_ZOOM_RATIO_REPLACE) != std::string::npos) {
        file_string.replace(file_string.find(POS_ZOOM_RATIO_REPLACE), 
                            POS_ZOOM_RATIO_REPLACE.length(), 
                            std::to_string(CONTROL::POS_ZOOM_RATIO));
    }
    

    while (file_string.find(TIME_ZOOM_RATIO_REPLACE) != std::string::npos) {
        file_string.replace(file_string.find(TIME_ZOOM_RATIO_REPLACE), 
                            TIME_ZOOM_RATIO_REPLACE.length(), 
                            std::to_string(CONTROL::TIME_ZOOM_RATIO));
    }

    while (file_string.find(COMMON_ZOOM_RATIO_REPLACE) != std::string::npos) {
        file_string.replace(file_string.find(COMMON_ZOOM_RATIO_REPLACE), 
                            COMMON_ZOOM_RATIO_REPLACE.length(), 
                            std::to_string(CONTROL::COMMON_ZOOM_RATIO));
    }

    while (file_string.find(REVERSE_DATA_SIZE_REPLACE) != std::string::npos) {
        
        file_string.replace(file_string.find(REVERSE_DATA_SIZE_REPLACE), \
                            REVERSE_DATA_SIZE_REPLACE.length(), \
                            std::to_string(ReverseInterface::REVERSE_DATA_SIZE));
    }

    while (file_string.find(TRAJECTORY_DATA_SIZE_REPLACE) != std::string::npos) {
        file_string.replace(file_string.find(TRAJECTORY_DATA_SIZE_REPLACE), \
                            TRAJECTORY_DATA_SIZE_REPLACE.length(), \
                            std::to_string(TrajectoryInterface::TRAJECTORY_MESSAGE_LEN));
    }

    while (file_string.find(SCRIPT_COMMAND_DATA_SIZE_REPLACE) != std::string::npos) {
        file_string.replace(file_string.find(SCRIPT_COMMAND_DATA_SIZE_REPLACE), \
                            SCRIPT_COMMAND_DATA_SIZE_REPLACE.length(), \
                            std::to_string(ScriptCommandInterface::SCRIPT_COMMAND_DATA_SIZE));
    }

}

EliteDriver::EliteDriver(const std::string& robot_ip, const std::string& local_ip, const std::string& script_file,
                bool headless_mode, int script_sender_port, int reverse_port,
                int trajectory_port, int script_command_port, float servoj_time,
                float servoj_lookhead_time, int servoj_gain) {
    ELITE_LOG_DEBUG("Elite Driver initialization...");
    
    impl_ = new EliteDriver::Impl(robot_ip, local_ip, script_sender_port);
    
    // Generate external control script.
    impl_->control_script_ = impl_->readScriptFile(script_file);
    impl_->scriptParamWrite(impl_->control_script_, reverse_port, trajectory_port, script_command_port, servoj_time, servoj_lookhead_time, servoj_gain);

    impl_->reverse_server_ = std::make_unique<ReverseInterface>(reverse_port);
    ELITE_LOG_DEBUG("Created reverse interface");
    impl_->trajectory_server_ = std::make_unique<TrajectoryInterface>(trajectory_port);
    ELITE_LOG_DEBUG("Created trajectory interface");
    impl_->script_command_server_ = std::make_unique<ScriptCommandInterface>(script_command_port);
    ELITE_LOG_DEBUG("Created script command interface");

    // Instantiate primary port
    impl_->primary_port_ = std::make_unique<PrimaryPortInterface>();

    impl_->headless_mode_ = headless_mode;

    ELITE_LOG_DEBUG("Elite Driver initialized but not yet connected");
}


EliteDriver::~EliteDriver() {
    delete impl_;
}

bool EliteDriver::connect() {
    ELITE_LOG_INFO("Connecting to robot primary port...");

    if (!impl_->primary_port_->connect(impl_->robot_ip_, PrimaryPortInterface::PRIMARY_PORT)) {
        ELITE_LOG_WARN(
            "Robot primary port connection failed for %s:%d",
            impl_->robot_ip_.c_str(), PrimaryPortInterface::PRIMARY_PORT
        );
        return false;
    }

    ELITE_LOG_INFO(
        "Robot connected to primary port at %s:%d",
        impl_->robot_ip_.c_str(), PrimaryPortInterface::PRIMARY_PORT
    );
    
    if (impl_->headless_mode_) {
        impl_->robot_script_ += "def externalControl():\n";
        std::istringstream control_script_stream(impl_->control_script_);
        std::string line;
        while (std::getline(control_script_stream, line)) {
            impl_->robot_script_ += "\t" + line + "\n";
        }
        impl_->robot_script_ += "end";

        sendExternalControlScript();
    } else {
        impl_->robot_script_ = impl_->control_script_;
        impl_->script_sender_ = std::make_unique<ScriptSender>(impl_->script_sender_port_, impl_->robot_script_);
        ELITE_LOG_DEBUG("Created script sender");
    }

    return true;
}


bool EliteDriver::writeServoj(const vector6d_t& pos, int timeout_ms) {
    return impl_->reverse_server_->writeJointCommand(pos, ControlMode::MODE_SERVOJ, timeout_ms);
}

bool EliteDriver::writeSpeedl(const vector6d_t& vel, int timeout_ms) {
    return impl_->reverse_server_->writeJointCommand(vel, ControlMode::MODE_SPEEDL, timeout_ms);
}

bool EliteDriver::writeSpeedj(const vector6d_t& vel, int timeout_ms) {
    return impl_->reverse_server_->writeJointCommand(vel, ControlMode::MODE_SPEEDJ, timeout_ms);
}

void EliteDriver::setTrajectoryResultCallback(std::function<void(TrajectoryMotionResult)> cb) {
    impl_->trajectory_server_->setMotionResultCallback(cb);
}

bool EliteDriver::writeTrajectoryPoint(const vector6d_t& positions, float time, float blend_radius, bool cartesian) {
    return impl_->trajectory_server_->writeTrajectoryPoint(positions, time, blend_radius, cartesian);
}

bool EliteDriver::writeTrajectoryControlAction(TrajectoryControlAction action, const int point_number, int robot_receive_timeout) {
    return impl_->reverse_server_->writeTrajectoryControlAction(action, point_number, robot_receive_timeout);
}


bool EliteDriver::stopControl() {
    return impl_->reverse_server_->stopControl();
}

bool EliteDriver::writeIdle(int timeout_ms) {
    return impl_->reverse_server_->writeJointCommand(nullptr, ControlMode::MODE_IDLE, timeout_ms);
}

void EliteDriver::printRobotScript() {
    std::cout << impl_->robot_script_ << std::endl;
}

bool EliteDriver::isRobotConnected() {
    return impl_->reverse_server_->isRobotConnect() && impl_->trajectory_server_->isRobotConnect();
}

bool EliteDriver::zeroFTSensor() {
    return impl_->script_command_server_->zeroFTSensor();
}

bool EliteDriver::setPayload(double mass, const vector3d_t& cog) {
    return impl_->script_command_server_->setPayload(mass, cog);
}

bool EliteDriver::setToolVoltage(const ToolVoltage& vol) {
    return impl_->script_command_server_->setToolVoltage(vol);
}

bool EliteDriver::startForceMode(const vector6d_t& reference_frame, const vector6int32_t& selection_vector,
                                 const vector6d_t& wrench, const ForceMode& mode, const vector6d_t& limits) {
    return impl_->script_command_server_->startForceMode(reference_frame, selection_vector, wrench, mode, limits);
}

bool EliteDriver::endForceMode() {
    return impl_->script_command_server_->endForceMode();
}

bool EliteDriver::sendScript(const std::string& script) {
    if (!impl_->primary_port_) {
        ELITE_LOG_ERROR("Not connect to robot primary port");
        return false;
    }
    return impl_->primary_port_->sendScript(script);
}

bool EliteDriver::sendExternalControlScript() {
    if (!impl_->headless_mode_) {
        ELITE_LOG_ERROR("Not in headless mode");
        return false;
    }
    return sendScript(impl_->robot_script_);
}

bool EliteDriver::getPrimaryPackage(std::shared_ptr<PrimaryPackage> pkg, int timeout_ms) {
    if (!impl_->primary_port_) {
        ELITE_LOG_ERROR("Not connect to robot primary port");
        return false;
    }
    return impl_->primary_port_->getPackage(pkg, timeout_ms);
}

bool EliteDriver::primaryReconnect() {
    impl_->primary_port_->disconnect();
    return impl_->primary_port_->connect(impl_->robot_ip_);
}