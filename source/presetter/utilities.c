#include "ext_obex.h"
#include "ext_path.h"
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

short presetter_get_patcher_path(t_presetter *p) {
    t_object *patcher = NULL;
    object_obex_lookup(p, gensym("#P"), &patcher);

    if (!patcher)
        return 0;

    t_symbol *patcher_fpath = object_attr_getsym(patcher, gensym("filepath"));

    if (!patcher_fpath || patcher_fpath == gensym(""))
        return 0;

    short patch_pathid = 0;
    char patcher_fname[MAX_PATH_CHARS];
    path_frompathname(patcher_fpath->s_name, &patch_pathid, patcher_fname);

    return patch_pathid;
}

bool presetter_resolve_filter_path(t_presetter *p, const char *filename, short *out_path, char *out_name) {
    if (p->j_patcher_path == 0)
        return false;

    short path_id = 0;
    char fname[MAX_PATH_CHARS];
    path_frompathname(filename, &path_id, fname);

    if (path_id != 0) {
        *out_path = path_id;
        strncpy_zero(out_name, fname, MAX_PATH_CHARS);
    } else {
        *out_path = p->j_patcher_path;
        strncpy_zero(out_name, filename, MAX_PATH_CHARS);
    }

    return true;
}

void presetter_read_filters_dictionary(t_presetter *p) {
    if (p->j_filters_filename == NULL || p->j_filters_filename == gensym(""))
        return;

    short path_id;
    char fname[MAX_PATH_CHARS];

    if (!presetter_resolve_filter_path(p, p->j_filters_filename->s_name, &path_id, fname))
        return;

    p->j_filters = dictionary_new();

    if (dictionary_read(fname, path_id, &p->j_filters) != MAX_ERR_NONE) {
        p->j_filters = dictionary_new();
    }
}

void presetter_write_filters_dictionary(t_presetter *p) {
    if (p->j_filters_filename == NULL || p->j_filters_filename == gensym(""))
        return;

    short path_id;
    char fname[MAX_PATH_CHARS];

    if (!presetter_resolve_filter_path(p, p->j_filters_filename->s_name, &path_id, fname))
        return;

    dictionary_write(p->j_filters, fname, path_id);
}

void presetter_autowrite_filters_dictionary(t_presetter *p) {
    if (!p->j_filters_autowrite)
        return;

    presetter_write_filters_dictionary(p);
}
