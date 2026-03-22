#include "ext_atomarray.h"
#include "ext_dictionary.h"
#include "ext_hashtab.h"
#include "ext_obex.h"
#include "ext_proto.h"
#include "ext_strings.h"
#include "ext_sysmem.h"

// Local Includes
#include "bounds.h"
#include "drawing.h"
#include "filters.h"
#include "presets.h"
#include "structs.h"
#include "utilities.h"

// -----------------------------------------------------------------------------
// Attribute Methods
// -----------------------------------------------------------------------------

t_max_err presetter_set_pattrstorage(t_presetter *p, t_object *attr, long argc, t_atom *argv) {
    if (argc && argv) {
        p->j_pattrstorage_name = atom_getsym(argv);
        p->j_pattrstorage = NULL;

        t_object *ps = presetter_find_pattrstorage(p);
        if (ps) {
            p->j_pattrstorage = ps;
            object_method_typed(ps, gensym("getslotnamelist"), 0, NULL, NULL);
        } else {
            return MAX_ERR_GENERIC;
        }
    } else {
        return MAX_ERR_GENERIC;
    }

    return MAX_ERR_NONE;
}

void presetter_assist(t_presetter *x, void *b, long io, long index, char *s) {
    switch (io) {
    case 1:
        switch (index) {
        case 0:
            strncpy_zero(s, "Receive pattrstorage messages", 512);
            break;
        }
        break;
    case 2:
        strncpy_zero(s, "Bang when operation completed", 512);
        break;
    }
}

// -----------------------------------------------------------------------------
// Notify
// -----------------------------------------------------------------------------

t_max_err presetter_notify(t_presetter *p, t_symbol *s, t_symbol *msg, void *sender, void *data) {
    if (sender == p->j_pattrstorage && msg == gensym("slot_modified")) {
        object_method_typed(sender, gensym("getslotnamelist"), 0, NULL, NULL);
    }

    return jbox_notify((t_jbox *)p, s, msg, sender, data);
}

// -----------------------------------------------------------------------------
// Message Methods
// -----------------------------------------------------------------------------

void presetter_loadbang(t_presetter *p, long action) {
    if (action == 0)
        return;

    t_object *ps = presetter_find_pattrstorage(p);
    if (ps) {
        object_method_typed(ps, gensym("getslotnamelist"), 0, NULL, NULL);
    }
}

void presetter_bang(t_presetter *p) {
    long inlet = proxy_getinlet((t_object *)p);
    if (inlet != 0)
        return;

    if (p->j_pattrstorage) {
        object_method_typed(p->j_pattrstorage, gensym("getslotnamelist"), 0, NULL, NULL);
    }
}

void presetter_read(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    long inlet = proxy_getinlet((t_object *)p);
    if (inlet != 0)
        return;

    t_object *ps = presetter_find_pattrstorage(p);
    if (ps) {
        object_method_typed(ps, gensym("getslotnamelist"), 0, NULL, NULL);
    }
}

void presetter_recall(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    long inlet = proxy_getinlet((t_object *)p);
    if (inlet != 0 || argc != 1 || atom_gettype(argv) != A_LONG)
        return;

    long cell_idx = atom_getlong(argv);
    t_symbol *slot = presetter_lookup_preset_slot(p, cell_idx);

    if (slot) {
        p->j_selected_preset_cell = cell_idx;
        snprintf_zero(p->j_preset_name, sizeof(p->j_preset_name), "%s", slot->s_name);

        snprintf_zero(
            p->j_preset_idle_status_text, sizeof(p->j_preset_idle_status_text), "Selected Preset %ld", cell_idx
        );

        p->j_preset_status = PRESETTER_IDLE_STATUS;

        t_atom a;
        atom_setlong(&a, cell_idx);

        jbox_redraw((t_jbox *)p);
    }
    return;
}

