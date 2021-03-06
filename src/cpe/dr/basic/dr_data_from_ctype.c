#include <ctype.h>
#include <limits.h>
#include <string.h>
#include "cpe/dr/dr_data.h"
#include "../dr_internal_types.h"

int dr_entry_set_from_ctype(void * output, const void * input, int input_type, LPDRMETAENTRY entry, error_monitor_t em) {
    switch(input_type) {
    case CPE_DR_TYPE_INT8:
        return dr_entry_set_from_int8(output, *(const int8_t *)input, entry, em);
    case CPE_DR_TYPE_INT16:
        return dr_entry_set_from_int16(output, *(const int16_t *)input, entry, em);
    case CPE_DR_TYPE_UINT16:
        return dr_entry_set_from_uint16(output, *(const uint16_t *)input, entry, em);
    case CPE_DR_TYPE_INT32:
        return dr_entry_set_from_int32(output, *(const int32_t *)input, entry, em);
    case CPE_DR_TYPE_UINT32:
        return dr_entry_set_from_uint32(output, *(const uint32_t *)input, entry, em);
    case CPE_DR_TYPE_INT64:
        return dr_entry_set_from_int64(output, *(const int64_t *)input, entry, em);
    case CPE_DR_TYPE_UINT64:
        return dr_entry_set_from_uint64(output, *(const uint64_t *)input, entry, em);
    case CPE_DR_TYPE_UINT8:
        return dr_entry_set_from_uint8(output, *(const uint8_t *)input, entry, em);
    case CPE_DR_TYPE_FLOAT:
        return dr_entry_set_from_float(output, *(const float *)input, entry, em);
    case CPE_DR_TYPE_DOUBLE:
        return dr_entry_set_from_double(output, *(const double *)input, entry, em);
    case CPE_DR_TYPE_STRING:
        return dr_entry_set_from_string(output, (const char *)input, entry, em);
    default:
        CPE_ERROR(em, "not support set from %s to %s!", dr_type_name(input_type), dr_type_name(entry->m_type));
        return -1;
    }
}
