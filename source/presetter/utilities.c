#include "ext_obex.h"
#include "ext_proto.h"
#include "ext_strings.h"

// Local Includes
#include "structs.h"

t_symbol *presetter_long_to_sym(long i) {
    char key[24];
    snprintf_zero(key, sizeof(key), "%ld", i);
    return gensym(key);
}

/* Redraw Utilities */

void presetter_redraw_deferred(t_presetter *p, t_symbol *s, short arc, t_atom *argv) {
    jbox_redraw((t_jbox *)p);
}

/* Find pattrstorage */

t_object *presetter_find_pattrstorage(t_presetter *p) {
    if (p->j_pattrstorage) {
        object_detach_byptr(p, p->j_pattrstorage);
        p->j_pattrstorage = NULL;
    }

    if (!p->j_pattrstorage_name || p->j_pattrstorage_name == gensym(""))
        return NULL;

    t_object *patcher = NULL;
    object_obex_lookup(p, gensym("#P"), &patcher);
    if (!patcher)
        return NULL;

    t_object *box = (t_object *)object_method(patcher, gensym("getnamedbox"), p->j_pattrstorage_name);

    if (box) {
        t_object *ps = jbox_get_object(box);
        p->j_pattrstorage = ps;
        object_attach_byptr(p, p->j_pattrstorage);
        return ps;
    }

    return NULL;
}
