#include "ext_hashtab.h"

#include "structs.h"
#include "utilities.h"

t_symbol *presetter_lookup_preset_slot(t_presetter *p, long index) {
    t_symbol *name = NULL;
    hashtab_lookup(p->j_presets, presetter_long_to_sym(index), (t_object **)&name);
    return name;
}
