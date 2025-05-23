#include "TrajectoryInterface.hpp"
#include "ControlCommon.hpp"
#include "EliteException.hpp"
#include "Log.hpp"
#include <boost/asio.hpp>

using namespace ELITE;

TrajectoryInterface::TrajectoryInterface(int port) : ReversePort(port, sizeof(TrajectoryMotionResult)) {
    server_->setReceiveCallback([&](const uint8_t data[], int nb){
        if (nb != sizeof(TrajectoryMotionResult)) {
            return;
        }
        TrajectoryMotionResult motion_result = (TrajectoryMotionResult)htonl(*((const uint32_t*)data));
        if (motion_result_func_) {
            motion_result_func_(motion_result);
        }
    });
    server_->startListen();
}


TrajectoryInterface::~TrajectoryInterface() {
    
}


bool TrajectoryInterface::writeTrajectoryPoint( const vector6d_t& positions, 
                                                float time, 
                                                float blend_radius, 
                                                bool cartesian) {
    int32_t buffer[TRAJECTORY_MESSAGE_LEN] = {0};
    for (size_t i = 0; i < 6; i++) {
        buffer[i] = htonl(round(positions[i] * CONTROL::POS_ZOOM_RATIO));
    }
    buffer[18] = htonl(round(time * CONTROL::TIME_ZOOM_RATIO));
    buffer[19] = htonl(round(blend_radius * CONTROL::POS_ZOOM_RATIO));
    if (cartesian) {
        buffer[20] = htonl((int)TrajectoryMotionType::CARTESIAN);
    } else {
        buffer[20] = htonl((int)TrajectoryMotionType::JOINT);
    }

    return write(buffer, sizeof(buffer)) > 0;
}