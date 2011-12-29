#include <assert.h>
#include "gd/om/om_manage.h"
#include "om_manage_internal.h"

gd_om_mgr_t
gd_om_mgr_create(
    mem_allocrator_t alloc,
    size_t page_size,
    size_t buffer_size,
    gd_om_backend_t backend,
    void * backend_ctx)
{
    gd_om_mgr_t pgm = mem_alloc(alloc, sizeof(struct gd_om_mgr));
    if (pgm == NULL) return NULL;

    pgm->m_alloc = alloc;

    return pgm;
}

void gd_om_mgr_free(gd_om_mgr_t mgr) {
    
}
