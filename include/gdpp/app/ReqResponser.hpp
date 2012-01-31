#ifndef GDPP_APP_RESPONSER_H
#define GDPP_APP_RESPONSER_H
#include "gd/dp/dp_responser.h"
#include "System.hpp"

namespace Gd { namespace App {

class ReqResponser {
public:
    virtual int process(Dp::Request & req, error_monitor_t em) = 0;
    virtual ~ReqResponser();

    static int _process(gd_dp_req_t req, void * ctx, error_monitor_t em);
    static gd_dp_rsp_type_t _type;
};

}}

#define GD_DP_RESPONSER_REG(__rsp_name, __rsp_type) \
extern "C"                                          \
int rsp_ ## __rsp_name ## _init(                    \
    gd_dp_rsp_t rsp, gd_app_context_t context,      \
    gd_app_module_t module,                         \
    cfg_t cfg)                                      \
{                                                   \
    ::Gd::App::ReqResponser * processor =              \
          new __rsp_type(                           \
              *((::Gd::App::Application *)context), \
              *((::Gd::App::Module*)module),        \
              cfg);                                 \
    if (processor == NULL) {return -1;}             \
                                                    \
    gd_dp_rsp_set_opt(                              \
        rsp, gd_dp_rsp_set_processor,               \
        &::Gd::App::ReqResponser::_process);           \
                                                    \
    gd_dp_rsp_set_opt(                              \
        rsp, gd_dp_rsp_set_context,                 \
        processor);                                 \
                                                    \
    gd_dp_rsp_set_opt(                              \
        rsp, gd_dp_rsp_set_type,                    \
        ::Gd::App::ReqResponser::_type);               \
                                                    \
    return 0;                                       \
}

#endif