void presetter_slotname(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    long inlet = proxy_getinlet((t_object *)p);
    if (inlet != 0)
        return;

    if (atom_gettype(argv) == A_LONG && atom_getlong(argv) == 0) {
        hashtab_clear(p->j_slots);
        return;
    }

    if (atom_gettype(argv) == A_SYM && atom_getsym(argv) == gensym("done")) {
        if (!presetter_lookup_preset_slot(p, p->j_selected_preset_cell)) {
            p->j_selected_preset_cell = -1;
            p->j_preset_name[0] = '\0';
            p->j_preset_status = PRESETTER_NO_STATUS;
        }
        jbox_redraw((t_jbox *)p);
        return;
    }

    if (argc < 2)
        return;

    if (atom_gettype(argv) != A_LONG)
        return;
    if (atom_gettype(argv + 1) != A_SYM)
        return;

    long index = atom_getlong(argv);
    t_symbol *name = atom_getsym(argv + 1);

    if (name == gensym("(undefined)"))
        return;

    hashtab_store(p->j_slots, presetter_long_to_sym(index), (t_object *)name);
}

/* Filters */

void presetter_addfilter(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    if (argc < 1)
        return;

    if (atom_gettype(argv) != A_SYM)
        return;

    if (presetter_add_filter_sym(p, atom_getsym(argv), 0)) {
        presetter_autowrite_filters_dictionary(p);
        jbox_redraw((t_jbox *)p);
        return;
    }
}

void presetter_renamefilter(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    if (argc < 2)
        return;

    if (atom_gettype(argv) != A_SYM || atom_gettype(argv + 1) != A_SYM)
        return;

    if (presetter_rename_filter_sym(p, atom_getsym(argv), atom_getsym(argv + 1))) {
        presetter_autowrite_filters_dictionary(p);
        jbox_redraw((t_jbox *)p);
        return;
    }
}

void presetter_clearfilter(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    if (argc < 1)
        return;

    if (atom_gettype(argv) != A_SYM)
        return;

    if (presetter_clear_filter_sym(p, atom_getsym(argv))) {
        presetter_autowrite_filters_dictionary(p);
        return;
    }
}

void presetter_removefilter(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    if (argc < 1)
        return;

    if (atom_gettype(argv) != A_SYM)
        return;

    if (presetter_remove_filter_sym(p, atom_getsym(argv))) {
        presetter_autowrite_filters_dictionary(p);
        defer_low((t_object *)p, (method)presetter_filters_clear_deferred, NULL, 0, NULL);
        return;
    }
}

void presetter_addfilterslot(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    if (argc < 2)
        return;

    if (atom_gettype(argv) != A_SYM || atom_gettype(argv + 1) != A_LONG)
        return;

    if (presetter_set_filter_slot_sym(p, atom_getsym(argv), atom_getlong(argv + 1))) {
        presetter_autowrite_filters_dictionary(p);
        defer_low((t_object *)p, (method)presetter_redraw_deferred, NULL, 0, NULL);
        return;
    }
}

void presetter_removefilterslot(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    if (argc < 2)
        return;

    if (atom_gettype(argv) != A_SYM || atom_gettype(argv + 1) != A_LONG)
        return;

    if (presetter_drop_filter_slot_sym(p, atom_getsym(argv), atom_getlong(argv + 1))) {
        presetter_autowrite_filters_dictionary(p);
        defer_low((t_object *)p, (method)presetter_redraw_deferred, NULL, 0, NULL);
        return;
    }

    jbox_redraw((t_jbox *)p);
}

void presetter_applyfilter(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    if (argc < 1)
        return;

    if (atom_gettype(argv) != A_SYM)
        return;

    presetter_apply_filter_sym(p, atom_getsym(argv));
    jbox_redraw((t_jbox *)p);
}

void presetter_resetfilter(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    if (argc < 1)
        return;

    if (atom_gettype(argv) != A_SYM)
        return;

    presetter_reset_filter_sym(p, atom_getsym(argv));
    jbox_redraw((t_jbox *)p);
}

void presetter_resetfilters(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    presetter_reset_filter_all(p);
    jbox_redraw((t_jbox *)p);
}

void presetter_readfilters(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    presetter_read_filters_dictionary(p);
    jbox_redraw((t_jbox *)p);
}

void presetter_writefilters(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    presetter_write_filters_dictionary(p);
}

// Pass through unknown messages silently
void presetter_anything(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    return;
}

// -----------------------------------------------------------------------------
// Callback Methods
// -----------------------------------------------------------------------------

