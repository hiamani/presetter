#include "ext_hashtab.h"
#include "utilities.h"

#include "structs.h"

t_symbol *presetter_lookup_preset_slot(t_presetter *p, long index) {
    t_symbol *name = NULL;
    hashtab_lookup(p->j_slots, presetter_long_to_sym(index), (t_object **)&name);
    return name;
}
