#include "EliteException.hpp"

using namespace ELITE;


const char* EliteException::exceptionCodeToString(const Code& ec) {
    switch (ec) {
    case Code::SUCCESS:
        return "success";
    case Code::SOCKET_OPT_CANCEL:
        return "socket option cancel";
    case Code::SOCKET_CONNECT_FAIL:
        return "socket connect fail";
    case Code::SOCKET_FAIL:
        return "socket disconnected";
    case Code::RTSI_UNKNOW_VARIABLE_TYPE:
        return "rtsi unknow variable type";
    case Code::RTSI_RECIPE_PARSER_FAIL:
        return "rtsi recipe parser fail";
    case Code::ILLEGAL_PARAM:
        return "parametric is illegal";
    case Code::DASHBOARD_NOT_EXPECT_RECIVE:
        return "dashboard not expect recive";
    case Code::TCP_SERVER_CONTEXT_NULL:
        return "tcp server io_context is nullptr";
    default:
        return "unknow code";
    }
}
