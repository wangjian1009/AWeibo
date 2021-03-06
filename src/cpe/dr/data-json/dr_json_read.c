#include <assert.h>
#include "yajl/yajl_parse.h"
#include "cpe/pal/pal_string.h"
#include "cpe/pal/pal_strings.h"
#include "cpe/dr/dr_json.h"
#include "cpe/dr/dr_error.h"
#include "cpe/dr/dr_data.h"
#include "cpe/dr/dr_metalib_manage.h"
#include "../dr_internal_types.h"
#include "../dr_ctype_ops.h"

#define JSON_PARSE_CTX_BUF_LEN CPE_DR_MACRO_LEN + 1

#define JSON_PARSE_CTX_COPY_STR_TMP(__ctx, str, len) do {   \
        size_t __len = len;                                 \
        if (__len >= JSON_PARSE_CTX_BUF_LEN) {              \
            __len = JSON_PARSE_CTX_BUF_LEN - 1;             \
        }                                                   \
        memcpy(__ctx->m_buf, str, __len);                   \
        __ctx->m_buf[__len] = 0;                            \
    } while(0)

enum dr_json_read_select_state {
    dr_json_read_select_not_use
    , dr_json_read_select_use
    , dr_json_read_select_error
};

struct dr_json_parse_stack_info {
    LPDRMETA m_meta;
    size_t m_start_pos;
    int m_capacity;
    LPDRMETAENTRY m_entry;
    int m_in_array;
    int m_array_count;
    enum dr_json_read_select_state m_select_state;

    int32_t m_select_data;
};

struct dr_json_parse_ctx {
    char * m_output_buf;
    size_t m_output_capacity;
    struct mem_buffer * m_output_alloc;
    error_monitor_t m_em;

    struct dr_json_parse_stack_info m_typeStacks[CPE_DR_MAX_LEVEL];
    int m_stackPos;

    int m_size;

    char m_buf[JSON_PARSE_CTX_BUF_LEN]; //used to store tmp data
};

static char * dr_json_parse_get_write_pos(
    struct dr_json_parse_ctx * ctx,
    struct dr_json_parse_stack_info * stackInfo,
    int start_pos,
    size_t capacity)
{
    size_t start;
    size_t total_size;

    if (start_pos < 0) return NULL;

    assert(ctx);
    assert(stackInfo);

    start = stackInfo->m_start_pos + start_pos;
    total_size = start + capacity;

    if (ctx->m_output_buf) {
        if (total_size > ctx->m_output_capacity) return NULL;

        if (total_size > ctx->m_size) ctx->m_size = total_size;

        return ctx->m_output_buf + start;
    }
    else {
        assert(ctx->m_output_alloc);

        if (mem_buffer_size(ctx->m_output_alloc) < total_size) {
            if (mem_buffer_set_size(ctx->m_output_alloc, total_size) != 0) return NULL;
        }

        assert(total_size <= mem_buffer_size(ctx->m_output_alloc));

        if (total_size > ctx->m_size) ctx->m_size = total_size;

        return mem_buffer_at(ctx->m_output_alloc, start);
    }
}

static char * dr_json_parse_get_read_pos(
    struct dr_json_parse_ctx * ctx,
    struct dr_json_parse_stack_info * stackInfo,
    int start_pos,
    size_t capacity)
{
    size_t start;
    size_t total_size;

    if (start_pos < 0) return NULL;

    assert(ctx);
    assert(stackInfo);

    start = stackInfo->m_start_pos + start_pos;
    total_size = start + capacity;

    if (total_size > ctx->m_size) return NULL;

    if (ctx->m_output_buf) {
        return ctx->m_output_buf + start;
    }
    else {
        return mem_buffer_at(ctx->m_output_alloc, start);
    }
}

static int dr_json_null(void * ctx) {
    return 1;
}

static void dr_json_parse_stack_init(
    struct dr_json_parse_stack_info * stackInfo, LPDRMETA meta, size_t start_pos, int capacity)
{
    stackInfo->m_meta = meta;
    stackInfo->m_start_pos = start_pos;
    stackInfo->m_capacity = capacity;
    stackInfo->m_entry = NULL;
    stackInfo->m_in_array = 0;
    stackInfo->m_array_count = 0;
    stackInfo->m_select_state = dr_json_read_select_not_use;
    stackInfo->m_select_data = 0;
}

