#ifndef WEIBO_DATA_PERSISTENT_DIR_H
#define WEIBO_DATA_PERSISTENT_DIR_H
#include "weibo_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int weibo_manage_set_persistent_dir(weibo_manage_t mgr, const char * path, size_t max_file_len);

#ifdef __cplusplus
}
#endif

#endif