void presetter_handle_preset_recall(t_presetter *p, long cell_idx) {
    t_object *ps = presetter_find_pattrstorage(p);
    if (!ps)
        return;

    t_symbol *slot = presetter_lookup_preset_slot(p, cell_idx);

    if (slot) {
        p->j_selected_preset_cell = cell_idx;

        snprintf_zero(p->j_preset_name, sizeof(p->j_preset_name), "%s", slot->s_name);

        snprintf_zero(
            p->j_preset_idle_status_text, sizeof(p->j_preset_idle_status_text), "Selected Preset %ld",
            p->j_selected_preset_cell
        );

        p->j_preset_status = PRESETTER_IDLE_STATUS;

        t_atom a;
        atom_setlong(&a, cell_idx);
        object_method_typed(ps, gensym("recall"), 1, &a, NULL);

        jbox_redraw((t_jbox *)p);
    }
}

void presetter_handle_preset_store(t_presetter *p, long cell_idx) {
    t_object *ps = presetter_find_pattrstorage(p);
    if (!ps)
        return;

    snprintf_zero(p->j_preset_idle_status_text, sizeof(p->j_preset_idle_status_text), "Stored Preset %ld", cell_idx);
    p->j_preset_status = PRESETTER_IDLE_STATUS;

    t_atom a;
    atom_setlong(&a, cell_idx);
    object_method_typed(ps, gensym("store"), 1, &a, NULL);
    object_method_typed(ps, gensym("getslotnamelist"), 0, NULL, NULL);
    outlet_bang(p->j_outlet1);

    jbox_redraw((t_jbox *)p);
}

void presetter_handle_preset_delete(t_presetter *p, long cell_idx) {
    t_object *ps = presetter_find_pattrstorage(p);
    if (!ps)
        return;

    snprintf_zero(p->j_preset_idle_status_text, sizeof(p->j_preset_idle_status_text), "Deleted Preset %ld", cell_idx);
    p->j_preset_status = PRESETTER_IDLE_STATUS;

    if (p->j_selected_preset_cell == cell_idx) {
        p->j_preset_name[0] = '\0';
        p->j_selected_preset_cell = -1;
    }

    t_atom a;
    atom_setlong(&a, cell_idx);
    object_method_typed(ps, gensym("delete"), 1, &a, NULL);
    object_method_typed(ps, gensym("getslotnamelist"), 0, NULL, NULL);
    outlet_bang(p->j_outlet1);
    jbox_redraw((t_jbox *)p);
}

void presetter_handle_preset_rename(t_presetter *p) {
    t_object *ps = presetter_find_pattrstorage(p);
    if (!ps)
        return;

    snprintf_zero(
        p->j_preset_idle_status_text, sizeof(p->j_preset_idle_status_text), "Renamed Preset %ld",
        p->j_selected_preset_cell
    );
    p->j_preset_status = PRESETTER_IDLE_STATUS;
    p->j_editing_preset_name = false;

    t_atom args[2];
    atom_setlong(&args[0], p->j_selected_preset_cell);
    atom_setsym(&args[1], gensym(p->j_preset_name));

    object_method_typed(ps, gensym("slotname"), 2, args, NULL);
    object_method_typed(ps, gensym("store"), 1, args, NULL);
    object_method_typed(ps, gensym("getslotnamelist"), 0, NULL, NULL);

    outlet_bang(p->j_outlet1);

    jbox_redraw((t_jbox *)p);
}

void presetter_handle_filter_rename(t_presetter *p) {
    if (presetter_rename_filter_idx(p, p->j_selected_filter_cell, gensym(p->j_filter_name))) {
        presetter_autowrite_filters_dictionary(p);
        snprintf_zero(
            p->j_filter_idle_status_text, sizeof(p->j_filter_idle_status_text), "Renamed Filter %ld",
            p->j_selected_filter_cell
        );
        p->j_filter_status = PRESETTER_IDLE_STATUS;
        p->j_editing_filter_name = false;
        p->j_write_filter_button_down = false;
        jbox_redraw((t_jbox *)p);
        return;
    }

    if (p->j_filter_name[0] != '\0') {
        presetter_add_filter_sym(p, gensym(p->j_filter_name), p->j_selected_filter_cell);
        presetter_autowrite_filters_dictionary(p);
        snprintf_zero(
            p->j_filter_idle_status_text, sizeof(p->j_filter_idle_status_text), "Created Filter %ld",
            p->j_selected_filter_cell
        );
        p->j_filter_status = PRESETTER_IDLE_STATUS;
        p->j_editing_filter_name = false;
        p->j_write_filter_button_down = false;
        jbox_redraw((t_jbox *)p);
        return;
    }

    p->j_editing_filter_name = false;
    p->j_write_filter_button_down = false;
    jbox_redraw((t_jbox *)p);
}

