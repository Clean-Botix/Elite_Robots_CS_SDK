#include "RtsiClient.hpp"
#include "VersionInfo.hpp"
#include "EliteException.hpp"
#include "Utils.hpp"
#include "RtsiRecipeInternal.hpp"
#include "Log.hpp"

#include <array>
#include <iostream>

using namespace ELITE;
using namespace ELITE::UTILS;

#define RTSI_HEADR_SIZE (3)

bool RtsiClient::connect(const std::string& ip, int port) {
    ELITE_LOG_INFO("Connecting to robot RTSI port...");

    try {
        // If reconnect, the buffer not clean
        recv_buffer_.clear();
        socket_ptr_.reset(new boost::asio::ip::tcp::socket(io_context_));
        resolver_ptr_.reset(new boost::asio::ip::tcp::resolver(io_context_));
        socket_ptr_->open(boost::asio::ip::tcp::v4());
        socket_ptr_->set_option(boost::asio::ip::tcp::no_delay(true));
        socket_ptr_->set_option(boost::asio::socket_base::reuse_address(true));
        socket_ptr_->set_option(boost::asio::socket_base::keep_alive(false));
#if defined(__linux) || defined(linux) || defined(__linux__)
        socket_ptr_->set_option(boost::asio::detail::socket_option::boolean<IPPROTO_TCP, TCP_QUICKACK>(true));
#endif
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip), port);
        socket_ptr_->async_connect(endpoint, [&](const boost::system::error_code& error) {
            if (!error) {
                connection_state = ConnectionState::CONNECTED;
            }
        });
        io_context_.run();
        
    } catch(const boost::system::system_error &error) {
        ELITE_LOG_WARN("Robot RTSI connection failed for %s:%d: %s", ip.c_str(), port, error.what());
        return false;
    }

    ELITE_LOG_INFO("Robot RTSI connected to %s:%d", ip.c_str(), port);
    return true;
}

void RtsiClient::disconnect() {
    if (connection_state == STARTED) {
        pause();
    }
    socketDisconnect();
}

bool RtsiClient::negotiateProtocolVersion(uint16_t version) {
    std::vector<uint8_t> payload{(uint8_t)(version >> 8), (uint8_t)version};
    sendAll(PackageType::REQUEST_PROTOCOL_VERSION, payload);
    bool is_accept = false;
    receive(PackageType::REQUEST_PROTOCOL_VERSION, [&](int len, const std::vector<uint8_t>& package){
        // According to the RTSI document, does the fourth byte of the message represent whether the version check is successful.
        is_accept = package[3];
    });
    return is_accept;
}

VersionInfo RtsiClient::getControllerVersion() {
    sendAll(PackageType::GET_ELITE_CONTROL_VERSION);
    VersionInfo version;
    receive(PackageType::GET_ELITE_CONTROL_VERSION, [&](int len, const std::vector<uint8_t>& package) {
        int offset = RTSI_HEADR_SIZE;
        EndianUtils::unpack(package, offset, version.major);
        EndianUtils::unpack(package, offset, version.minor);
        EndianUtils::unpack(package, offset, version.bugfix);
        EndianUtils::unpack(package, offset, version.build);
    });
    return version;
}

RtsiRecipeSharedPtr RtsiClient::setupOutputRecipe(const std::vector<std::string>& recipe_list, double frequency) {
    // The first eight bytes of the payload section in the output subscription message are the frequency.
    std::vector<uint8_t> payload = EndianUtils::pack(frequency);
    for (auto i : recipe_list) {
        std::copy(i.begin(), i.end(), std::back_inserter(payload));
        payload.push_back(',');
    }
    // Remove the last redundant ','.
    payload.pop_back();
    sendAll(PackageType::CONTROL_PACKAGE_SETUP_OUTPUTS, payload);

    RtsiRecipeInternal* recipe = new RtsiRecipeInternal(recipe_list);
    receive(PackageType::CONTROL_PACKAGE_SETUP_OUTPUTS, [&](int len, const std::vector<uint8_t>& package){
        recipe->parserTypePackage(len, package);
    });
    RtsiRecipeSharedPtr result(static_cast<RtsiRecipe*>(recipe));
    return result;
}