static int dr_json_boolean(void * ctx, int boolean) {
    //struct dr_json_parse_ctx * c = (struct dr_json_parse_ctx *) ctx;
    return 1;
}

static int dr_json_do_parse_calc_start_pos(
    struct dr_json_parse_ctx * c, 
    struct dr_json_parse_stack_info * parseType)
{
    size_t diffPos;

    diffPos = parseType->m_entry->m_data_start_pos;

    if (parseType->m_entry->m_array_count == 1) {
        if (parseType->m_in_array) return -1;
    }
    else if (parseType->m_entry->m_array_count > 1) {
        size_t elementSize;

        if (parseType->m_array_count >= parseType->m_entry->m_array_count) {
            CPE_ERROR(
                c->m_em,
                "process %s.%s, array count overflow!",
                dr_meta_name(parseType->m_meta), c->m_buf);
            return -1;
        }

        if (parseType->m_entry->m_type <= CPE_DR_TYPE_COMPOSITE) {
            LPDRMETA refMeta = dr_entry_ref_meta(parseType->m_entry);
            if (refMeta == NULL) {
                CPE_ERROR(
                    c->m_em, "process %s.%s, ref meta not exist!",
                    dr_meta_name(parseType->m_meta), c->m_buf);
                return -1;
            }

            elementSize = dr_meta_size(refMeta);
        }
        else {
            const struct tagDRCTypeInfo * typeInfo;
            typeInfo = dr_find_ctype_info_by_type(parseType->m_entry->m_type);
            if (typeInfo == NULL) {
                CPE_ERROR(
                    c->m_em, "process %s.%s, type "FMT_DR_INT_T" is unknown!",
                    dr_meta_name(parseType->m_meta), c->m_buf,
                    parseType->m_entry->m_type);
                return -1;
            }

            if (typeInfo->m_size <= 0) {
                CPE_ERROR(
                    c->m_em, "process %s.%s, type "FMT_DR_INT_T" size is invalid!",
                    dr_meta_name(parseType->m_meta), c->m_buf,
                    parseType->m_entry->m_type);
                return -1;
            }

            elementSize = typeInfo->m_size;
        }

        diffPos += elementSize * parseType->m_array_count;
        ++parseType->m_array_count;
    }

    return (int)diffPos;
}
 
static void dr_json_do_parse_from_string(
    struct dr_json_parse_ctx * c, 
    struct dr_json_parse_stack_info * parseType,
    const char * s, size_t l)
{
    size_t elementSize;
    char * writePos;

    if (parseType->m_entry == NULL) return;

    elementSize = dr_entry_element_size(parseType->m_entry);

    writePos = dr_json_parse_get_write_pos(
        c,
        parseType,
        dr_json_do_parse_calc_start_pos(c, parseType),
        elementSize);
    if (writePos == NULL) return;

    JSON_PARSE_CTX_COPY_STR_TMP(c, s, l);
    dr_entry_set_from_string(writePos, c->m_buf, parseType->m_entry, c->m_em);
}

static int dr_json_number(void * ctx, const char * s, size_t l) {
    struct dr_json_parse_ctx * c = (struct dr_json_parse_ctx *) ctx;
    struct dr_json_parse_stack_info * parseType;

    if (c->m_stackPos < 0 || c->m_stackPos >= CPE_DR_MAX_LEVEL) {
        return 1;
    }

    parseType = &c->m_typeStacks[c->m_stackPos];

    dr_json_do_parse_from_string(c, parseType, s, l);

    return 1;
}

static int dr_json_string(void * ctx, const unsigned char * s, size_t l) {
    struct dr_json_parse_ctx * c = (struct dr_json_parse_ctx *) ctx;
    struct dr_json_parse_stack_info * parseType;

    if (c->m_stackPos < 0 || c->m_stackPos >= CPE_DR_MAX_LEVEL) {
        return 1;
    }

    parseType = &c->m_typeStacks[c->m_stackPos];

    dr_json_do_parse_from_string(c, parseType, (const char *)s, l);

    return 1;
}