// -----------------------------------------------------------------------------
// Pointer Event Methods
// -----------------------------------------------------------------------------

void presetter_clear_confirm(t_presetter *p) {
    // Presets
    p->j_preset_status_override = PRESETTER_NO_STATUS;
    p->j_confirm_preset_cell = -1;
    p->j_confirm_preset_store = false;
    p->j_confirm_preset_delete = false;
    p->j_confirm_preset_status_text[0] = '\0';
    p->j_confirm_preset_ok_button_down = false;
    p->j_confirm_preset_cancel_button_down = false;

    // Filters
    p->j_filter_status_override = PRESETTER_NO_STATUS;
    p->j_confirm_filter_cell = -1;
    p->j_confirm_filter_delete = false;
    p->j_confirm_filter_status_text[0] = '\0';
    p->j_confirm_filter_ok_button_down = false;
    p->j_confirm_filter_cancel_button_down = false;
}

/* Grid Helpers */

void presetter_preset_grid_onclick_toggle_filter(t_presetter *p, long cell_idx) {
    long kc;
    t_symbol **kvs = NULL;

    hashtab_getkeys(p->j_applied_filters, &kc, &kvs);

    // Find if cell exists in any applied filters

    bool exists_in_any = false;

    for (long i = 0; i < kc; i++) {
        char *end;
        long idx = strtol(kvs[i]->s_name, &end, 10);
        if (*end != '\0') {
            continue;
        }

        t_dictionary *dict = presetter_lookup_filter_slot(p, idx);
        if (!dict) {
            continue;
        }

        t_atomarray *arr = NULL;
        dictionary_getatomarray(dict, gensym("slots"), (t_object **)&arr);
        if (!arr) {
            continue;
        }

        long size;
        t_atom *av;
        if (atomarray_getatoms(arr, &size, &av) != MAX_ERR_NONE)
            continue;

        for (long j = 0; j < size; j++) {
            if (atom_gettype(&av[j]) == A_LONG && atom_getlong(&av[j]) == cell_idx) {
                exists_in_any = true;
                break;
            }
        }

        if (exists_in_any)
            break;
    }

    // If the cell exists in any applied filter, drop the cell from all applied
    // filters; otherwise, add the cell to all applied filters.

    bool filter_set = false;

    for (long i = 0; i < kc; i++) {
        char *end;
        long idx = strtol(kvs[i]->s_name, &end, 10);

        if (*end != '\0')
            continue;

        if (exists_in_any) {
            if (presetter_drop_filter_slot_idx(p, idx, cell_idx)) {
                filter_set = true;
            }
        } else {
            if (presetter_set_filter_slot_idx(p, idx, cell_idx)) {
                filter_set = true;
            }
        }
    }

    if (filter_set) {
        presetter_autowrite_filters_dictionary(p);
    }

    sysmem_freeptr(kvs);
}

void presetter_preset_grid_onclick(t_presetter *p, t_rect *rect, t_pt *pt, long modifiers) {
    bool filters_applied = hashtab_getsize(p->j_applied_filters) > 0;
    long cell_idx = presetter_get_preset_cell_idx(p, rect, pt);

    if ((modifiers & eLeftButton) && (modifiers & eAltKey) && (modifiers & eShiftKey) && !filters_applied) {
        p->j_editing_preset_name = false;
        p->j_confirm_preset_delete = true;
        p->j_confirm_preset_cell = cell_idx;
        p->j_preset_status_override = PRESETTER_CONFIRM_STATUS;
        snprintf_zero(
            p->j_confirm_preset_status_text, sizeof(p->j_confirm_preset_status_text), "Delete Preset %ld?", cell_idx
        );
        jbox_redraw((t_jbox *)p);
        return;
    }

    if ((modifiers & eLeftButton) && !(modifiers & eShiftKey) && !(modifiers & eAltKey)) {
        p->j_editing_preset_name = false;
        presetter_clear_confirm(p);
        presetter_handle_preset_recall(p, cell_idx);
        jbox_redraw((t_jbox *)p);
        return;
    }

    if ((modifiers & eLeftButton) && (modifiers & eShiftKey) && !(modifiers & eAltKey)) {
        if (filters_applied) {
            presetter_preset_grid_onclick_toggle_filter(p, cell_idx);
        } else {
            p->j_editing_preset_name = false;
            p->j_confirm_preset_store = true;
            p->j_confirm_preset_cell = cell_idx;
            p->j_preset_status_override = PRESETTER_CONFIRM_STATUS;
            snprintf_zero(
                p->j_confirm_preset_status_text, sizeof(p->j_confirm_preset_status_text), "Store Preset %ld?", cell_idx
            );
        }
        jbox_redraw((t_jbox *)p);
        return;
    }
}

