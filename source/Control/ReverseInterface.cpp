#include "ReverseInterface.hpp"
#include "ControlCommon.hpp"
#include "EliteException.hpp"
#include "Log.hpp"

using namespace ELITE;

ReverseInterface::ReverseInterface(int port) : ReversePort(port, 4) {
    server_->startListen();
}

ReverseInterface::~ReverseInterface() { }

bool ReverseInterface::writeJointCommand(const vector6d_t& pos, ControlMode mode, int timeout) {
    return writeJointCommand(&pos, mode, timeout);
}

bool ReverseInterface::writeJointCommand(const vector6d_t* pos, ControlMode mode, int timeout) {
    int32_t data[REVERSE_DATA_SIZE] = {0};
    data[0] = htonl(timeout);
    data[REVERSE_DATA_SIZE - 1] = htonl((int)mode);
    if (pos) {
        for (size_t i = 0; i < 6; i++) {
            data[i + 1] = htonl(static_cast<int>(round((*pos)[i] * CONTROL::POS_ZOOM_RATIO)));
        }
    }
    
    return write(data, sizeof(data)) > 0;
}

bool ReverseInterface::writeTrajectoryControlAction(TrajectoryControlAction action, int point_number, int timeout) {
    int32_t data[REVERSE_DATA_SIZE] = {0};
    data[0] = htonl(timeout);
    data[1] = htonl((int)action);
    data[2] = htonl(point_number);
    data[REVERSE_DATA_SIZE - 1] = htonl((int)ControlMode::MODE_TRAJECTORY);
    return write(data, sizeof(data)) > 0;
}

bool ReverseInterface::writeFreedrive(FreedriveAction action, int timeout_ms) {
    int32_t data[REVERSE_DATA_SIZE] = {0};
    data[0] = htonl(timeout_ms);
    data[1] = htonl((int)action);
    data[REVERSE_DATA_SIZE - 1] = htonl((int)ControlMode::MODE_FREEDRIVE);
    return write(data, sizeof(data)) > 0;
}

bool ReverseInterface::stopControl() {
    int32_t data[REVERSE_DATA_SIZE];
    data[0] = 0;
    data[REVERSE_DATA_SIZE - 1] = htonl((int)ControlMode::MODE_STOPPED);
    
    return write(data, sizeof(data)) > 0;
}
