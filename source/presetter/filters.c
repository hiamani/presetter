#include "ext_atomarray.h"
#include "ext_dictionary.h"
#include "ext_hashtab.h"
#include "ext_obex.h"
#include "ext_proto.h"
#include "ext_strings.h"
#include "ext_sysmem.h"

#include "structs.h"
#include "utilities.h"

void presetter_filters_clear_deferred(t_presetter *p, t_symbol *s, short arc, t_atom *argv) {
    if (p->j_applied_filters) {
        hashtab_clear(p->j_applied_filters);
    }
    jbox_redraw((t_jbox *)p);
}

t_dictionary *presetter_lookup_filter_slot(t_presetter *p, long index) {
    t_dictionary *dict = NULL;

    dictionary_getdictionary(p->j_filters, presetter_long_to_sym(index), (t_object **)&dict);

    return dict;
}

bool presetter_find_filter_by_name(t_presetter *p, t_symbol *s, t_filter_result *result) {
    t_dictionary *dict = p->j_filters;

    long numkeys = 0;
    t_symbol **keys = NULL;

    if (dictionary_getkeys(dict, &numkeys, &keys) != MAX_ERR_NONE) {
        return false;
    }

    for (long i = 0; i < numkeys; i++) {
        t_symbol *key = keys[i];

        t_dictionary *obj = NULL;
        dictionary_getdictionary(dict, key, (t_object **)&obj);

        if (!obj) {
            continue;
        }

        t_symbol *name = NULL;
        dictionary_getsym(obj, gensym("name"), &name);

        if (!name) {
            continue;
        }

        if (name == s) {
            result->dict = obj;
            result->index = key;

            dictionary_freekeys(dict, numkeys, keys);
            return true;
        }
    }

    dictionary_freekeys(dict, numkeys, keys);
    return false;
}

static t_symbol *presetter_find_free_filter_slot(t_dictionary *d) {
    char keystr[24];

    for (long i = 0; i < dictionary_getentrycount(d) + 1; i++) {
        snprintf_zero(keystr, sizeof(keystr), "%ld", i + 1);
        t_symbol *key = gensym(keystr);
        if (!dictionary_hasentry(d, key)) {
            return key;
        }
    }

    return NULL;
}

bool presetter_add_filter_sym(t_presetter *p, t_symbol *name, long index) {
    t_filter_result result;

    if (presetter_find_filter_by_name(p, name, &result)) {
        return false;
    }

    t_symbol *key;

    if (index == 0) {
        key = presetter_find_free_filter_slot(p->j_filters);

        if (!key) {
            return false;
        }

    } else {
        key = presetter_long_to_sym(index);
    }

    t_dictionary *obj = NULL;
    dictionary_getdictionary(p->j_filters, key, (t_object **)&obj);

    if (obj) {
        return false;
    }

    obj = dictionary_new();

    if (dictionary_appenddictionary(p->j_filters, key, (t_object *)obj) != MAX_ERR_NONE) {
        object_free(obj);
        return false;
    }

    t_atomarray *arr = atomarray_new(0, NULL);

    if (dictionary_appendatomarray(obj, gensym("slots"), (t_object *)arr) != MAX_ERR_NONE) {
        object_free(arr);
        object_free(obj);
        return false;
    }

    if (dictionary_appendsym(obj, gensym("name"), name) != MAX_ERR_NONE) {
        object_free(arr);
        object_free(obj);
        return false;
    }

    return true;
}

bool presetter_rename_filter_sym(t_presetter *p, t_symbol *so, t_symbol *sn) {
    t_filter_result result;

    if (!presetter_find_filter_by_name(p, so, &result)) {
        return false;
    }

    if (dictionary_appendsym(result.dict, gensym("name"), sn) == MAX_ERR_NONE) {
        return true;
    }

    return false;
}

bool presetter_rename_filter_idx(t_presetter *p, long idx, t_symbol *sn) {
    t_dictionary *dict = presetter_lookup_filter_slot(p, idx);

    if (!dict) {
        return false;
    }

    if (dictionary_appendsym(dict, gensym("name"), sn) == MAX_ERR_NONE) {
        return true;
    }

    return false;
}

bool presetter_clear_filter_sym(t_presetter *p, t_symbol *s) {
    t_filter_result result;

    if (!presetter_find_filter_by_name(p, s, &result)) {
        return false;
    }

    t_atomarray *arr = NULL;
    dictionary_getatomarray(result.dict, gensym("slots"), (t_object **)&arr);

    if (!arr) {
        return false;
    }

    atomarray_clear(arr);
    return true;
}

