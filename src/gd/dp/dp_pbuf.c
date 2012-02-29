#include "dp_internal_ops.h"

void gd_dp_pbuf_init(gd_dp_mgr_t dm, struct gd_dp_processing_rsp_buf * buf) {
    TAILQ_INSERT_HEAD(&dm->m_processiong_rsps, buf, m_sh_other);

    buf->m_alloc = dm->m_alloc;
    buf->m_first_block.m_read_pos = 0;
    buf->m_first_block.m_write_pos = 0;

    TAILQ_INIT(&buf->m_blocks);

    TAILQ_INSERT_HEAD(&buf->m_blocks, &buf->m_first_block, m_next);
}

void gd_dp_pbuf_fini(gd_dp_mgr_t dm, struct gd_dp_processing_rsp_buf * buf) {
    struct gd_dp_processing_rsp_block * block;

    TAILQ_REMOVE(&dm->m_processiong_rsps, buf, m_sh_other);

    while((block = TAILQ_FIRST(&buf->m_blocks))) {
        TAILQ_REMOVE(&buf->m_blocks, block, m_next);
        if (block != &buf->m_first_block) {
            mem_free(buf->m_alloc, buf);
        }
    }
}

void gd_dp_pbuf_add_rsp(struct gd_dp_processing_rsp_buf * buf, gd_dp_rsp_t rsp, error_monitor_t em) {
    struct gd_dp_processing_rsp_block * putAt;
    putAt = TAILQ_LAST(&buf->m_blocks, gd_dp_processing_rsp_block_list);

    if (putAt->m_write_pos >= PROCESSING_BUF_RSP_COUNT) {
        struct gd_dp_processing_rsp_block * newBlock =
            (struct gd_dp_processing_rsp_block *)
            mem_alloc(buf->m_alloc, sizeof(struct gd_dp_processing_rsp_block));
        if (newBlock == NULL) {
            CPE_ERROR(em, "alloc new pub fail!");
            return;
        }

        newBlock->m_write_pos = 0;
        newBlock->m_read_pos = 0;
        putAt = newBlock;

        TAILQ_INSERT_TAIL(&buf->m_blocks, newBlock, m_next);
    }

    putAt->m_rsps[putAt->m_write_pos] = rsp;
    ++putAt->m_write_pos;
}

void gd_dp_pbuf_remove_rsp(gd_dp_mgr_t dm, gd_dp_rsp_t rsp) {
    struct gd_dp_processing_rsp_buf * pbuf;
    TAILQ_FOREACH(pbuf, &dm->m_processiong_rsps, m_sh_other) {
        struct gd_dp_processing_rsp_block * block;
        TAILQ_FOREACH(block, &pbuf->m_blocks, m_next) {
            size_t i;
            for(i = block->m_read_pos; block && i < block->m_write_pos; ++i) {
                if (block->m_rsps[i] == rsp) {
                    memmove(
                        block->m_rsps + i,
                        block->m_rsps + i + 1,
                        sizeof(gd_dp_rsp_t) * (block->m_write_pos - i - 1));
                    --block->m_write_pos;

                    block = NULL;
                }
            }

            if (block == NULL) break;
        }
    }
}

gd_dp_rsp_t gd_dp_pbuf_retrieve_first(struct gd_dp_processing_rsp_buf * buf) {
    struct gd_dp_processing_rsp_block * block;

    while((block = TAILQ_FIRST(&buf->m_blocks)) && block->m_read_pos >= block->m_write_pos) {
        TAILQ_REMOVE(&buf->m_blocks, block, m_next);
        if (block != &buf->m_first_block) {
            mem_free(buf->m_alloc, block);
        }
    }

    
    return block
        ? block->m_rsps[block->m_read_pos++]
        : NULL;
}