void presetter_filter_grid_onclick(t_presetter *p, t_rect *rect, t_pt *pt, long modifiers) {
    t_cell_pos pos = presetter_get_filter_cell_pos(p, rect, pt);
    long cell_idx = presetter_get_filter_cell_idx(p, rect, pt);
    t_dictionary *filter_slot = presetter_lookup_filter_slot(p, cell_idx);

    if ((modifiers & eLeftButton) && (modifiers & eAltKey) && (modifiers & eShiftKey) && filter_slot) {
        p->j_editing_filter_name = false;
        p->j_confirm_filter_delete = true;
        p->j_confirm_filter_cell = cell_idx;
        p->j_filter_status_override = PRESETTER_CONFIRM_STATUS;
        snprintf_zero(
            p->j_confirm_filter_status_text, sizeof(p->j_confirm_filter_status_text), "Delete Filter %ld?", cell_idx
        );
        jbox_redraw((t_jbox *)p);
        return;
    }

    presetter_clear_confirm(p);

    if (presetter_in_filter_circle_bounds(p, rect, pt, &pos)) {
        t_dictionary *dict = presetter_lookup_filter_slot(p, cell_idx);

        if (!dict) {
            return;
        }

        t_symbol *name = NULL;
        dictionary_getsym(dict, gensym("name"), &name);

        if (name) {
            long val = 0;
            hashtab_lookuplong(p->j_applied_filters, presetter_long_to_sym(cell_idx), &val);

            if (val) {
                presetter_reset_filter_sym(p, name);
            } else {
                presetter_apply_filter_sym(p, name);
            }
        }

        return;
    }

    if (p->j_selected_filter_cell != cell_idx) {
        p->j_selected_filter_cell = cell_idx;
    } else {
        p->j_selected_filter_cell = -1;
        p->j_editing_filter_name = false;
        p->j_filter_status = PRESETTER_NO_STATUS;
        snprintf_zero(p->j_filter_name, sizeof(p->j_filter_name), "%s", "\0");
        return;
    }

    t_dictionary *dict = presetter_lookup_filter_slot(p, p->j_selected_filter_cell);
    t_symbol *name = NULL;
    if (dict) {
        dictionary_getsym(dict, gensym("name"), &name);
    }

    if (name) {
        snprintf_zero(p->j_filter_name, sizeof(p->j_filter_name), "%s", name->s_name);
    } else {
        snprintf_zero(p->j_filter_name, sizeof(p->j_filter_name), "%s", "\0");
    }

    snprintf_zero(
        p->j_filter_idle_status_text, sizeof(p->j_filter_idle_status_text), "Selected Filter %ld",
        p->j_selected_filter_cell
    );

    p->j_filter_status = PRESETTER_IDLE_STATUS;
    p->j_editing_filter_name = false;
}

/* Mousedown */