bool presetter_remove_filter_sym(t_presetter *p, t_symbol *s) {
    t_filter_result result;

    if (!presetter_find_filter_by_name(p, s, &result)) {
        return false;
    }

    if (dictionary_deleteentry(p->j_filters, result.index) == MAX_ERR_NONE) {
        if (result.index == presetter_long_to_sym(p->j_selected_filter_cell)) {
            p->j_selected_filter_cell = -1;
        }

        jbox_redraw((t_jbox *)p);
        defer_low((t_object *)p, (method)presetter_filters_clear_deferred, NULL, 0, NULL);
        return true;
    }

    return false;
}

bool presetter_remove_filter_idx(t_presetter *p, long idx) {
    t_symbol *is = presetter_long_to_sym(idx);

    if (dictionary_deleteentry(p->j_filters, is) == MAX_ERR_NONE) {
        if (is == presetter_long_to_sym(p->j_selected_filter_cell)) {
            p->j_selected_filter_cell = -1;
        }

        jbox_redraw((t_jbox *)p);
        defer_low((t_object *)p, (method)presetter_filters_clear_deferred, NULL, 0, NULL);
        return true;
    }

    return false;
}

bool presetter_set_filter_slot_sym(t_presetter *p, t_symbol *s, long slot) {
    t_filter_result result;

    if (!presetter_find_filter_by_name(p, s, &result)) {
        presetter_add_filter_sym(p, s, 0);
    }

    if (!presetter_find_filter_by_name(p, s, &result)) {
        return false;
    }

    t_atom a;
    atom_setlong(&a, slot);

    t_atomarray *arr = NULL;
    dictionary_getatomarray(result.dict, gensym("slots"), (t_object **)&arr);

    if (!arr) {
        arr = atomarray_new(1, &a);
        if (!arr) {
            return false;
        }

        if (dictionary_appendatomarray(result.dict, gensym("slots"), (t_object *)arr) != MAX_ERR_NONE) {
            return false;
        }

        return true;
    }

    long size;
    t_atom *results;

    if (atomarray_getatoms(arr, &size, &results) != MAX_ERR_NONE) {
        return false;
    }

    for (long i = 0; i < size; i++) {
        t_atom *result = &results[i];
        if (atom_gettype(result) == A_LONG && atom_getlong(result) == slot) {
            return false;
        }
    }

    atomarray_appendatom((t_atomarray *)arr, &a);

    return true;
}

bool presetter_set_filter_slot_idx(t_presetter *p, long idx, long slot) {
    t_dictionary *dict = presetter_lookup_filter_slot(p, idx);

    if (!dict) {
        return false;
    }

    t_atom a;
    atom_setlong(&a, slot);

    t_atomarray *arr = NULL;
    dictionary_getatomarray(dict, gensym("slots"), (t_object **)&arr);

    if (!arr) {
        arr = atomarray_new(1, &a);
        if (!arr) {
            return false;
        }

        if (dictionary_appendatomarray(dict, gensym("slots"), (t_object *)arr) != MAX_ERR_NONE) {
            return false;
        }

        return true;
    }

    long size;
    t_atom *results;

    if (atomarray_getatoms(arr, &size, &results) != MAX_ERR_NONE) {
        return false;
    }

    for (long i = 0; i < size; i++) {
        t_atom *result = &results[i];
        if (atom_gettype(result) == A_LONG && atom_getlong(result) == slot) {
            return false;
        }
    }

    atomarray_appendatom((t_atomarray *)arr, &a);

    return true;
}

bool presetter_drop_filter_slot_sym(t_presetter *p, t_symbol *s, long slot) {
    t_filter_result result;

    if (!presetter_find_filter_by_name(p, s, &result)) {
        return false;
    }

    t_atomarray *arr = NULL;
    dictionary_getatomarray(result.dict, gensym("slots"), (t_object **)&arr);

    if (!arr) {
        return false;
    }

    long size;
    t_atom *results;

    if (MAX_ERR_NONE != atomarray_getatoms(arr, &size, &results)) {
        return false;
    }

    for (long i = 0; i < size; i++) {
        t_atom *result = &results[i];
        if (atom_gettype(result) == A_LONG && atom_getlong(result) == slot) {
            atomarray_chuckindex(arr, i);
            return true;
        }
    }

    return false;
}

