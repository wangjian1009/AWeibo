#include <assert.h>
#include "cpe/pal/pal_string.h"
#include "cpe/utils/stream_buffer.h"
#include "cpe/dr/dr_ctypes_op.h"
#include "cpe/cfg/cfg_read.h"
#include "cfg_internal_types.h"
#include "cfg_internal_ops.h"

void * cfg_data(cfg_t cfg) {
    if (cfg == NULL || !cfg_is_value(cfg)) {
        return NULL;
    }

    return (void*)(cfg + 1);
}

const char * cfg_name(cfg_t cfg) {
    if (cfg == NULL || cfg->m_parent == NULL) return "";

    switch (cfg->m_parent->m_type) {
    case CPE_CFG_TYPE_STRUCT:
        return cfg_to_struct_item(cfg)->m_name;
    case CPE_CFG_TYPE_SEQUENCE:
        return cfg_name(cfg->m_parent);
    default:
        return "";
    }
}

int cfg_type(cfg_t cfg) {
    if (cfg == NULL) return CPE_DR_TYPE_UNKOWN;

    return cfg->m_type;
}

cfg_t cfg_parent(cfg_t cfg) {
    if (cfg == NULL) return NULL;

    return cfg->m_parent;
}

int cfg_type_is_value(int type) {
    return type > CPE_DR_TYPE_COMPOSITE;
}

int cfg_is_value(cfg_t cfg) {
    return cfg ? (cfg->m_type > CPE_DR_TYPE_COMPOSITE) : 0;
}

#define CPE_CFG_GEN_READ_TYPE(__type_t, __type)                  \
__type_t cfg_as_  ## __type(cfg_t at, __type_t dft) {    \
    __type_t rv;                                             \
    if (at == NULL || !cfg_is_value(at) ) return dft;            \
    return dr_ctype_try_read_ ## __type(                         \
        &rv, cfg_data(at), at->m_type, NULL) == 0                \
        ? rv                                                     \
        : dft;                                                   \
}                                                                \
int cfg_try_as_  ## __type(cfg_t at, __type_t * data) {      \
    if (at == NULL || !cfg_is_value(at) ) return -1;             \
    return dr_ctype_try_read_ ## __type(                         \
        data, cfg_data(at), at->m_type, NULL);                   \
}                                                                \
__type_t cfg_get_  ## __type(                                \
    cfg_t cfg, const char * path, __type_t dft) {            \
    __type_t rv;                                             \
    cfg_t at = cfg_find_cfg(cfg, path);                          \
    if (at == NULL || !cfg_is_value(at) ) return dft;            \
    return dr_ctype_try_read_ ## __type(                         \
        &rv, cfg_data(at), at->m_type, NULL) == 0                \
        ? rv                                                     \
        : dft;                                                   \
}

CPE_CFG_GEN_READ_TYPE(int8_t, int8);
CPE_CFG_GEN_READ_TYPE(uint8_t, uint8);
CPE_CFG_GEN_READ_TYPE(int16_t, int16);
CPE_CFG_GEN_READ_TYPE(uint16_t, uint16);
CPE_CFG_GEN_READ_TYPE(int32_t, int32);
CPE_CFG_GEN_READ_TYPE(uint32_t, uint32);
CPE_CFG_GEN_READ_TYPE(int64_t, int64);
CPE_CFG_GEN_READ_TYPE(uint64_t, uint64);
CPE_CFG_GEN_READ_TYPE(float, float);
CPE_CFG_GEN_READ_TYPE(double, double);

const char * cfg_as_string(cfg_t cfg, const char * dft) {
    return cfg && cfg->m_type == CPE_DR_TYPE_STRING
        ? (const char *)cfg_data(cfg)
        : dft;
}

const char * cfg_get_string(cfg_t cfg, const char * path, const char * dft) {
    cfg_t at = cfg_find_cfg(cfg, path);
    return cfg_as_string(at, dft);
}

