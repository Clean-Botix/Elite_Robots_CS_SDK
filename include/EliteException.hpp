/**
 * @file EliteException.hpp
 * @author yanxiaojia
 * @brief Exception of the sdk
 * @date 2024-08-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __ELITE_EXCEPTION_HPP__
#define __ELITE_EXCEPTION_HPP__

#include <Elite/EliteOptions.hpp>
#include <stdexcept>
#include <string>

namespace ELITE {

/**
 * @brief Exception of the sdk
 * 
 */
class EliteException : virtual public std::runtime_error {
public:
    /**
     * @brief The exception code
     * 
     */
    enum class Code {
        /// success
        SUCCESS,
        /// socket operation cancelled.
        SOCKET_OPT_CANCEL,
        /// connect fail
        SOCKET_CONNECT_FAIL,
        /// socket communicate error
        SOCKET_FAIL,
        /// RTSI receive unknown data type,
        /// maybe "NOT_FOUND" or "IN_USE"
        RTSI_UNKNOW_VARIABLE_TYPE,
        /// RTSI recipe parser fail
        RTSI_RECIPE_PARSER_FAIL,
        /// illegal parameter
        ILLEGAL_PARAM,
        /// dashboard did not receive the expected response
        DASHBOARD_NOT_EXPECT_RECIVE,
        /// open file fail
        FILE_OPEN_FAIL,
        /// The "s_io_context_ptr_" point is nullptr, if throw this expection, SDK had a bug
        TCP_SERVER_CONTEXT_NULL,
    };

    EliteException() = delete;

    /**
     * @brief Construct a new Elite Exception object
     * 
     * @param code The exception code
     * @param addition addition message
     */
    explicit EliteException(Code code, const std::string& addition) 
                                : std::runtime_error(std::string(exceptionCodeToString(code)) + ": " + addition), 
                                exception_code_(code)  { }

    /**
     * @brief Construct a new Elite Exception object
     * 
     * @param code The exception code
     */
    explicit EliteException(Code code) : std::runtime_error(exceptionCodeToString(code)), exception_code_(code)  { }

    virtual ~EliteException() { }

    bool operator==(Code code) const {
        return exception_code_ == code;
    }

    operator bool() const {
        return exception_code_ != Code::SUCCESS;
    }

    const char* exceptionCodeToString(const Code& ec);

private:
    std::string exceptionStringDeal(const std::string& what) {
        std::string exception_string = what;
        exceptionStringReplace(&exception_string, "\n", "\\n");
        exceptionStringReplace(&exception_string, "\r", "\\r");
        return exception_string;
    }
    void exceptionStringReplace(std::string* exception_string, const char* target, const char* replace) {
        size_t found = exception_string->find(target);
        while (found != std::string::npos) {
            exception_string->replace(found, 1, replace);
            found = exception_string->find(target, found);
        }
    }

    Code exception_code_;
};

} // namespace ELITE

#endif