bool presetter_drop_filter_slot_idx(t_presetter *p, long idx, long slot) {
    t_dictionary *dict = presetter_lookup_filter_slot(p, idx);

    if (!dict) {
        return false;
    }

    t_atomarray *arr = NULL;
    dictionary_getatomarray(dict, gensym("slots"), (t_object **)&arr);

    if (!arr) {
        return false;
    }

    long size;
    t_atom *results;

    if (MAX_ERR_NONE != atomarray_getatoms(arr, &size, &results)) {
        return false;
    }

    for (long i = 0; i < size; i++) {
        if (atom_gettype(&results[i]) == A_LONG && atom_getlong(&results[i]) == slot) {
            atomarray_chuckindex(arr, i);
            return true;
        }
    }

    return false;
}

/* Filter Hashtab Utilities */

void presetter_set_clear_filter_button_text(t_presetter *p) {
    long size = hashtab_getsize(p->j_applied_filters);
    char *fmt_text = "";

    if (size == 0) {
        p->j_clear_filters_button_text[0] = '\0';
        return;
    } else if (size == 1) {
        fmt_text = "CLEAR %ld FILTER";
    } else {
        fmt_text = "CLEAR %ld FILTERS";
    }

    snprintf_zero(p->j_clear_filters_button_text, sizeof(p->j_clear_filters_button_text), fmt_text, size);
}

bool presetter_apply_filter_sym(t_presetter *p, t_symbol *s) {
    t_filter_result result;

    if (!presetter_find_filter_by_name(p, s, &result)) {
        return false;
    }

    bool stored = hashtab_storelong(p->j_applied_filters, result.index, (t_atom_long) true) == MAX_ERR_NONE;

    if (stored) {
        presetter_set_clear_filter_button_text(p);
        return true;
    }

    return false;
}

bool presetter_apply_filter_idx(t_presetter *p, long idx) {
    t_dictionary *dict = presetter_lookup_filter_slot(p, idx);

    if (!dict)
        return false;

    bool stored =
        hashtab_storelong(p->j_applied_filters, presetter_long_to_sym(idx), (t_atom_long) true) == MAX_ERR_NONE;

    if (stored) {
        presetter_set_clear_filter_button_text(p);
        return true;
    }

    return false;
}

bool presetter_reset_filter_sym(t_presetter *p, t_symbol *s) {
    t_filter_result result;

    if (!presetter_find_filter_by_name(p, s, &result)) {
        return false;
    }

    if (hashtab_delete(p->j_applied_filters, result.index) == MAX_ERR_NONE) {
        presetter_set_clear_filter_button_text(p);
        return true;
    }

    return false;
}

bool presetter_reset_filter_idx(t_presetter *p, long idx) {
    t_dictionary *dict = presetter_lookup_filter_slot(p, idx);

    if (!dict)
        return false;

    if (!presetter_lookup_filter_slot(p, idx)) {
        return false;
    }

    if (hashtab_delete(p->j_applied_filters, presetter_long_to_sym(idx)) == MAX_ERR_NONE) {
        presetter_set_clear_filter_button_text(p);
        return true;
    }

    return false;
}

void presetter_reset_filter_all(t_presetter *p) {
    hashtab_clear(p->j_applied_filters);
    presetter_set_clear_filter_button_text(p);
}

bool presetter_filtered_cell(t_presetter *p, long cell_idx) {
    long filters_size = hashtab_getsize(p->j_applied_filters);

    if (filters_size == 0) {
        return false;
    }

    long kc;
    t_symbol **kvs = NULL;
    hashtab_getkeys(p->j_applied_filters, &kc, &kvs);

    long matches = 0;

    if (kvs == NULL) {
        return false;
    }

    for (long i = 0; i < kc; i++) {
        t_symbol *key = kvs[i];
        t_dictionary *obj = NULL;
        dictionary_getdictionary(p->j_filters, key, (t_object **)&obj);

        if (!obj)
            continue;

        t_atomarray *arr = NULL;
        dictionary_getatomarray(obj, gensym("slots"), (t_object **)&arr);

        if (!arr)
            continue;

        long ac = 0;
        t_atom *av = NULL;
        atomarray_getatoms(arr, &ac, &av);

        if (ac == 0 || av == NULL)
            continue;

        for (long j = 0; j < ac; j++) {
            if (atom_gettype(&av[j]) == A_LONG && atom_getlong(&av[j]) == cell_idx) {
                matches++;
                break;
            }
        }
    }

    sysmem_freeptr(kvs);
    return matches == filters_size;
}