static cfg_t cfg_do_find_cfg_from_struct(cfg_t cfg, const char * path, const char * end) {
    char buf[CPE_CFG_NAME_MAX_LEN + 1];

    int size = end - path;
    if (size > CPE_CFG_NAME_MAX_LEN) return NULL;

    memcpy(buf, path, size);
    buf[size] = 0;
    return cfg_struct_find_cfg(cfg, buf);
}

static cfg_t cfg_do_find_cfg_from_seq(cfg_t cfg, const char * path, const char * end) {
    char buf[20 + 1];
    uint32_t seqPos;
    int size = end - path;
    if (size > 20) return NULL;

    memcpy(buf, path, size);
    buf[size] = 0;

    if (dr_ctype_set_from_string(&seqPos, CPE_DR_TYPE_UINT32, buf, NULL) != 0) return NULL;
    return cfg_seq_at(cfg, seqPos);
}

cfg_t cfg_find_cfg(cfg_t cfg, const char * path) {
    const char * end = path + strlen(path);
    const char * nextSeqTag = strchr(path, '[');
    const char * nextNameTag = strchr(path, '.');

    if (nextSeqTag == NULL) nextSeqTag = end;
    if (nextNameTag == NULL) nextNameTag = end;

    while(cfg && path < end) {
        if (path[0] == '[') {
			const char * seqEndTag;

            if (cfg->m_type != CPE_CFG_TYPE_SEQUENCE) {
                if (cfg->m_type == CPE_CFG_TYPE_STRUCT) {
                    cfg = cfg_struct_find_cfg(cfg, "");
                    if (cfg && cfg->m_type == CPE_CFG_TYPE_SEQUENCE) {
                        continue;
                    }
                }

                return NULL;
            }

            seqEndTag = strchr(nextSeqTag, ']');
            if (seqEndTag == NULL) return NULL;

            cfg = cfg_do_find_cfg_from_seq(cfg, path + 1, seqEndTag);
            path = seqEndTag + 1;
            nextSeqTag = strchr(path, '[');
            if (nextSeqTag == NULL) nextSeqTag = end;

            if (*path == '.') {
                path += 1;
                nextNameTag = strchr(path, '.');
                if (nextNameTag == NULL) nextNameTag = end;
            }
        }
        else {
            if (cfg->m_type != CPE_CFG_TYPE_STRUCT) {
                return NULL;
            }

            if (nextSeqTag < nextNameTag) {
                cfg = cfg_do_find_cfg_from_struct(cfg, path, nextSeqTag);
                path = nextSeqTag;
                nextSeqTag = strchr(nextSeqTag, '[');
                if (nextSeqTag == NULL) nextSeqTag = end;
            }
            else if (nextNameTag < nextSeqTag) {
                cfg = cfg_do_find_cfg_from_struct(cfg, path, nextNameTag);
                path = nextNameTag + 1;
                nextNameTag = strchr(path, '.');
                if (nextNameTag == NULL) nextNameTag = end;
            }
            else {
                cfg = cfg_struct_find_cfg(cfg, path);
                path = end;
            }
        }
    }

    return cfg;
}

static cfg_t cfg_seq_it_next(cfg_it_t * it) {
    cfg_t rv;
    struct cfg_seq_block * block;

    assert(it);

    if (it->m_data.m_seq_it.m_left_count == 0 || it->m_data.m_seq_it.m_block == NULL) {
        return NULL;
    }

    block = (struct cfg_seq_block *)it->m_data.m_seq_it.m_block;
    rv = block->m_items[it->m_data.m_seq_it.m_pos_in_block];

    --it->m_data.m_seq_it.m_left_count;
    ++it->m_data.m_seq_it.m_pos_in_block;
    if (it->m_data.m_seq_it.m_pos_in_block >= CPE_CFG_SEQ_BLOCK_ITEM_COUNT) {
        it->m_data.m_seq_it.m_pos_in_block -= CPE_CFG_SEQ_BLOCK_ITEM_COUNT;
        it->m_data.m_seq_it.m_block = block->m_next;
    }

    return rv;
}

