#ifndef GDPP_EVT_EVENT_H
#define GDPP_EVT_EVENT_H
#include "cpe/utils/stream.h"
#include "cpepp/utils/ClassCategory.hpp"
#include "cpepp/utils/CString.hpp"
#include "cpepp/dr/Meta.hpp"
#include "cpepp/dr/Data.hpp"
#include "gd/evt/evt_read.h"
#include "System.hpp"

namespace Gd { namespace Evt {

class Event : public Cpe::Utils::SimulateObject {
public:
    operator gd_evt_t (void) const { return (gd_evt_t)this; }

    Cpe::Utils::CString const & type(void) const { return Cpe::Utils::CString::_cast(gd_evt_type(*this)); }

    Cpe::Utils::CString const & target(void) const { return Cpe::Utils::CString::_cast(gd_evt_target(*this)); }
    cpe_hash_string_t targetHs(void) const { return gd_evt_target_hs(*this); }
    void setTarget(const char * target);

    /*carry operations*/
    Cpe::Dr::Meta const * carryDataMeta(void) const { return (Cpe::Dr::Meta const *)gd_evt_carry_meta(*this); }
    size_t carryDataCapacity(void) const { return gd_evt_carry_data_capacity(*this); }
    void * carryData(void) { return gd_evt_carry_data(*this); }
    void const * carryData(void) const { return gd_evt_carry_data(*this); }

    /*data operations*/
    Cpe::Dr::Meta const & meta(void) const { return Cpe::Dr::Meta::_cast(gd_evt_meta(*this)); }

    Cpe::Dr::ConstData args(void) const { return Cpe::Dr::ConstData(gd_evt_data(*this), meta()); }
    Cpe::Dr::Data args(void) {return Cpe::Dr::Data(gd_evt_data(*this), meta()); }

    Cpe::Dr::ConstDataElement operator[] (const char * name) const { return args()[name]; }
    Cpe::Dr::DataElement operator[] (const char * name) { return args()[name]; }

    void dump(write_stream_t stream) const { gd_evt_dump(stream, *this); }

    Event * clone(mem_allocrator_t alloc = NULL) const;

    void destory(void);

    static Event & _cast(gd_evt_t evt);
    static Event & _cast(tl_event_t tl_evt);

};

}}

#endif