void presetter_mousedown(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers) {
    t_rect rect;
    jbox_get_rect_for_view((t_object *)p, patcherview, &rect);

    if (p->j_selected_tab == gensym("presets")) {
        if (presetter_in_name_bounds(p, &rect, &pt) && p->j_selected_preset_cell != -1) {
            p->j_editing_preset_name = !p->j_editing_preset_name;
            presetter_clear_confirm(p);
            jbox_redraw((t_jbox *)p);
            return;
        }

        if (presetter_in_write_button_bounds(p, &rect, &pt) && p->j_editing_preset_name) {
            p->j_write_button_down = true;
            jbox_redraw((t_jbox *)p);
            return;
        }

        if (presetter_in_grid_bounds(p, &rect, &pt)) {
            presetter_preset_grid_onclick(p, &rect, &pt, modifiers);
            jbox_redraw((t_jbox *)p);
            return;
        }

        bool preset_confirming = (p->j_confirm_preset_store || p->j_confirm_preset_delete) &&
                                 p->j_confirm_preset_status_text[0] != '\0' && p->j_confirm_preset_cell != -1;

        if (presetter_in_confirm_preset_ok_button_bounds(p, &rect, &pt) && preset_confirming) {
            p->j_confirm_preset_ok_button_down = true;
            jbox_redraw((t_jbox *)p);
            return;
        }

        if (presetter_in_confirm_preset_cancel_button_bounds(p, &rect, &pt) && preset_confirming) {
            p->j_confirm_preset_cancel_button_down = true;
            jbox_redraw((t_jbox *)p);
            return;
        }

        if (presetter_in_left_arrow_bounds(p, &rect, &pt) && p->j_preset_pagination_number > 1) {
            p->j_pagination_left_arrow_down = true;
            presetter_clear_confirm(p);
            jbox_redraw((t_jbox *)p);
            return;
        }
    } else {
        if (presetter_in_name_bounds(p, &rect, &pt) && p->j_selected_filter_cell != -1) {
            p->j_editing_filter_name = !p->j_editing_filter_name;
            presetter_clear_confirm(p);
            jbox_redraw((t_jbox *)p);
            return;
        }

        if (presetter_in_write_button_bounds(p, &rect, &pt) && p->j_editing_filter_name) {
            p->j_write_filter_button_down = true;
            jbox_redraw((t_jbox *)p);
            return;
        }

        if (presetter_in_grid_bounds(p, &rect, &pt)) {
            presetter_filter_grid_onclick(p, &rect, &pt, modifiers);
            jbox_redraw((t_jbox *)p);
            return;
        }

        bool filter_confirming =
            p->j_confirm_filter_delete && p->j_confirm_filter_status_text[0] != '\0' && p->j_confirm_filter_cell != -1;

        if (presetter_in_confirm_filter_ok_button_bounds(p, &rect, &pt) && filter_confirming) {
            p->j_confirm_filter_ok_button_down = true;
            jbox_redraw((t_jbox *)p);
            return;
        }

        if (presetter_in_confirm_filter_cancel_button_bounds(p, &rect, &pt) && filter_confirming) {
            p->j_confirm_filter_cancel_button_down = true;
            jbox_redraw((t_jbox *)p);
            return;
        }

        if (presetter_in_left_arrow_bounds(p, &rect, &pt) && p->j_filter_pagination_number > 1) {
            p->j_pagination_left_arrow_down = true;
            presetter_clear_confirm(p);
            jbox_redraw((t_jbox *)p);
            return;
        }
    }

    if (presetter_in_right_arrow_bounds(p, &rect, &pt)) {
        p->j_pagination_right_arrow_down = true;
        presetter_clear_confirm(p);
        jbox_redraw((t_jbox *)p);
        return;
    }

    if (presetter_in_presets_tab_bounds(p, &rect, &pt)) {
        p->j_selected_tab = gensym("presets");
        p->j_editing_filter_name = false;
        p->j_editing_preset_name = false;
        presetter_clear_confirm(p);
        jbox_redraw((t_jbox *)p);
        return;
    }

    if (presetter_in_filters_tab_bounds(p, &rect, &pt)) {
        p->j_selected_tab = gensym("filters");
        p->j_editing_filter_name = false;
        p->j_editing_preset_name = false;
        presetter_clear_confirm(p);
        jbox_redraw((t_jbox *)p);
        return;
    }

    if (presetter_in_clear_filters_button_bounds(p, &rect, &pt) && hashtab_getsize(p->j_applied_filters) > 0) {
        p->j_clear_filters_button_down = true;
        jbox_redraw((t_jbox *)p);
    }
}

/* Mouseup */