static int dr_json_map_key(void * ctx, const unsigned char * stringVal, size_t stringLen) {
    struct dr_json_parse_ctx * c = (struct dr_json_parse_ctx *) ctx;
    struct dr_json_parse_stack_info * curStack;
    LPDRMETAENTRY entry = NULL;

    if (c->m_stackPos < 0 || c->m_stackPos >= CPE_DR_MAX_LEVEL) {
        return 1;
    }
    
    curStack = &c->m_typeStacks[c->m_stackPos];

    curStack->m_entry = NULL;

    if (curStack->m_meta == NULL) {
        return 1;
    }

    JSON_PARSE_CTX_COPY_STR_TMP(c, stringVal, stringLen);


    entry = dr_meta_find_entry_by_name(curStack->m_meta, c->m_buf);
    if (entry == NULL) {
        return 1;
    }

    switch(curStack->m_select_state) {
    case dr_json_read_select_error:
        return 1;
    case dr_json_read_select_not_use:
        break;
    case dr_json_read_select_use:
        if (curStack->m_select_data < entry->m_select_range_min
            || curStack->m_select_data > entry->m_select_range_max)
        {
            return 1;
        }
        break;
    default:
        CPE_ERROR(c->m_em, "process %s.%s, unknown select state %d", dr_meta_name(curStack->m_meta), c->m_buf, curStack->m_select_state);
        return 1;
    }

    curStack->m_entry = entry;
    return 1;
}

static int dr_json_start_map(void * ctx) {
    struct dr_json_parse_ctx * c = (struct dr_json_parse_ctx *) ctx;
    LPDRMETA refType = NULL;
    int nextStackPos = c->m_stackPos + 1;
    struct dr_json_parse_stack_info * curStack;
    struct dr_json_parse_stack_info * nestStackNode;

    if (nextStackPos < 0 || nextStackPos >= CPE_DR_MAX_LEVEL) {
        if (c->m_stackPos < CPE_DR_MAX_LEVEL) {
            CPE_ERROR(c->m_em, "too complex, max nest level %d reached", CPE_DR_MAX_LEVEL);
        }

        ++c->m_stackPos;
        return 1;
    }

    curStack = NULL;
    if (c->m_stackPos >= 0 && c->m_stackPos < CPE_DR_MAX_LEVEL) {
        curStack = &c->m_typeStacks[c->m_stackPos];
    }

    nestStackNode = &c->m_typeStacks[nextStackPos];

    ++c->m_stackPos;
    if (curStack == NULL  || curStack->m_entry == NULL) {
        return 1;
    }

    refType = dr_entry_ref_meta(curStack->m_entry);
    if (refType) { /*composite*/
        LPDRMETAENTRY selectEntry;
        int startPos;

        startPos = dr_json_do_parse_calc_start_pos(c, curStack);
        if (startPos < 0) return 1;

        dr_json_parse_stack_init(
            nestStackNode,
            refType,
            startPos,
            curStack->m_entry->m_unitsize);

        selectEntry = dr_entry_select_entry(curStack->m_entry);
        if (selectEntry) {
            size_t select_entry_capaity = dr_entry_element_size(selectEntry);
            const char * read_pos = dr_json_parse_get_read_pos(c, curStack, curStack->m_entry->m_select_data_start_pos, select_entry_capaity);
            if (read_pos && dr_entry_try_read_int32(&nestStackNode->m_select_data, read_pos, selectEntry, c->m_em) == 0) {
                nestStackNode->m_select_state = dr_json_read_select_use;
            }
            else {
                nestStackNode->m_select_state = dr_json_read_select_error;
            }
        }
    }

    return 1;
}

static int dr_json_end_map(void * ctx) {
    struct dr_json_parse_ctx * c = (struct dr_json_parse_ctx *) ctx;
    if (c->m_stackPos >= 0 && c->m_stackPos < CPE_DR_MAX_LEVEL) {
        /*clear current stack*/
        dr_json_parse_stack_init(&c->m_typeStacks[c->m_stackPos], NULL, 0, 0);
    }
    --c->m_stackPos;
    return 1;
}

static int dr_json_start_array(void * ctx) {
    struct dr_json_parse_ctx * c = (struct dr_json_parse_ctx *) ctx;

    if (c->m_stackPos < 0 || c->m_stackPos >= CPE_DR_MAX_LEVEL) {
        return 1;
    }

    c->m_typeStacks[c->m_stackPos].m_in_array = 1;
    return 1;
}

