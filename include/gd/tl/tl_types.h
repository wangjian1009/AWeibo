#ifndef GD_TL_TYPES_H
#define GD_TL_TYPES_H
#include "cpe/pal/types.h"

typedef struct gd_tl_event * gd_tl_event_t;
typedef struct gd_tl * gd_tl_t;
typedef struct gd_tl_manage * gd_tl_manage_t;

typedef int64_t gd_tl_time_t;
typedef int64_t gd_tl_time_span_t;


typedef gd_tl_time_t (*gd_tl_time_fun_t)(void * context);
typedef gd_tl_time_span_t (*gd_tl_time_cvt_fun_t)(gd_tl_time_span_t delta, void * context);

typedef int (*gd_tl_event_dispatcher_t)(gd_tl_event_t event, void * context);
typedef void (*gd_tl_event_destory_t)(gd_tl_event_t event, void * context);

#endif