void presetter_mouseup(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers) {
    if (p->j_write_button_down) {
        presetter_handle_preset_rename(p);
        p->j_write_button_down = false;
        jbox_redraw((t_jbox *)p);
        return;
    }

    if (p->j_write_filter_button_down) {
        presetter_handle_filter_rename(p);
        return;
    }

    if (p->j_confirm_preset_ok_button_down) {
        if (p->j_confirm_preset_store && p->j_confirm_preset_cell != -1) {
            presetter_handle_preset_store(p, p->j_confirm_preset_cell);
        } else if (p->j_confirm_preset_delete && p->j_confirm_preset_cell != -1) {
            presetter_handle_preset_delete(p, p->j_confirm_preset_cell);
        }
        presetter_clear_confirm(p);
        jbox_redraw((t_jbox *)p);
        return;
    }

    if (p->j_confirm_preset_cancel_button_down) {
        presetter_clear_confirm(p);
        jbox_redraw((t_jbox *)p);
        return;
    }

    if (p->j_confirm_filter_ok_button_down) {
        if (p->j_confirm_filter_delete) {
            presetter_remove_filter_idx(p, p->j_confirm_filter_cell);
            p->j_filter_name[0] = '\0';
            presetter_autowrite_filters_dictionary(p);
        }
        presetter_clear_confirm(p);
        jbox_redraw((t_jbox *)p);
        return;
    }

    if (p->j_confirm_filter_cancel_button_down) {
        presetter_clear_confirm(p);
        jbox_redraw((t_jbox *)p);
        return;
    }

    if (p->j_pagination_right_arrow_down) {
        p->j_pagination_right_arrow_down = false;
        if (p->j_selected_tab == gensym("presets")) {
            p->j_preset_pagination_number = p->j_preset_pagination_number + 1;
        } else {
            p->j_filter_pagination_number = p->j_filter_pagination_number + 1;
            p->j_selected_filter_cell = -1;
            snprintf_zero(p->j_filter_name, sizeof(p->j_filter_name), "\0");
        }
        jbox_redraw((t_jbox *)p);
        return;
    }

    if (p->j_pagination_left_arrow_down) {
        p->j_pagination_left_arrow_down = false;
        if (p->j_selected_tab == gensym("presets")) {
            p->j_preset_pagination_number = p->j_preset_pagination_number - 1;
        } else {
            p->j_filter_pagination_number = p->j_filter_pagination_number - 1;
            p->j_selected_filter_cell = -1;
        }
        jbox_redraw((t_jbox *)p);
        return;
    }

    if (p->j_clear_filters_button_down) {
        p->j_clear_filters_button_down = false;
        presetter_reset_filter_all(p);
        jbox_redraw((t_jbox *)p);
        return;
    }
}

/* Mousemove */

void presetter_mousemove(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers) {
    t_rect rect;
    jbox_get_rect_for_view((t_object *)p, patcherview, &rect);

    if (p->j_selected_tab == gensym("presets")) {
        if (presetter_in_grid_bounds(p, &rect, &pt)) {
            t_grid_dim dim = presetter_get_preset_grid_dim(p, &rect);
            t_cell_pos pos = presetter_get_preset_cell_pos(p, &rect, &pt);

            p->j_hovered_preset_cell = (pos.x + dim.columns * pos.y + 1) + presetter_get_preset_grid_offset(p, &dim);

            t_symbol *name = presetter_lookup_preset_slot(p, p->j_hovered_preset_cell);

            if (name && name != gensym("<(unnamed)>")) {
                snprintf_zero(
                    p->j_preset_hover_status_text, sizeof(p->j_preset_hover_status_text), "Preset %ld: %s",
                    p->j_hovered_preset_cell, name->s_name
                );
            } else {
                snprintf_zero(
                    p->j_preset_hover_status_text, sizeof(p->j_preset_hover_status_text), "Preset %ld",
                    p->j_hovered_preset_cell
                );
            }

            if (p->j_preset_status == PRESETTER_HOVER_STATUS ||
                p->j_hovered_preset_cell != p->j_last_hovered_preset_cell) {
                p->j_preset_status = PRESETTER_HOVER_STATUS;
            }

            p->j_last_hovered_preset_cell = p->j_hovered_preset_cell;
        } else {
            p->j_preset_status = PRESETTER_IDLE_STATUS;
            p->j_hovered_preset_cell = -1;
            p->j_last_hovered_preset_cell = -2;
        }
    } else {
        if (presetter_in_grid_bounds(p, &rect, &pt)) {
            long cell_idx = presetter_get_filter_cell_idx(p, &rect, &pt);
            p->j_hovered_filter_cell = cell_idx;
        } else {
            p->j_hovered_filter_cell = -1;
            p->j_last_hovered_filter_cell = -2;
        }
    }

    jbox_redraw((t_jbox *)p);
}

/* Mouseleave */

void presetter_mouseleave(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers) {
    p->j_hovered_preset_cell = -1;
    p->j_last_hovered_preset_cell = -2;
    p->j_hovered_filter_cell = -1;
    p->j_last_hovered_filter_cell = -2;
    jbox_redraw((t_jbox *)p);
}

