#include "PrimaryPort.hpp"
#include "PrimaryPortInterface.hpp"

namespace ELITE
{

class PrimaryPortInterface::Impl {
public:
    PrimaryPort primary_;
};


PrimaryPortInterface::PrimaryPortInterface() {
    impl_ = std::make_unique<Impl>();
}

PrimaryPortInterface::~PrimaryPortInterface() {

}

bool PrimaryPortInterface::connect(const std::string& ip, int port) {
    return impl_->primary_.connect(ip, port);
}

void PrimaryPortInterface::disconnect() {
    impl_->primary_.disconnect();
}

bool PrimaryPortInterface::sendScript(const std::string& script) {
    return impl_->primary_.sendScript(script);
}

bool PrimaryPortInterface::getPackage(std::shared_ptr<PrimaryPackage> pkg, int timeout_ms) {
    return impl_->primary_.getPackage(pkg, timeout_ms);
}

std::string PrimaryPortInterface::getLocalIP() {
    return impl_->primary_.getLocalIP();
}

} // namespace ELITE