RtsiRecipeSharedPtr RtsiClient::setupInputRecipe(const std::vector<std::string>& recipe_list) {
    std::vector<uint8_t> payload;
    for (auto i : recipe_list) {
        std::copy(i.begin(), i.end(), std::back_inserter(payload));
        payload.push_back(',');
    }
    // Remove the last redundant ','.
    payload.pop_back();
    sendAll(PackageType::CONTROL_PACKAGE_SETUP_INPUTS, payload);

    RtsiRecipeInternal* recipe = new RtsiRecipeInternal(recipe_list);
    receive(PackageType::CONTROL_PACKAGE_SETUP_INPUTS, [&](int len, const std::vector<uint8_t>& package){
        recipe->parserTypePackage(len, package);
    });
    RtsiRecipeSharedPtr result(static_cast<RtsiRecipe*>(recipe));
    return result;
}

bool RtsiClient::start() {
    sendAll(PackageType::CONTROL_PACKAGE_START);
    bool is_start = false;
    receive(PackageType::CONTROL_PACKAGE_START, [&](int len, const std::vector<uint8_t>& package){
        // According to the RTSI document, does the fourth byte of the message represent whether data transmission has started successfully.
        is_start = package[3];
        if (is_start) {
            connection_state = ConnectionState::STARTED;
        }
    });
    return is_start;
}

bool RtsiClient::pause() {
    sendAll(PackageType::CONTROL_PACKAGE_PAUSE);
    bool is_pause = false;
    receive(PackageType::CONTROL_PACKAGE_PAUSE, [&](int len, const std::vector<uint8_t>& package) {
        // According to the RTSI document, does the fourth byte of the message represent whether data transmission has paused successfully.
        is_pause = package[3];
        if (is_pause) {
            connection_state = ConnectionState::STOPED;
        }
    });
    return is_pause;
}

bool RtsiClient::isConnected() {
    return connection_state != ConnectionState::DISCONNECTED;
}

bool RtsiClient::isStarted() {
    return connection_state == ConnectionState::STARTED;
}

bool RtsiClient::isReadAvailable() {
    return socket_ptr_ ? socket_ptr_->available() : false;
}

void RtsiClient::send(RtsiRecipeSharedPtr& recipe) {
    sendAll(PackageType::DATA_PACKAGE, static_cast<RtsiRecipeInternal*>(recipe.get())->packToBytes());
}

int RtsiClient::receiveData(std::vector<RtsiRecipeSharedPtr>& recipes, bool read_newest) {
    int result_id = -1;
    receive(PackageType::DATA_PACKAGE, [&](int len, const std::vector<uint8_t>& package) {
        // Referring to the RTSI document, the fourth byte of the message is the recipe ID.
        int recipe_id = package[3];
        for (size_t i = 0; i < recipes.size(); i++) {
            if (!recipes[i]) {
                break;
            }
            if (recipes[i]->getID() == recipe_id) {
                static_cast<RtsiRecipeInternal*>(recipes[i].get())->parserDataPackage(len, package);
                result_id = recipe_id;
                break;
            }
        }
    }, read_newest);
    return result_id;
}

bool RtsiClient::receiveData(RtsiRecipeSharedPtr recipe, bool read_newest) {
    bool result = false;
    receive(PackageType::DATA_PACKAGE, [&](int len, const std::vector<uint8_t>& package) {
        // Referring to the RTSI document, the fourth byte of the message is the recipe ID.
        int recipe_id = package[3];
        if (recipe->getID() == recipe_id) {
            static_cast<RtsiRecipeInternal*>(recipe.get())->parserDataPackage(len, package);
            result = true;
        }
    }, read_newest);
    
    return result;
}

