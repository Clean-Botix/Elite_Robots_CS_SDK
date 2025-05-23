/**
 * @file PrimaryPackage.hpp
 * @author yanxiaojia
 * @brief About primary port package
 * @date 2024-08-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __ELITE__PRIMARY_PACKAGE_HPP__
#define __ELITE__PRIMARY_PACKAGE_HPP__

#include <Elite/EliteOptions.hpp>

#include <vector>
#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <chrono>

namespace ELITE 
{

/**
 * @brief Inherit this class to obtain the data of the primary port.
 * 
 */
class PrimaryPackage {
private:
    int type_;
    std::mutex cv_mutex_;
    std::condition_variable cv_;
    bool ready_;
public:
    PrimaryPackage() = delete;

    /**
     * @brief Construct a new Primary Package object
     * 
     * @param type The sub-package type
     */
    explicit PrimaryPackage(int type) : type_(type), ready_(false) { }
    virtual ~PrimaryPackage() = default;

    /**
     * @brief Parser sub-package. Internal use.
     * 
     * @param len The len of sub-package
     * @param iter Position of the sub-package in the entire package
     */
    ELITE_EXPORT virtual void parser(int len, const std::vector<uint8_t>::const_iterator& iter) = 0;

    /**
     * @brief Waiting for packet data update. Invoked inside the getPackage() function.
     * 
     * @param timeout_ms
     * @return true 
     * @return false 
     */
    bool waitUpdate(int timeout_ms) {
        std::unique_lock<std::mutex> lock(cv_mutex_);
        return cv_.wait_for(lock, std::chrono::milliseconds(timeout_ms), [&]{return ready_;});
    }

    /**
     * @brief Notify the data has been updated. 
     *  This function is called when the data of this package is updated by a background thread.
     * 
     */
    void notifyUpated() {
        std::unique_lock<std::mutex> lock(cv_mutex_);
        ready_ = true;
        cv_.notify_all();
    }

    /**
     * @brief Get the sub-package type
     * 
     * @return int Sub-package type
     */
    int getType() { return type_; }
};


} // namespace ELITE


#endif