//------------------------------------------------------------------------------
// Keyboard Methods
//------------------------------------------------------------------------------

long presetter_key(t_presetter *p, t_object *patcherview, long keycode, long modifiers, long textcharacter) {
    if (p->j_editing_preset_name && p->j_selected_tab == gensym("presets")) {
        size_t len = strlen(p->j_preset_name);

        // Backspace
        if (keycode == -7 && len > 0) {
            p->j_preset_name[len - 1] = '\0';
            jbox_redraw((t_jbox *)p);
            return 1;
        }

        // Enter / Return
        if (keycode == -4) {
            presetter_handle_preset_rename(p);
            jbox_redraw((t_jbox *)p);
            return 1;
        }

        // Escape
        if (keycode == -3) {
            p->j_editing_preset_name = false;
            t_symbol *slot = presetter_lookup_preset_slot(p, p->j_selected_preset_cell);
            if (slot) {
                snprintf_zero(p->j_preset_name, sizeof(p->j_preset_name), "%s", slot->s_name);
            }
            jbox_redraw((t_jbox *)p);
            return 1;
        }

        // Printable characters
        if (textcharacter >= 32 && textcharacter < 127 && len < sizeof(p->j_preset_name) - 1) {
            p->j_preset_name[len] = (char)textcharacter;
            p->j_preset_name[len + 1] = '\0';
            jbox_redraw((t_jbox *)p);
            return 1;
        }

        return 0;
    }

    if (p->j_editing_filter_name && p->j_selected_tab == gensym("filters")) {
        size_t len = strlen(p->j_filter_name);

        // Backspace
        if (keycode == -7 && len > 0) {
            p->j_filter_name[len - 1] = '\0';
            jbox_redraw((t_jbox *)p);
            return 1;
        }

        // Enter / Return
        if (keycode == -4) {
            presetter_handle_filter_rename(p);
            return 1;
        }

        // Escape
        if (keycode == -3) {
            p->j_editing_filter_name = false;
            t_symbol *name = NULL;
            t_dictionary *dict = presetter_lookup_filter_slot(p, p->j_selected_filter_cell);
            if (dict && dictionary_getsym(dict, gensym("name"), &name) == MAX_ERR_NONE && name) {
                snprintf_zero(p->j_filter_name, sizeof(p->j_filter_name), "%s", name->s_name);
            }
            jbox_redraw((t_jbox *)p);
            return 1;
        }

        // Printable characters
        if (textcharacter >= 32 && textcharacter < 127 && len < sizeof(p->j_filter_name) - 1) {
            p->j_filter_name[len] = (char)textcharacter;
            p->j_filter_name[len + 1] = '\0';
            jbox_redraw((t_jbox *)p);
            return 1;
        }

        return 0;
    }

    return 0;
}

//------------------------------------------------------------------------------
// Paint Method
//------------------------------------------------------------------------------

void presetter_paint(t_presetter *p, t_object *patcherview) {
    t_jgraphics *g = (t_jgraphics *)patcherview_get_jgraphics(patcherview);

    t_rect rect;
    jbox_get_rect_for_view((t_object *)p, patcherview, &rect);

    presetter_draw_background(p, g, &rect);

    presetter_draw_write_name(p, g, &rect);

    if (p->j_selected_tab == gensym("presets")) {
        presetter_draw_write_button(p, g, &rect);
        presetter_draw_preset_grid(p, g, &rect);
        presetter_draw_confirm_preset_ok_button(p, g, &rect);
        presetter_draw_confirm_preset_cancel_button(p, g, &rect);
        presetter_draw_preset_status(p, g, &rect);
    } else {
        presetter_draw_write_filter_button(p, g, &rect);
        presetter_draw_filter_grid(p, g, &rect);
        presetter_draw_confirm_filter_ok_button(p, g, &rect);
        presetter_draw_confirm_filter_cancel_button(p, g, &rect);
        presetter_draw_filter_status(p, g, &rect);
    }

    presetter_draw_clear_filters_button(p, g, &rect);
    presetter_draw_right_arrow(p, g, &rect);
    presetter_draw_pagination_number(p, g, &rect);
    presetter_draw_left_arrow(p, g, &rect);

    presetter_draw_tab_bar(p, g, &rect);
    presetter_draw_presets_tab(p, g, &rect);
    presetter_draw_filters_tab(p, g, &rect);
}