static cfg_t cfg_struct_it_next(cfg_it_t * it) {
    struct cfg_struct_item * item;
    cfg_t rv;

    assert(it);

    if (it == NULL
        || it->m_data.m_struct_it.m_curent == NULL
        || it->m_data.m_struct_it.m_curent->m_parent == NULL)
    {
        return NULL;
    }

    rv = it->m_data.m_struct_it.m_curent;

    item = RB_NEXT(
        cfg_struct_item_tree,
        &((struct cfg_struct *)it->m_data.m_struct_it.m_curent->m_parent)->m_items,
        cfg_to_struct_item(it->m_data.m_struct_it.m_curent));

    it->m_data.m_struct_it.m_curent = (item == NULL) ? NULL : &item->m_data;

    return rv;
}

void cfg_it_init(cfg_it_t * it, cfg_t cfg) {
    assert(it);

    if (cfg == NULL) {
        it->next = NULL;
        return;
    }

    switch (cfg ->m_type) {
    case CPE_CFG_TYPE_SEQUENCE: {
        struct cfg_seq * seq = (struct cfg_seq *)cfg;
        it->m_data.m_seq_it.m_block = &seq->m_block_head;
        it->m_data.m_seq_it.m_pos_in_block = 0;
        it->m_data.m_seq_it.m_left_count = seq->m_count;
        it->next = cfg_seq_it_next;
        break;
    }
    case CPE_CFG_TYPE_STRUCT: {
        struct cfg_struct_item * item;
        item = RB_MIN(cfg_struct_item_tree, &((struct cfg_struct *)cfg)->m_items);
        it->m_data.m_struct_it.m_curent = (item == NULL) ? NULL : &item->m_data;
        it->next = cfg_struct_it_next;
        break;
    }
    default:
        it->next = NULL;
        break;
    }
}

int cfg_child_count(cfg_t cfg) {
    if (cfg == NULL) return 0;

    switch (cfg->m_type) {
    case CPE_CFG_TYPE_SEQUENCE:
        return cfg_struct_count(cfg);
    case CPE_CFG_TYPE_STRUCT:
        return cfg_seq_count(cfg);
    default:
        return 0;
    }
}

cfg_t cfg_child_only(cfg_t cfg) {
    struct cfg_it it;
    cfg_t child;

    cfg_it_init(&it, cfg);
    child = cfg_it_next(&it);

    return cfg_it_next(&it) ? NULL : child;
}

int cfg_path_print(write_stream_t stream, cfg_t cfg, cfg_t to) {
    int haveParent;
    cfg_t parent;

    if (cfg->m_parent == 0 || cfg == to) {
        return -1;
    }

    assert(cfg->m_parent);
    parent = cfg->m_parent;

    haveParent = cfg_path_print(stream, parent, to) == 0 ? 1 : 0;

    if (parent->m_type == CPE_CFG_TYPE_SEQUENCE) {
        int i;
        for(i = 0; i < cfg_seq_count(parent); ++i) {
            if (cfg_seq_at(parent, i) == cfg) break;
        }

        stream_printf(stream, "[%d]", i);
    }
    else {
        if (haveParent) stream_putc(stream, '.');

        assert(cfg->m_parent->m_type == CPE_CFG_TYPE_STRUCT);
        stream_printf(stream, "%s", cfg_name(cfg));
    }

    return 0;
}

const char * cfg_path(mem_buffer_t buffer, cfg_t cfg, cfg_t to) {
    struct write_stream_buffer stream = CPE_WRITE_STREAM_BUFFER_INITIALIZER(buffer);
    mem_buffer_clear_data(buffer);
    cfg_path_print((write_stream_t)&stream, cfg, to);
    stream_putc((write_stream_t)&stream, 0);
    return (const char *)mem_buffer_make_continuous(buffer, 0);
}
