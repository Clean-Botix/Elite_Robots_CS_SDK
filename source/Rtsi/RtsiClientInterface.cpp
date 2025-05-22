#include "RtsiClientInterface.hpp"
#include "RtsiClient.hpp"

using namespace ELITE;

class RtsiClientInterface::Impl {
public:
    RtsiClient client_;
};

RtsiClientInterface::RtsiClientInterface() {
    impl_ = std::make_unique<Impl>();
}

RtsiClientInterface::~RtsiClientInterface() = default;


bool RtsiClientInterface::connect(const std::string& ip, int port) {
    return impl_->client_.connect(ip, port);
}

void RtsiClientInterface::disconnect() {
    impl_->client_.disconnect();
}


bool RtsiClientInterface::negotiateProtocolVersion(uint16_t version) {
    return impl_->client_.negotiateProtocolVersion(version);
}


VersionInfo RtsiClientInterface::getControllerVersion() {
    return impl_->client_.getControllerVersion();
}


RtsiRecipeSharedPtr RtsiClientInterface::setupOutputRecipe(const std::vector<std::string>& recipe_list, double frequency) {
    return impl_->client_.setupOutputRecipe(recipe_list, frequency);
}


RtsiRecipeSharedPtr RtsiClientInterface::setupInputRecipe(const std::vector<std::string>& recipe) {
    return impl_->client_.setupInputRecipe(recipe);
}


bool RtsiClientInterface::start() {
    return impl_->client_.start();
}


bool RtsiClientInterface::pause() {
    return impl_->client_.pause();
}


void RtsiClientInterface::send(RtsiRecipeSharedPtr& recipe) {
    impl_->client_.send(recipe);
}


int RtsiClientInterface::receiveData(std::vector<RtsiRecipeSharedPtr>& recipes, bool read_newest) {
    return impl_->client_.receiveData(recipes, read_newest);
}

bool RtsiClientInterface::receiveData(RtsiRecipeSharedPtr recipe, bool read_newest) {
    return impl_->client_.receiveData(recipe, read_newest);
}

bool RtsiClientInterface::isConnected() {
    return impl_->client_.isConnected();
}


bool RtsiClientInterface::isStarted() {
    return impl_->client_.isStarted();
}


bool RtsiClientInterface::isReadAvailable() {
    return impl_->client_.isReadAvailable();
}