void RtsiClient::sendAll(const PackageType& cmd, const std::vector<uint8_t>& payload) {
    std::vector<uint8_t> message(RTSI_HEADR_SIZE);
    uint16_t message_len = RTSI_HEADR_SIZE + payload.size();

    // Build package header
    message[0] = (uint8_t)(message_len >> 8);
    message[1] = (uint8_t)message_len;
    message[2] = static_cast<uint8_t>(cmd);
    // Push back payload 
    std::copy(payload.begin(), payload.end(), std::back_inserter(message));

    boost::system::error_code ec;
    socket_ptr_->write_some(boost::asio::buffer(message), ec);
    if (ec == boost::asio::error::operation_aborted) {
        throw EliteException(EliteException::Code::SOCKET_OPT_CANCEL, ec.message());
    } else if (ec) {
        throw EliteException(EliteException::Code::SOCKET_FAIL, ec.message());
    }
}

void RtsiClient::socketDisconnect() {
    socket_ptr_.reset();
    connection_state = DISCONNECTED;
}

int RtsiClient::receiveToBuffer(unsigned timeout_ms) {
    std::vector<uint8_t> temp_buffer(4096);
    int read_len = 0;
    socket_ptr_->async_read_some(boost::asio::buffer(temp_buffer), [&](const boost::system::error_code &ec, std::size_t nb) {
        if (ec == boost::asio::error::operation_aborted) {
            throw EliteException(EliteException::Code::SOCKET_OPT_CANCEL, ec.message());
        } else if (ec) {
            throw EliteException(EliteException::Code::SOCKET_FAIL, ec.message());
        }
        read_len = nb;
    });

    // Restart the io_context, as it may have been left in the "stopped" state
    // by a previous operation.
    if (io_context_.stopped()) {
        io_context_.restart();
    }

    // Block until the asynchronous operation has completed, or timed out. If
    // the pending asynchronous operation is a composed operation, the deadline
    // applies to the entire operation, rather than individual operations on
    // the socket.
    io_context_.run_for(std::chrono::steady_clock::duration(std::chrono::milliseconds(timeout_ms)));

    // If the asynchronous operation completed successfully then the io_context
    // would have been stopped due to running out of work. If it was not
    // stopped, then the io_context::run_for call must have timed out.
    if (!io_context_.stopped()) {
        // Disconnect to cancel the outstanding asynchronous operation.
        socketDisconnect();

        // Clear socket receive or send operation
        io_context_.reset();

        // Run the io_context again.Make it in stopped
        io_context_.run();

        return -1;
    }
    recv_buffer_.insert(recv_buffer_.end(), temp_buffer.begin(), temp_buffer.begin() + read_len);
    return read_len;
}


void RtsiClient::receive(const PackageType& target_type, 
                         std::function<void(int, const std::vector<uint8_t>&)> parser_func, 
                         bool read_newest) {
    while (receiveToBuffer() > 0 || recv_buffer_.size() > RTSI_HEADR_SIZE) {
        if (recv_buffer_.size() < RTSI_HEADR_SIZE) {
            continue;
        }
        // Parser the packets in the buffer
        do {
            // Parser head of package
            uint16_t package_len = 0;
            EndianUtils::unpack(recv_buffer_.begin(), package_len);
            if (package_len > recv_buffer_.size()) {
                break;
            }
            PackageType package_type = static_cast<PackageType>(recv_buffer_[2]);
            // If the package is target package, parser it
            if (package_type == target_type) {
                parser_func(package_len, recv_buffer_);
                recv_buffer_.erase(recv_buffer_.begin(), recv_buffer_.begin() + package_len);
                if (!read_newest) {
                    return;
                }
                // If want to parser the newest message
                if (recv_buffer_.size() >= RTSI_HEADR_SIZE) {
                    uint16_t next_package_len = 0;
                    EndianUtils::unpack(recv_buffer_.begin(), next_package_len);
                    if (next_package_len <= recv_buffer_.size() &&
                        (PackageType)recv_buffer_[2] == target_type) {
                        continue;
                    } else {
                        return;
                    }
                } else {
                    return;
                }
            } else {
                // Remove not target package bytes in buffer
                recv_buffer_.erase(recv_buffer_.begin(), recv_buffer_.begin() + package_len);
            }
        } while (recv_buffer_.size() > RTSI_HEADR_SIZE);
    }
}