static int dr_json_end_array(void * ctx) {
    struct dr_json_parse_ctx * c = (struct dr_json_parse_ctx *) ctx;
    struct dr_json_parse_stack_info * curStack;
    LPDRMETAENTRY refer;

    if (c->m_stackPos < 0 || c->m_stackPos >= CPE_DR_MAX_LEVEL) {
        return 1;
    }

    curStack = &c->m_typeStacks[c->m_stackPos];

    curStack->m_in_array = 0;

    refer = dr_entry_array_refer_entry(curStack->m_entry);
    if (refer) {
        char * ref_write_pos;
        ref_write_pos = dr_json_parse_get_write_pos(c, curStack, curStack->m_entry->m_array_refer_data_start_pos, dr_entry_element_size(refer));
        if (ref_write_pos) {
            dr_entry_set_from_int32(ref_write_pos, curStack->m_array_count, refer, c->m_em);
        }
    }

    curStack->m_array_count = 0;

    return 1;
}

static yajl_callbacks g_dr_json_callbacks = {
    dr_json_null,
    dr_json_boolean,
    NULL,
    NULL,
    dr_json_number,
    dr_json_string,
    dr_json_start_map,
    dr_json_map_key,
    dr_json_end_map,
    dr_json_start_array,
    dr_json_end_array
};

static void dr_json_parse_ctx_init(
    struct dr_json_parse_ctx * ctx,
    void * result,
    size_t capacity,
    struct mem_buffer * result_buffer, 
    LPDRMETA meta,
    error_monitor_t em)
{
    bzero(ctx, sizeof(struct dr_json_parse_ctx));

    dr_json_parse_stack_init(&ctx->m_typeStacks[0], meta, 0, -1);

    ctx->m_output_buf = result;
    ctx->m_output_capacity = capacity;
    ctx->m_output_alloc = result_buffer;

    ctx->m_stackPos = -1;
    ctx->m_em = em;
}

static int dr_json_read_i(
    void * result,
    size_t capacity,
    struct mem_buffer * result_buf, 
    const char * input,
    LPDRMETA meta,
    error_monitor_t em)
{
    struct dr_json_parse_ctx ctx;
    yajl_handle hand;
    yajl_status stat;

    dr_json_parse_ctx_init(&ctx, result, capacity, result_buf, meta, em);

    hand = yajl_alloc(&g_dr_json_callbacks, NULL, (void *)&ctx);
    if (hand == NULL) {
        CPE_ERROR_EX(em, CPE_DR_ERROR_NO_MEMORY, "can`t alloc memory for json parser");
        return 0;
    }

    stat = yajl_parse(hand, (const unsigned char *)input, strlen(input));
    if (stat != yajl_status_ok) {  
        yajl_free(hand);
        return 0;
    }

    yajl_free(hand);

    return ctx.m_size;
}

int dr_json_read(
    void * result,
    size_t capacity,
    const char * input,
    LPDRMETA meta,
    error_monitor_t em)
{
    int ret = 0;
    int size = 0;

    if (em) {
        CPE_DEF_ERROR_MONITOR_ADD(logError, em, cpe_error_save_last_errno, &ret);
        size = dr_json_read_i(result, capacity, NULL, input, meta, em);
        CPE_DEF_ERROR_MONITOR_REMOVE(logError, em);
    }
    else {
        CPE_DEF_ERROR_MONITOR(logError, cpe_error_save_last_errno, &ret);
        size = dr_json_read_i(result, capacity, NULL, input, meta, &logError);
    }

    return ret == 0 ? size : ret;
}

int dr_json_read_to_buffer(
    struct mem_buffer * result, 
    const char * input,
    LPDRMETA meta,
    error_monitor_t em)
{
    int ret = 0;
    int size = 0;

    if (em) {
        CPE_DEF_ERROR_MONITOR_ADD(logError, em, cpe_error_save_last_errno, &ret);
        size = dr_json_read_i(NULL, 0, result, input, meta, em);
        CPE_DEF_ERROR_MONITOR_REMOVE(logError, em);
    }
    else {
        CPE_DEF_ERROR_MONITOR(logError, cpe_error_save_last_errno, &ret);
        size = dr_json_read_i(NULL, 0, result, input, meta, &logError);
    }

    return ret == 0 ? size : ret;
}
