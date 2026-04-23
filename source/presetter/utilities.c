#include "ext_obex.h"
#include "ext_path.h"
#include "ext_post.h"
#include "ext_proto.h"
#include "ext_strings.h"
#include "jpatcher_api.h"

// Local Includes
#include "structs.h"
#include <math.h>
#include <string.h>

/* General */

bool presetter_double_is_whole(double d) {
    const double epsilon = 1e-9;
    if (!isfinite(d))
        return false;
    return fabs(d - trunc(d)) < epsilon;
}

/* Type conversion */

t_symbol *presetter_long_to_sym(long i) {
    char key[24];
    snprintf_zero(key, sizeof(key), "%ld", i);
    return gensym(key);
}

/* Redraw Utilities */

void presetter_redraw_deferred(t_presetter *p, t_symbol *s, short arc, t_atom *argv) {
    jbox_redraw((t_jbox *)p);
}

/* pattrstorage Utilities */

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

void presetter_connect_pattrstorage_deferred(t_presetter *p, t_symbol *s, short argc, t_atom *argv) {
    t_object *patcher = NULL;
    object_obex_lookup(p, gensym("#P"), &patcher);

    if (!patcher)
        return;

    t_object *ps = presetter_find_pattrstorage(p);

    if (!ps) {
        object_error((t_object *)p, "Couldn't connect to pattrstorage %s", p->j_pattrstorage_name->s_name);
        return;
    }

    t_object *ps_box = NULL;
    object_obex_lookup(ps, gensym("#B"), &ps_box);

    if (!ps_box)
        return;

    t_object *my_box = NULL;
    object_obex_lookup(p, gensym("#B"), &my_box);

    if (!my_box)
        return;

    t_atom msg[4], rv;
    atom_setobj(msg, ps_box);
    atom_setlong(msg + 1, 0);
    atom_setobj(msg + 2, my_box);
    atom_setlong(msg + 3, 0);
    object_method_typed(patcher, gensym("connect"), 4, msg, &rv);

    object_method_typed(ps, gensym("getslotnamelist"), 0, NULL, NULL);
}

void presetter_connect_pattrstorage(t_presetter *p) {
    defer_low((t_object *)p, (method)presetter_connect_pattrstorage_deferred, NULL, 0, NULL);
}

/* Path Utilities */

short presetter_get_patcher_path(t_presetter *p) {
    t_object *patcher = NULL;
    object_obex_lookup(p, gensym("#P"), &patcher);

    if (!patcher)
        return 0;

    t_object *parent = patcher;

    while (parent) {
        t_object *next = jpatcher_get_parentpatcher(parent);
        if (!next) {
            break;
        }
        parent = next;
    }

    t_symbol *patcher_fpath = object_attr_getsym(parent, gensym("filepath"));

    if (!patcher_fpath || patcher_fpath == gensym(""))
        return 0;

    short patch_pathid = 0;
    char patcher_fname[MAX_PATH_CHARS];
    path_frompathname(patcher_fpath->s_name, &patch_pathid, patcher_fname);

    return patch_pathid;
}

/* Read / Write Filters */

bool presetter_read_filters_dictionary(t_presetter *p) {
    // Try to resolve if we have a filename attribute but no resolved path
    if ((!p->j_filters_file_path || !p->j_filters_resolved_filename || p->j_filters_resolved_filename == gensym("")) &&
        p->j_filters_filename && p->j_filters_filename != gensym("")) {

        short path_id = presetter_get_patcher_path(p);
        if (path_id != 0) {
            p->j_filters_file_path = path_id;
            p->j_filters_resolved_filename = p->j_filters_filename;
        }
    }

    if (!p->j_filters_file_path || !p->j_filters_resolved_filename || p->j_filters_resolved_filename == gensym(""))
        return false;

    object_free(p->j_filters);
    p->j_filters = dictionary_new();

    if (dictionary_read(p->j_filters_resolved_filename->s_name, p->j_filters_file_path, &p->j_filters) !=
        MAX_ERR_NONE) {
        object_free(p->j_filters);
        p->j_filters = dictionary_new();
        return false;
    }

    return true;
}

bool presetter_write_filters_dictionary(t_presetter *p) {
    if ((!p->j_filters_file_path || !p->j_filters_resolved_filename || p->j_filters_resolved_filename == gensym("")) &&
        p->j_filters_filename && p->j_filters_filename != gensym("")) {

        short path_id = presetter_get_patcher_path(p);
        if (path_id != 0) {
            p->j_filters_file_path = path_id;
            p->j_filters_resolved_filename = p->j_filters_filename;
        }
    }

    if (!p->j_filters_file_path || !p->j_filters_resolved_filename || p->j_filters_resolved_filename == gensym(""))
        return false;

    if (dictionary_write(p->j_filters, p->j_filters_resolved_filename->s_name, p->j_filters_file_path) != MAX_ERR_NONE)
        return false;

    outlet_bang(p->j_outlet2);
    return true;
}

void presetter_autowrite_filters_dictionary(t_presetter *p) {
    if (!p->j_filters_autowrite)
        return;

    presetter_write_filters_dictionary(p);
}
