#include "ext.h"
#include "ext_atomarray.h"
#include "ext_dictionary.h"
#include "ext_hashtab.h"
#include "ext_mess.h"
#include "ext_obex.h"
#include "ext_proto.h"
#include "jgraphics.h"
#include "jpatcher_api.h"
#include "max_types.h"
#include <math.h>

// -----------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------

/* Slot */

#define SLOT_NAME_LEN 1024

/* Colors */

// Global
#define PATCHER_OBJECT_COLOR_HEX "#9EBEF3"
#define BG_COLOR_HEX "#1E1E1E"

// Preset Name
#define PRESET_NAME_EDITING_BG_COLOR_SYM gensym("live_arranger_grid_tiles")
#define PRESET_NAME_TEXT_UNSELECTED_COLOR_HEX "#555555"
#define PRESET_NAME_TEXT_EDITING_COLOR_SYM gensym("live_contrast_frame")
#define PRESET_NAME_TEXT_DEFAULT_COLOR_SYM gensym("live_arranger_grid_tiles")

// Write Button
#define WRITE_BUTTON_INACTIVE_BG_COLOR_HEX BG_COLOR_HEX
#define WRITE_BUTTON_INACTIVE_TEXT_COLOR_HEX "#444444"
#define WRITE_BUTTON_UP_BG_COLOR_SYM gensym("live_contrast_frame")
#define WRITE_BUTTON_UP_TEXT_COLOR_SYM gensym("live_arranger_grid_tiles")
#define WRITE_BUTTON_ON_BG_COLOR_SYM gensym("live_arranger_grid_tiles")
#define WRITE_BUTTON_ON_TEXT_COLOR_SYM gensym("live_contrast_frame")

// Grid
#define GRID_SELECTED_CELL_COLOR_SYM gensym("live_display_handle_two")
#define GRID_HOVERED_CELL_COLOR_HEX "#999999"
#define GRID_STORED_CELL_COLOR_HEX "#666666"
#define GRID_DEFAULT_CELL_COLOR_HEX "#333333"

// Status
#define STATUS_TEXT_COLOR_HEX "#999999"
#define STATUS_CONFIRM_TEXT_COLOR_SYM gensym("live_display_handle_two")

// Refresh
// #define REFRESH_ON_COLOR_HEX PATCHER_OBJECT_COLOR_HEX
// #define REFRESH_OFF_COLOR_HEX "#333333"

// Confirm
#define CONFIRM_BUTTON_UP_BG_COLOR_SYM gensym("live_contrast_frame")
#define CONFIRM_BUTTON_UP_TEXT_COLOR_SYM gensym("live_display_handle_two")
#define CONFIRM_BUTTON_ON_BG_COLOR_SYM gensym("live_display_handle_two")
#define CONFIRM_BUTTON_ON_TEXT_COLOR_SYM gensym("live_contrast_frame")

// Pagination
#define PAGINATION_ARROW_OFF_COLOR_HEX "#444444"
#define PAGINATION_ARROW_ON_COLOR_HEX "#666666"
#define PAGINATION_ARROW_DOWN_COLOR_HEX PATCHER_OBJECT_COLOR_HEX

// Tabs
#define TAB_PRESETS_UP_BG_COLOR_HEX "#181818"
#define TAB_PRESETS_UP_TEXT_COLOR_HEX "#555555"
#define TAB_PRESETS_ON_BG_COLOR_SYM gensym("live_display_handle_two")
#define TAB_PRESETS_ON_TEXT_COLOR_SYM gensym("live_contrast_frame")

#define TAB_FILTERS_UP_BG_COLOR_HEX "#181818"
#define TAB_FILTERS_UP_TEXT_COLOR_HEX "#555555"
#define TAB_FILTERS_ON_BG_COLOR_HEX PATCHER_OBJECT_COLOR_HEX
#define TAB_FILTERS_ON_TEXT_COLOR_SYM gensym("live_contrast_frame")

#define TAB_BAR_COLOR_HEX "#181818"

/* Dimensions & Text */

// Grid
#define GRID_OFFSET_X 5
#define GRID_OFFSET_Y 3
#define CELL_SIZE 7.0
#define CELL_PADDING 3.5
#define CELL_TOTAL_SIZE (CELL_SIZE + CELL_PADDING)

// Write Name
#define WRITE_NAME_OFFSET_X (GRID_OFFSET_X + CELL_PADDING)
#define WRITE_NAME_OFFSET_Y 9
#define WRITE_NAME_MARGIN_RIGHT 4
#define WRITE_NAME_FONT_SIZE 10

// Buttons
#define BUTTON_FONT_SIZE 9
#define BUTTON_PADDING_X 4
#define BUTTON_PADDING_Y 1

// Write Button
#define WRITE_BUTTON_TEXT "WRITE NAME"
#define WRITE_FILTER_BUTTON_TEXT "WRITE FILTER"
#define WRITE_BUTTON_OFFSET_X (GRID_OFFSET_X + CELL_PADDING)
#define WRITE_BUTTON_OFFSET_Y 8

// Status
#define STATUS_FONT_SIZE 9
#define STATUS_OFFSET_X 8
#define STATUS_OFFSET_Y 4
#define STATUS_PADDING_RIGHT 2

// Confirm
#define CONFIRM_OK_BUTTON_TEXT "OK"
#define CONFIRM_OK_BUTTON_MARGIN_LEFT 12
#define CONFIRM_CANCEL_BUTTON_TEXT "CANCEL"
#define CONFIRM_CANCEL_BUTTON_MARGIN_LEFT 8
#define CONFIRM_BUTTON_OFFSET_Y 5

// Pagination
#define PAGINATION_MARGIN_TOP 0
#define PAGINATION_MARGIN_RIGHT 9
#define PAGINATION_NUMBER_FONT "Arial"
#define PAGINATION_NUMBER_FONT_SIZE 10
#define PAGINATION_LEFT_ARROW_TEXT "‹"
#define PAGINATION_RIGHT_ARROW_TEXT "›"
#define PAGINATION_ARROW_FONT "Menlo"
#define PAGINATION_ARROW_FONT_SIZE 13
#define PAGINATION_ARROW_PADDING 2

// Tabs
#define TAB_MARGIN 5;
#define TAB_FONT_SIZE 9
#define TAB_PADDING_X 5
#define TAB_PADDING_Y 2.5
#define TAB_PRESETS_TEXT "PRESETS"
#define TAB_FILTERS_TEXT "FILTERS"

/** Filters **/

#define FILTER_CELL_MIN_WIDTH 120
#define FILTER_CELL_HEIGHT 19
#define FILTER_GRID_PADDING 5

// -----------------------------------------------------------------------------
// Structs
// -----------------------------------------------------------------------------

typedef struct _slot {
    char name[SLOT_NAME_LEN];
} t_slot;

typedef struct _grid_dim {
    int columns;
    int rows;
} t_grid_dim;

typedef struct _grid_bounds {
    double x;
    double y;
    double width;
    double height;
} t_bounds;

typedef struct _cell_pos {
    int x;
    int y;
} t_cell_pos;

typedef enum {
    PRESETTER_NO_STATUS,
    PRESETTER_IDLE_STATUS,
    PRESETTER_HOVER_STATUS,
    PRESETTER_CONFIRM_STATUS
} t_status;

typedef struct _t_filter_result {
    t_dictionary *dict;
    t_symbol *index;
} t_filter_result;

typedef struct _presetter {
    // Base
    t_jbox j_box;

    // Inlets
    long j_inlet_num;

    // Outlets
    void *j_outlet1;

    // Patcher
    short j_patcher_path;

    // Attributes
    t_symbol *j_pattrstorage_name;

    // pattrstorage
    t_object *j_pattrstorage;

    // Slots
    t_hashtab *j_slots;

    // jgraphics
    t_jsurface *surface;
    t_jgraphics *offscreen;

    // Cells
    long j_hovered_preset_cell;
    long j_last_hovered_preset_cell;
    long j_selected_preset_cell;

    // Preset Name
    bool j_editing_preset_name;
    char j_preset_name[512];

    // Write Button
    char *j_write_button_text;
    bool j_write_button_down;

    // Status line
    t_status j_preset_status;
    t_status j_preset_status_override;
    char j_preset_idle_status_text[512];
    char j_preset_hover_status_text[1024];
    char j_preset_confirm_status_text[512];

    double j_status_height;

    // Confirm
    long j_confirm_cell;
    bool j_confirm_store;
    bool j_confirm_delete;
    bool j_confirm_ok_button_down;
    bool j_confirm_cancel_button_down;

    // Pagination
    long j_preset_pagination_number;
    bool j_pagination_left_arrow_down;
    bool j_pagination_right_arrow_down;

    // Tabs
    t_symbol *j_selected_tab;

    /** Filters **/

    t_dictionary *j_filters;
    t_hashtab *j_applied_filters;

    // Filter Name
    bool j_editing_filter_name;
    char j_filter_name[512];

    // Write Button
    char *j_write_filter_button_text;
    bool j_write_filter_button_down;

    // Cells
    long j_hovered_filter_cell;
    long j_last_hovered_filter_cell;
    long j_selected_filter_cell;

    // Status
    t_status j_filter_status;
    t_status j_filter_status_override;
    char j_filter_idle_status_text[512];
    char j_filter_hover_status_text[1024];
    char j_filter_confirm_status_text[512];

    // Pagination
    long j_filter_pagination_number;
} t_presetter;

// -----------------------------------------------------------------------------
// Forward Declarations
// -----------------------------------------------------------------------------

/* Init */

t_presetter *presetter_new(t_symbol *s, short argc, t_atom *argv);
void presetter_free(t_presetter *p);

/* Attributes */

t_max_err presetter_set_pattrstorage(t_presetter *p, t_object *attr, long argc, t_atom *argv);

/* Messages */

// General
void presetter_loadbang(t_presetter *p, long action);
void presetter_bang(t_presetter *p);
void presetter_assist(t_presetter *x, void *b, long io, long index, char *s);
t_max_err presetter_notify(t_presetter *p, t_symbol *s, t_symbol *msg, void *sender, void *data);

// pattrstorage Dumpout
void presetter_read(t_presetter *p, t_symbol *s, long argc, t_atom *argv);
void presetter_recall(t_presetter *p, t_symbol *s, long argc, t_atom *argv);
void presetter_slotname(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

// Filters
void presetter_addfilter(t_presetter *p, t_symbol *s, long argc, t_atom *argv);
void presetter_renamefilter(t_presetter *p, t_symbol *s, long argc, t_atom *argv);
void presetter_clearfilter(t_presetter *p, t_symbol *s, long argc, t_atom *argv);
void presetter_removefilter(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

void presetter_addfilterslot(t_presetter *p, t_symbol *s, long argc, t_atom *argv);
void presetter_removefilterslot(t_presetter *p, t_symbol *s, long argc, t_atom *argv);
void presetter_applyfilter(t_presetter *p, t_symbol *s, long argc, t_atom *argv);
void presetter_resetfilter(t_presetter *p, t_symbol *s, long argc, t_atom *argv);
void presetter_resetfilters(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

// Catchall
void presetter_anything(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

/* Pointer events */

void presetter_mousedown(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers);
void presetter_mouseup(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers);
void presetter_mousemove(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers);
void presetter_mouseleave(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers);

/* Keyboard events */

long presetter_key(t_presetter *p, t_object *patcherview, long keycode, long modifiers, long textcharacter);

/* Drawing */

void presetter_paint(t_presetter *p, t_object *patcherview);

// -----------------------------------------------------------------------------
// Main entry
// -----------------------------------------------------------------------------

static t_class *s_presetter_class = NULL;

//

void ext_main(void *r) {
    t_class *c;

    c = class_new(
        "presetter", (method)presetter_new, (method)presetter_free, sizeof(t_presetter), (method)NULL, A_GIMME, 0
    );

    c->c_flags |= CLASS_FLAG_NEWDICTIONARY;

    jbox_initclass(c, 0);

    // Methods

    class_addmethod(c, (method)presetter_bang, "bang", 0);
    class_addmethod(c, (method)presetter_loadbang, "loadbang", 0);
    class_addmethod(c, (method)presetter_assist, "assist", A_CANT, 0);
    class_addmethod(c, (method)presetter_notify, "notify", A_CANT, 0);

    class_addmethod(c, (method)presetter_read, "read", A_GIMME, 0);
    class_addmethod(c, (method)presetter_recall, "recall", A_GIMME, 0);
    class_addmethod(c, (method)presetter_slotname, "slotname", A_GIMME, 0);

    class_addmethod(c, (method)presetter_addfilter, "addfilter", A_GIMME, 0);
    class_addmethod(c, (method)presetter_renamefilter, "renamefilter", A_GIMME, 0);
    class_addmethod(c, (method)presetter_clearfilter, "clearfilter", A_GIMME, 0);
    class_addmethod(c, (method)presetter_removefilter, "removefilter", A_GIMME, 0);

    class_addmethod(c, (method)presetter_addfilterslot, "addfilterslot", A_GIMME, 0);
    class_addmethod(c, (method)presetter_removefilterslot, "removefilterslot", A_GIMME, 0);
    class_addmethod(c, (method)presetter_resetfilters, "resetfilters", A_GIMME, 0);

    class_addmethod(c, (method)presetter_applyfilter, "applyfilter", A_GIMME, 0);
    class_addmethod(c, (method)presetter_resetfilter, "resetfilter", A_GIMME, 0);

    class_addmethod(c, (method)presetter_anything, "anything", A_GIMME, 0);

    class_addmethod(c, (method)presetter_mousedown, "mousedown", A_CANT, 0);
    class_addmethod(c, (method)presetter_mouseup, "mouseup", A_CANT, 0);
    class_addmethod(c, (method)presetter_mousemove, "mousemove", A_CANT, 0);
    class_addmethod(c, (method)presetter_mouseleave, "mouseleave", A_CANT, 0);

    class_addmethod(c, (method)presetter_key, "key", A_CANT, 0);

    class_addmethod(c, (method)presetter_paint, "paint", A_CANT, 0);

    // Attributes
    CLASS_ATTR_SYM(c, "pattrstorage", 0, t_presetter, j_pattrstorage_name);
    CLASS_ATTR_ACCESSORS(c, "pattrstorage", NULL, presetter_set_pattrstorage);
    CLASS_ATTR_LABEL(c, "pattrstorage", 0, "pattrstorage object name");
    CLASS_ATTR_SAVE(c, "pattrstorage", 0);

    CLASS_ATTR_DEFAULT(c, "patching_rect", 0, "0. 0. 257. 158.");

    class_register(CLASS_BOX, c);
    s_presetter_class = c;
}

// -----------------------------------------------------------------------------
// Lifecycle methods
// -----------------------------------------------------------------------------

/* Element Measurements (Avoid Circular Bounds References) */

void presetter_init_status_dim(t_presetter *p) {
    jgraphics_select_font_face(p->offscreen, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(p->offscreen, STATUS_FONT_SIZE);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(p->offscreen, &extents);
    p->j_status_height = extents.height;
}

/* Jbox Init */

short presetter_get_patcher_path(t_presetter *p) {
    t_object *patcher = NULL;
    object_obex_lookup(p, gensym("#P"), &patcher);

    if (!patcher)
        return 0;

    t_symbol *patcher_fpath = object_attr_getsym(patcher, gensym("filepath"));

    if (!patcher_fpath || patcher_fpath == gensym(""))
        return 0;

    short patch_pathid;
    char patcher_fname[MAX_PATH_CHARS];
    path_frompathname(patcher_fpath->s_name, &patch_pathid, patcher_fname);

    return patch_pathid;
}

t_presetter *presetter_new(t_symbol *s, short argc, t_atom *argv) {
    t_presetter *p = NULL;
    t_dictionary *d = NULL;

    if (!(d = object_dictionaryarg(argc, argv))) {
        return NULL;
    }

    p = (t_presetter *)object_alloc(s_presetter_class);

    if (p) {
        long flags = 0 | JBOX_DRAWFIRSTIN | JBOX_GROWBOTH | JBOX_NODRAWBOX | JBOX_HILITE;
        jbox_new(&p->j_box, flags, argc, argv);
        attr_dictionary_process(p, d);

        // Inlets
        p->j_box.b_firstin = (t_object *)p;

        // Outlets
        p->j_outlet1 = outlet_new((t_object *)p, NULL);

        // Slots
        p->j_slots = hashtab_new(0);

        // jgraphics
        p->surface = jgraphics_image_surface_create(JGRAPHICS_FORMAT_A8, 1, 1);
        p->offscreen = jgraphics_create(p->surface);

        // Grid
        p->j_selected_preset_cell = -1;
        p->j_hovered_preset_cell = -1;
        p->j_last_hovered_preset_cell = -2;

        // Preset Name
        p->j_editing_preset_name = false;

        // Write Button
        p->j_write_button_down = false;

        // Status
        p->j_preset_status = PRESETTER_NO_STATUS;
        p->j_preset_status_override = PRESETTER_NO_STATUS;
        presetter_init_status_dim(p);

        // Confirm
        p->j_confirm_cell = -1;
        p->j_confirm_store = false;
        p->j_confirm_delete = false;
        p->j_confirm_ok_button_down = false;
        p->j_confirm_cancel_button_down = false;

        // Pagination
        p->j_preset_pagination_number = 1;
        p->j_pagination_left_arrow_down = false;
        p->j_pagination_right_arrow_down = false;

        p->j_patcher_path = presetter_get_patcher_path(p);
        t_dictionary *dict = dictionary_new();
        p->j_filters = dict;
        p->j_applied_filters = hashtab_new(0);

        p->j_selected_tab = gensym("filters");

        p->j_selected_filter_cell = -1;
        p->j_hovered_filter_cell = -1;
        p->j_last_hovered_filter_cell = -1;

        // Status
        p->j_filter_status = PRESETTER_NO_STATUS;
        p->j_filter_status_override = PRESETTER_NO_STATUS;

        // Pagination
        p->j_filter_pagination_number = 1;

        if (dictionary_read("filters.json", p->j_patcher_path, &p->j_filters) != MAX_ERR_NONE) {
            // dictionary_read vaporizes the passed in dictionary if the file
            // doesn't exist. that is not good.
            p->j_filters = dictionary_new();
        }

        jbox_ready(&p->j_box);
    }

    return p;
}

void presetter_free(t_presetter *p) {
    object_free(p->j_slots);
    jbox_free((t_jbox *)p);
    if (p->j_pattrstorage) {
        object_detach_byptr(p, p->j_pattrstorage);
    }
    jgraphics_destroy(p->offscreen);
    jgraphics_surface_destroy(p->surface);
    // object_free(p->j_filters);
}

// -----------------------------------------------------------------------------
// Utility Methods
// -----------------------------------------------------------------------------

t_symbol *presetter_long_to_sym(long i) {
    char key[24];
    snprintf_zero(key, sizeof(key), "%ld", i);
    return gensym(key);
}

/* Redraw Utilities */

void presetter_redraw_deferred(t_presetter *p, t_symbol *s, short arc, t_atom *argv) {
    jbox_redraw((t_jbox *)p);
}

/* Colors Utilities */

void presetter_hex_to_rgba(t_jrgba *color, const char *hex, double alpha) {
    unsigned int r, g, b;
    const char *h = (hex[0] == '#') ? hex + 1 : hex;
    sscanf(h, "%2x%2x%2x", &r, &g, &b);
    jrgba_set(color, r / 255.0, g / 255.0, b / 255.0, alpha);
}

/* Hashtab Utilities */

t_symbol *presetter_lookup_preset_slot(t_presetter *p, long index) {
    t_symbol *name = NULL;
    hashtab_lookup(p->j_slots, presetter_long_to_sym(index), (t_object **)&name);
    return name;
}

void presetter_hashtab_clear_deferred(t_presetter *p, t_symbol *s, short arc, t_atom *argv) {
    if (p->j_applied_filters) {
        hashtab_clear(p->j_applied_filters);
    }
    jbox_redraw((t_jbox *)p);
}

/* Filter Dictionary Utilities */

t_dictionary *presetter_lookup_filter_slot(t_presetter *p, long index) {
    t_dictionary *dict = NULL;

    dictionary_getdictionary(p->j_filters, presetter_long_to_sym(index), (t_object **)&dict);

    return dict;
}

bool presetter_find_filter_by_name(t_presetter *p, t_symbol *s, t_filter_result *result) {
    t_dictionary *fs = p->j_filters;

    long numkeys = 0;
    t_symbol **keys = NULL;

    if (dictionary_getkeys(fs, &numkeys, &keys) != MAX_ERR_NONE) {
        return false;
    }

    for (long i = 0; i < numkeys; i++) {
        t_symbol *key = keys[i];

        t_dictionary *obj = NULL;
        dictionary_getdictionary(fs, key, (t_object **)&obj);

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

            dictionary_freekeys(fs, numkeys, keys);
            return true;
        }
    }

    dictionary_freekeys(fs, numkeys, keys);
    return false;
}

t_symbol *presetter_find_free_filter_slot(t_dictionary *d) {
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
        char keystr[24];
        snprintf_zero(keystr, sizeof(keystr), "%ld", index);
        key = gensym(keystr);
    }

    t_dictionary *obj = NULL;
    dictionary_getdictionary(p->j_filters, key, (t_object **)&obj);

    if (obj) {
        return false;
    }

    obj = dictionary_new();

    if (dictionary_appenddictionary(p->j_filters, key, (t_object *)obj) != MAX_ERR_NONE) {
        return false;
    }

    t_atomarray *arr = atomarray_new(0, NULL);

    if (dictionary_appendatomarray(obj, gensym("slots"), (t_object *)arr) != MAX_ERR_NONE) {
        return false;
    }

    if (dictionary_appendsym(obj, gensym("name"), name) != MAX_ERR_NONE) {
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
        defer_low((t_object *)p, (method)presetter_hashtab_clear_deferred, NULL, 0, NULL);
        return true;
    }

    return false;
}

bool presetter_set_filter_slot(t_presetter *p, t_symbol *s, long slot) {
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

bool presetter_drop_filter_slot(t_presetter *p, t_symbol *s, long slot) {
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

    long idx = -1;

    for (long i = 0; i < size; i++) {
        t_atom *result = &results[i];
        if (atom_gettype(result) == A_LONG && atom_getlong(result) == slot) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        return false;
    }

    atomarray_chuckindex(arr, idx);

    return true;
}

bool presetter_apply_filter_sym(t_presetter *p, t_symbol *s) {
    t_filter_result result;

    if (!presetter_find_filter_by_name(p, s, &result)) {
        return false;
    }

    return hashtab_storelong(p->j_applied_filters, result.index, (t_atom_long) true) == MAX_ERR_NONE;
}

bool presetter_reset_filter_sym(t_presetter *p, t_symbol *s) {
    t_filter_result result;

    if (!presetter_find_filter_by_name(p, s, &result)) {
        return false;
    }

    return hashtab_delete(p->j_applied_filters, result.index) == MAX_ERR_NONE;
}

void presetter_reset_filter_all(t_presetter *p) {
    hashtab_clear(p->j_applied_filters);
}

bool presetter_filtered_cell(t_presetter *p, long cell_idx) {
    if (hashtab_getsize(p->j_applied_filters) == 0) {
        return false;
    }

    long kc;
    t_symbol **kvs = NULL;
    hashtab_getkeys(p->j_applied_filters, &kc, &kvs);

    bool found_match = false;

    if (kvs == NULL) {
        return false;
    }

    for (long i = 0; i < kc; i++) {
        if (found_match)
            break;

        t_symbol *key = kvs[i];
        t_dictionary *obj = NULL;
        dictionary_getdictionary(p->j_filters, key, (t_object **)&obj);

        if (!obj)
            continue;

        t_atomarray *arr = NULL;
        dictionary_getatomarray(obj, gensym("slots"), (t_object **)&arr);

        long ac = 0;
        t_atom *av = NULL;
        atomarray_getatoms(arr, &ac, &av);

        if (ac == 0 || av == NULL)
            continue;

        for (long j = 0; j < ac; j++) {
            if (atom_gettype(&av[j]) == A_LONG && atom_getlong(&av[j]) == cell_idx) {
                found_match = true;
                break;
            }
        }
    }

    return found_match;
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
        }
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
            p->j_preset_idle_status_text, sizeof(p->j_preset_idle_status_text), "Selected Preset %d", cell_idx
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
        dictionary_write(p->j_filters, "filters.json", p->j_patcher_path);
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
        dictionary_write(p->j_filters, "filters.json", p->j_patcher_path);
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
        dictionary_write(p->j_filters, "filters.json", p->j_patcher_path);
        return;
    }
}

void presetter_removefilter(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    if (argc < 1)
        return;

    if (atom_gettype(argv) != A_SYM)
        return;

    if (presetter_remove_filter_sym(p, atom_getsym(argv))) {
        dictionary_write(p->j_filters, "filters.json", p->j_patcher_path);
        defer_low((t_object *)p, (method)presetter_hashtab_clear_deferred, NULL, 0, NULL);
        return;
    }
}

void presetter_addfilterslot(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    if (argc < 2)
        return;

    if (atom_gettype(argv) != A_SYM || atom_gettype(argv + 1) != A_LONG)
        return;

    if (presetter_set_filter_slot(p, atom_getsym(argv), atom_getlong(argv + 1))) {
        dictionary_write(p->j_filters, "filters.json", p->j_patcher_path);
        defer_low((t_object *)p, (method)presetter_redraw_deferred, NULL, 0, NULL);
        return;
    }
}

void presetter_removefilterslot(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    if (argc < 2)
        return;

    if (atom_gettype(argv) != A_SYM || atom_gettype(argv + 1) != A_LONG)
        return;

    if (presetter_drop_filter_slot(p, atom_getsym(argv), atom_getlong(argv + 1))) {
        dictionary_write(p->j_filters, "filters.json", p->j_patcher_path);
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

// Pass through unknown messages silently
void presetter_anything(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    // post("%s", s->s_name);
    return;
}

// -----------------------------------------------------------------------------
// Callback Methods
// -----------------------------------------------------------------------------

void presetter_handle_recall(t_presetter *p, long cell_idx) {
    t_object *ps = presetter_find_pattrstorage(p);
    if (!ps)
        return;

    t_symbol *slot = presetter_lookup_preset_slot(p, cell_idx);

    if (slot) {
        p->j_selected_preset_cell = cell_idx;

        snprintf_zero(p->j_preset_name, sizeof(p->j_preset_name), "%s", slot->s_name);

        snprintf_zero(
            p->j_preset_idle_status_text, sizeof(p->j_preset_idle_status_text), "Selected Preset %d",
            p->j_selected_preset_cell
        );

        p->j_preset_status = PRESETTER_IDLE_STATUS;

        t_atom a;
        atom_setlong(&a, cell_idx);
        object_method_typed(ps, gensym("recall"), 1, &a, NULL);

        jbox_redraw((t_jbox *)p);
    }
}

void presetter_handle_store(t_presetter *p, long cell_idx) {
    t_object *ps = presetter_find_pattrstorage(p);
    if (!ps)
        return;

    snprintf_zero(p->j_preset_idle_status_text, sizeof(p->j_preset_idle_status_text), "Stored Preset %d", cell_idx);
    p->j_preset_status = PRESETTER_IDLE_STATUS;

    t_atom a;
    atom_setlong(&a, cell_idx);
    object_method_typed(ps, gensym("store"), 1, &a, NULL);
    object_method_typed(ps, gensym("getslotnamelist"), 0, NULL, NULL);
    outlet_bang(p->j_outlet1);

    jbox_redraw((t_jbox *)p);
}

void presetter_handle_delete(t_presetter *p, long cell_idx) {
    t_object *ps = presetter_find_pattrstorage(p);
    if (!ps)
        return;

    snprintf_zero(p->j_preset_idle_status_text, sizeof(p->j_preset_idle_status_text), "Deleted Preset %d", cell_idx);
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
        p->j_preset_idle_status_text, sizeof(p->j_preset_idle_status_text), "Renamed Preset %d",
        p->j_selected_preset_cell
    );
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

void presetter_handle_filter_name(t_presetter *p) {
    if (presetter_rename_filter_idx(p, p->j_selected_filter_cell, gensym(p->j_filter_name))) {
        dictionary_write(p->j_filters, "filters.json", p->j_patcher_path);
        p->j_editing_filter_name = false;
        p->j_write_filter_button_down = false;
        jbox_redraw((t_jbox *)p);
        return;
    }

    if (p->j_filter_name[0] != '\0') {
        presetter_add_filter_sym(p, gensym(p->j_filter_name), p->j_selected_filter_cell);
        dictionary_write(p->j_filters, "filters.json", p->j_patcher_path);
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
// Element Bounds
// -----------------------------------------------------------------------------

bool presetter_generic_in_bounds(t_bounds *bounds, t_pt *pt) {
    bool in_x = pt->x >= bounds->x && pt->x <= bounds->width + bounds->x;
    bool in_y = pt->y >= bounds->y && pt->y <= bounds->height + bounds->y;

    return in_x && in_y;
}

/* Tabs */

void presetter_measure_tab(t_presetter *p, char *text, double *outwidth, double *outheight) {
    jgraphics_select_font_face(p->offscreen, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(p->offscreen, TAB_FONT_SIZE);

    double text_width;
    double text_height;
    jgraphics_text_measure(p->offscreen, text, &text_width, &text_height);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(p->offscreen, &extents);

    *outwidth = text_width + TAB_PADDING_X * 2;
    *outheight = extents.ascent + extents.descent + TAB_PADDING_Y * 2;
}

t_bounds presetter_get_presets_tab_bounds(t_presetter *p, t_rect *rect) {
    t_bounds bounds;
    presetter_measure_tab(p, TAB_PRESETS_TEXT, &bounds.width, &bounds.height);
    bounds.x = 0;
    bounds.y = rect->height - bounds.height;
    return bounds;
}

bool presetter_in_presets_tab_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_presets_tab_bounds(p, rect);
    return presetter_generic_in_bounds(&bounds, pt);
}

t_bounds presetter_get_filters_tab_bounds(t_presetter *p, t_rect *rect) {
    t_bounds ptbounds = presetter_get_presets_tab_bounds(p, rect);

    t_bounds bounds;
    presetter_measure_tab(p, TAB_FILTERS_TEXT, &bounds.width, &bounds.height);
    bounds.x = ptbounds.width;
    bounds.y = rect->height - bounds.height;
    return bounds;
}

bool presetter_in_filters_tab_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_filters_tab_bounds(p, rect);
    return presetter_generic_in_bounds(&bounds, pt);
}

/* Write Buttons */

void presetter_measure_button(t_presetter *p, char *text, double *outwidth, double *outheight) {
    jgraphics_select_font_face(p->offscreen, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(p->offscreen, BUTTON_FONT_SIZE);

    double text_width;
    double text_height;
    jgraphics_text_measure(p->offscreen, text, &text_width, &text_height);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(p->offscreen, &extents);

    *outwidth = text_width + BUTTON_PADDING_X * 2;
    *outheight = extents.ascent + extents.descent + BUTTON_PADDING_Y * 2;
}

/* Write Button Bounds */

t_bounds presetter_get_write_button_bounds(t_presetter *p, t_rect *rect) {
    t_bounds bounds;
    presetter_measure_button(p, WRITE_BUTTON_TEXT, &bounds.width, &bounds.height);
    bounds.x = rect->width - bounds.width - WRITE_BUTTON_OFFSET_X;
    bounds.y = WRITE_BUTTON_OFFSET_Y;
    return bounds;
}

bool presetter_in_write_button_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_write_button_bounds(p, rect);
    return presetter_generic_in_bounds(&bounds, pt);
}

/* Preset Name Bounds */

t_bounds presetter_get_write_filter_button_bounds(t_presetter *p, t_rect *rect);

t_bounds presetter_get_name_bounds(t_presetter *p, t_rect *rect) {
    jgraphics_select_font_face(p->offscreen, "Menlo", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(p->offscreen, WRITE_NAME_FONT_SIZE);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(p->offscreen, &extents);

    t_bounds wbbounds;
    if (p->j_selected_tab == gensym("presets")) {
        wbbounds = presetter_get_write_button_bounds(p, rect);
    } else {
        wbbounds = presetter_get_write_filter_button_bounds(p, rect);
    }

    t_bounds bounds;
    bounds.x = WRITE_NAME_OFFSET_X;
    bounds.y = WRITE_NAME_OFFSET_Y;
    bounds.width = rect->width - wbbounds.width - WRITE_NAME_OFFSET_X - WRITE_BUTTON_OFFSET_X - WRITE_NAME_MARGIN_RIGHT;
    bounds.height = extents.height;
    return bounds;
}

bool presetter_in_name_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_name_bounds(p, rect);
    return presetter_generic_in_bounds(&bounds, pt);
}

/* Grid Bounds */

t_grid_dim presetter_get_preset_grid_dim(t_presetter *p, t_rect *rect) {
    t_bounds pnbounds = presetter_get_name_bounds(p, rect);
    t_bounds ptbounds = presetter_get_presets_tab_bounds(p, rect);

    t_grid_dim dim;
    dim.columns = (int)floor((rect->width - CELL_PADDING - GRID_OFFSET_X) / CELL_TOTAL_SIZE);

    double grid_y_pos = WRITE_NAME_OFFSET_Y + pnbounds.height + GRID_OFFSET_Y + ptbounds.height + TAB_MARGIN;
    double grid_space = rect->height - grid_y_pos - p->j_status_height - STATUS_OFFSET_Y - CELL_PADDING;
    dim.rows = (int)floor(grid_space / CELL_TOTAL_SIZE);
    if (dim.rows < 0)
        dim.rows = 0;

    return dim;
}

t_bounds presetter_get_preset_grid_bounds(t_presetter *p, t_rect *rect) {
    t_bounds pnbounds = presetter_get_name_bounds(p, rect);

    t_bounds bounds;
    t_grid_dim dim = presetter_get_preset_grid_dim(p, rect);

    bounds.x = GRID_OFFSET_X;
    bounds.y = pnbounds.y + pnbounds.height + GRID_OFFSET_Y;
    bounds.width = dim.columns * CELL_TOTAL_SIZE + CELL_PADDING / 2;
    bounds.height = dim.rows * CELL_TOTAL_SIZE + CELL_PADDING / 2;

    return bounds;
}

bool presetter_in_grid_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_preset_grid_bounds(p, rect);
    return presetter_generic_in_bounds(&bounds, pt);
}

/* Grid Utilities */

t_cell_pos presetter_get_preset_cell_pos(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_preset_grid_bounds(p, rect);

    t_cell_pos pos;
    double rel_x = pt->x - bounds.x - CELL_PADDING / 2;
    double rel_y = pt->y - bounds.y - CELL_PADDING / 2;
    pos.x = (int)floor(rel_x / CELL_TOTAL_SIZE);
    pos.y = (int)floor(rel_y / CELL_TOTAL_SIZE);
    return pos;
}

long presetter_get_preset_grid_offset(t_presetter *p, t_grid_dim *dim) {
    return (p->j_preset_pagination_number - 1) * dim->rows * dim->columns;
}

long presetter_get_preset_cell_idx(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_grid_dim dim = presetter_get_preset_grid_dim(p, rect);
    t_cell_pos pos = presetter_get_preset_cell_pos(p, rect, pt);

    return (pos.x + dim.columns * pos.y + 1) + presetter_get_preset_grid_offset(p, &dim);
}

/* Right Arrow Bounds */

t_bounds presetter_get_right_arrow_bounds(t_presetter *p, t_rect *rect) {
    t_bounds gbounds = presetter_get_preset_grid_bounds(p, rect);

    double text_width;
    double text_height;

    jgraphics_select_font_face(
        p->offscreen, PAGINATION_ARROW_FONT, JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD
    );
    jgraphics_set_font_size(p->offscreen, PAGINATION_ARROW_FONT_SIZE);
    jgraphics_text_measure(p->offscreen, PAGINATION_RIGHT_ARROW_TEXT, &text_width, &text_height);

    t_bounds bounds;
    bounds.x = rect->width - text_width - PAGINATION_MARGIN_RIGHT;
    bounds.y = gbounds.y + gbounds.height + PAGINATION_MARGIN_TOP;
    bounds.width = text_width;
    bounds.height = text_height;
    return bounds;
}

bool presetter_in_right_arrow_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_right_arrow_bounds(p, rect);
    return presetter_generic_in_bounds(&bounds, pt);
}

/* Pagination Number Bounds */

t_bounds presetter_get_pagination_number_bounds(t_presetter *p, t_rect *rect) {
    t_bounds gbounds = presetter_get_preset_grid_bounds(p, rect);
    t_bounds rabounds = presetter_get_right_arrow_bounds(p, rect);

    double text_width;
    double text_height;

    char number_text[16];
    snprintf_zero(number_text, sizeof(number_text), "%d", p->j_preset_pagination_number);

    jgraphics_select_font_face(
        p->offscreen, PAGINATION_NUMBER_FONT, JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD
    );
    jgraphics_set_font_size(p->offscreen, PAGINATION_NUMBER_FONT_SIZE);
    jgraphics_text_measure(p->offscreen, number_text, &text_width, &text_height);

    t_bounds bounds;
    bounds.x = rect->width - text_width - PAGINATION_ARROW_PADDING - (rect->width - rabounds.x);
    bounds.y = gbounds.y + gbounds.height + PAGINATION_MARGIN_TOP;
    bounds.width = text_width;
    bounds.height = rabounds.height;

    return bounds;
}

/* Left Arrow Bounds */

t_bounds presetter_get_left_arrow_bounds(t_presetter *p, t_rect *rect) {
    t_bounds gbounds = presetter_get_preset_grid_bounds(p, rect);
    t_bounds pnbounds = presetter_get_pagination_number_bounds(p, rect);

    double text_width;
    double text_height;

    jgraphics_select_font_face(
        p->offscreen, PAGINATION_ARROW_FONT, JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD
    );
    jgraphics_set_font_size(p->offscreen, PAGINATION_ARROW_FONT_SIZE);
    jgraphics_text_measure(p->offscreen, PAGINATION_RIGHT_ARROW_TEXT, &text_width, &text_height);

    t_bounds bounds;
    bounds.x = rect->width - text_width - PAGINATION_ARROW_PADDING - (rect->width - pnbounds.x);
    bounds.y = gbounds.y + gbounds.height + PAGINATION_MARGIN_TOP;
    bounds.width = text_width;
    bounds.height = text_height;
    return bounds;
}

bool presetter_in_left_arrow_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_left_arrow_bounds(p, rect);
    return presetter_generic_in_bounds(&bounds, pt);
}

/* Status Bounds */

t_bounds presetter_get_status_bounds(t_presetter *p, t_rect *rect) {
    t_bounds gbounds = presetter_get_preset_grid_bounds(p, rect);
    // t_bounds ra_bounds = presetter_get_right_arrow_bounds(p, rect);
    t_bounds la_bounds = presetter_get_left_arrow_bounds(p, rect);

    t_bounds bounds;
    bounds.x = STATUS_OFFSET_X;
    bounds.y = gbounds.y + gbounds.height + STATUS_OFFSET_Y;
    bounds.width = rect->width - STATUS_OFFSET_X - STATUS_PADDING_RIGHT - (rect->width - la_bounds.x);
    bounds.height = p->j_status_height;
    return bounds;
}

bool presetter_in_preset_status_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_status_bounds(p, rect);
    return presetter_generic_in_bounds(&bounds, pt);
}

/* Confirm Ok Button Bounds */

t_bounds presetter_get_confirm_ok_button_bounds(t_presetter *p, t_rect *rect) {
    t_bounds gbounds = presetter_get_preset_grid_bounds(p, rect);

    double text_width;
    double text_height;

    jgraphics_select_font_face(p->offscreen, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(p->offscreen, 9);
    jgraphics_text_measure(p->offscreen, p->j_preset_confirm_status_text, &text_width, &text_height);

    t_bounds bounds;
    bounds.x = text_width + CONFIRM_OK_BUTTON_MARGIN_LEFT;
    bounds.y = gbounds.y + gbounds.height + CONFIRM_BUTTON_OFFSET_Y;
    presetter_measure_button(p, CONFIRM_OK_BUTTON_TEXT, &bounds.width, &bounds.height);
    return bounds;
}

bool presetter_in_confirm_ok_button_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_confirm_ok_button_bounds(p, rect);
    return presetter_generic_in_bounds(&bounds, pt);
}

t_bounds presetter_get_confirm_cancel_button_bounds(t_presetter *p, t_rect *rect) {
    t_bounds cobounds = presetter_get_confirm_ok_button_bounds(p, rect);

    t_bounds bounds;
    bounds.x = cobounds.x + cobounds.width + CONFIRM_CANCEL_BUTTON_MARGIN_LEFT;
    bounds.y = cobounds.y;
    presetter_measure_button(p, CONFIRM_CANCEL_BUTTON_TEXT, &bounds.width, &bounds.height);
    return bounds;
}

bool presetter_in_confirm_cancel_button_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_confirm_cancel_button_bounds(p, rect);
    return presetter_generic_in_bounds(&bounds, pt);
}

/** Filters **/

long presetter_get_filter_grid_offset(t_presetter *p, t_grid_dim *dim) {
    return (p->j_filter_pagination_number - 1) * dim->columns * dim->rows;
}

t_bounds presetter_get_filter_grid_bounds(t_presetter *p, t_rect *rect) {
    t_bounds pnbounds = presetter_get_name_bounds(p, rect);

    t_bounds bounds;
    t_grid_dim dim = presetter_get_preset_grid_dim(p, rect);

    bounds.x = GRID_OFFSET_X;
    bounds.y = pnbounds.y + pnbounds.height + GRID_OFFSET_Y;
    bounds.width = dim.columns * CELL_TOTAL_SIZE + CELL_PADDING / 2;
    bounds.height = dim.rows * CELL_TOTAL_SIZE + CELL_PADDING / 2;

    return bounds;
}

t_cell_pos presetter_get_filter_cell_pos(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds pgbounds = presetter_get_preset_grid_bounds(p, rect);
    t_cell_pos pos;

    long columns = (long)floor(rect->width / FILTER_CELL_MIN_WIDTH);
    long rows = (long)floor(pgbounds.height / (FILTER_CELL_HEIGHT + FILTER_GRID_PADDING));

    if (columns == 0)
        columns = 1;

    if (rows == 0)
        rows = 1;

    double cell_width = (rect->width - 8.5 * 2 - (FILTER_GRID_PADDING * (columns - 1))) / columns;

    pos.x = (pt->x - pgbounds.x - 8.5) / (cell_width);
    pos.y = (pt->y - pgbounds.y - 3) / (FILTER_CELL_HEIGHT + FILTER_GRID_PADDING);

    return pos;
}

long presetter_get_filter_cell_idx(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_cell_pos pos = presetter_get_filter_cell_pos(p, rect, pt);

    t_bounds pgbounds = presetter_get_preset_grid_bounds(p, rect);
    t_grid_dim dim;

    dim.columns = (long)floor(rect->width / FILTER_CELL_MIN_WIDTH);
    dim.rows = (long)floor(pgbounds.height / (FILTER_CELL_HEIGHT + FILTER_GRID_PADDING));

    long offset = presetter_get_filter_grid_offset(p, &dim);
    return (pos.x + dim.columns * pos.y + 1) + offset;
}

bool presetter_in_filter_circle_bounds(t_presetter *p, t_rect *rect, t_pt *pt, t_cell_pos *pos) {
    t_bounds pgbounds = presetter_get_preset_grid_bounds(p, rect);

    long columns = (long)floor(rect->width / FILTER_CELL_MIN_WIDTH);
    if (columns == 0)
        columns = 1;

    double cell_width = (rect->width - 8.5 * 2 - (FILTER_GRID_PADDING * (columns - 1))) / columns;

    double x = pos->x * (cell_width + FILTER_GRID_PADDING) + 8.5;
    double y = pgbounds.y + 3 + pos->y * (FILTER_CELL_HEIGHT + FILTER_GRID_PADDING);

    double circle_x = x + 6;
    double circle_y = y + ((double)FILTER_CELL_HEIGHT - 8) / 2;

    // 8x8 hitbox (the circle's diameter)
    return (pt->x >= circle_x && pt->x <= circle_x + 8 && pt->y >= circle_y && pt->y <= circle_y + 8);
}

/* Write Button Bounds */

t_bounds presetter_get_write_filter_button_bounds(t_presetter *p, t_rect *rect) {
    t_bounds bounds;
    presetter_measure_button(p, WRITE_FILTER_BUTTON_TEXT, &bounds.width, &bounds.height);
    bounds.x = rect->width - bounds.width - WRITE_BUTTON_OFFSET_X;
    bounds.y = WRITE_BUTTON_OFFSET_Y;
    return bounds;
}

bool presetter_in_write_filter_button_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_write_filter_button_bounds(p, rect);
    return presetter_generic_in_bounds(&bounds, pt);
}

// -----------------------------------------------------------------------------
// Pointer Event Methods
// -----------------------------------------------------------------------------

void presetter_clear_confirm(t_presetter *p) {
    p->j_confirm_cell = -1;
    p->j_confirm_store = false;
    p->j_confirm_delete = false;
    p->j_preset_status_override = PRESETTER_NO_STATUS;
    p->j_preset_confirm_status_text[0] = '\0';
    p->j_confirm_ok_button_down = false;
    p->j_confirm_cancel_button_down = false;
}

void presetter_mousedown(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers) {
    t_rect rect;
    jbox_get_rect_for_view((t_object *)p, patcherview, &rect);

    if (presetter_in_name_bounds(p, &rect, &pt) && p->j_selected_tab == gensym("presets")) {
        if (p->j_selected_preset_cell != -1) {
            p->j_editing_preset_name = !p->j_editing_preset_name;
            presetter_clear_confirm(p);
            jbox_redraw((t_jbox *)p);
        }
        return;
    }

    if (presetter_in_name_bounds(p, &rect, &pt) && p->j_selected_tab == gensym("filters")) {
        if (p->j_selected_filter_cell != -1) {
            p->j_editing_filter_name = !p->j_editing_filter_name;
            presetter_clear_confirm(p);
            jbox_redraw((t_jbox *)p);
        }
        return;
    }

    if (presetter_in_write_button_bounds(p, &rect, &pt) && p->j_selected_tab == gensym("presets")) {
        if (p->j_editing_preset_name) {
            p->j_write_button_down = true;
            jbox_redraw((t_jbox *)p);
            return;
        }
    }

    if (presetter_in_write_button_bounds(p, &rect, &pt) && p->j_selected_tab == gensym("filters")) {
        if (p->j_editing_filter_name) {
            p->j_write_filter_button_down = true;
            jbox_redraw((t_jbox *)p);
            return;
        }
    }

    if (presetter_in_grid_bounds(p, &rect, &pt)) {
        if (p->j_selected_tab == gensym("presets")) {
            long cell_idx = presetter_get_preset_cell_idx(p, &rect, &pt);

            if (modifiers & eLeftButton && (modifiers & eAltKey) && (modifiers & eShiftKey) &&
                !((modifiers & eCommandKey) || (modifiers & eControlKey))) {
                p->j_editing_preset_name = false;
                p->j_confirm_delete = true;
                p->j_confirm_cell = cell_idx;
                p->j_preset_status_override = PRESETTER_CONFIRM_STATUS;
                snprintf_zero(
                    p->j_preset_confirm_status_text, sizeof(p->j_preset_confirm_status_text), "Delete Preset %d?",
                    cell_idx
                );
                jbox_redraw((t_jbox *)p);
                return;
            }

            if (modifiers & eLeftButton && !(modifiers & eShiftKey) && !(modifiers & eAltKey)) {
                p->j_editing_preset_name = false;
                presetter_clear_confirm(p);
                presetter_handle_recall(p, cell_idx);
                jbox_redraw((t_jbox *)p);
                return;
            }

            if (modifiers & eLeftButton && modifiers & eShiftKey && !(modifiers & eAltKey)) {
                p->j_editing_preset_name = false;
                p->j_confirm_store = true;
                p->j_confirm_cell = cell_idx;
                p->j_preset_status_override = PRESETTER_CONFIRM_STATUS;
                snprintf_zero(
                    p->j_preset_confirm_status_text, sizeof(p->j_preset_confirm_status_text), "Store Preset %d?",
                    cell_idx
                );
                jbox_redraw((t_jbox *)p);
                return;
            }
        } else {
            t_cell_pos pos = presetter_get_filter_cell_pos(p, &rect, &pt);
            long cell_idx = presetter_get_filter_cell_idx(p, &rect, &pt);

            if (presetter_in_filter_circle_bounds(p, &rect, &pt, &pos)) {
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
                p->j_filter_idle_status_text, sizeof(p->j_filter_idle_status_text), "Selected Filter %d",
                p->j_selected_filter_cell
            );

            p->j_preset_status = PRESETTER_IDLE_STATUS;
            p->j_editing_filter_name = false;

            jbox_redraw((t_jbox *)p);

            return;
        }
    }

    if ((p->j_confirm_store || p->j_confirm_delete) && p->j_preset_confirm_status_text[0] != '\0' &&
        p->j_confirm_cell != -1) {
        if (presetter_in_confirm_ok_button_bounds(p, &rect, &pt)) {
            p->j_confirm_ok_button_down = true;
        }

        if (presetter_in_confirm_cancel_button_bounds(p, &rect, &pt)) {
            p->j_confirm_cancel_button_down = true;
        }
        jbox_redraw((t_jbox *)p);
        return;
    }

    if (presetter_in_right_arrow_bounds(p, &rect, &pt)) {
        p->j_pagination_right_arrow_down = true;
        jbox_redraw((t_jbox *)p);
        return;
    }

    if (presetter_in_left_arrow_bounds(p, &rect, &pt)) {
        if (p->j_selected_tab == gensym("presets") && p->j_preset_pagination_number > 1) {
            p->j_pagination_left_arrow_down = true;
        }
        if (p->j_selected_tab == gensym("filters") && p->j_filter_pagination_number > 1) {
            p->j_pagination_left_arrow_down = true;
        }
        jbox_redraw((t_jbox *)p);
        return;
    }

    if (presetter_in_presets_tab_bounds(p, &rect, &pt)) {
        p->j_selected_tab = gensym("presets");
        p->j_editing_filter_name = false;
        p->j_editing_preset_name = false;
        jbox_redraw((t_jbox *)p);
        return;
    }

    if (presetter_in_filters_tab_bounds(p, &rect, &pt)) {
        p->j_selected_tab = gensym("filters");
        p->j_editing_filter_name = false;
        p->j_editing_preset_name = false;
        jbox_redraw((t_jbox *)p);
        return;
    }
}

void presetter_mouseup(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers) {
    if (p->j_write_button_down) {
        presetter_handle_preset_rename(p);
        p->j_write_button_down = false;
        jbox_redraw((t_jbox *)p);
        return;
    }

    if (p->j_write_filter_button_down) {
        presetter_handle_filter_name(p);
        return;
    }

    if (p->j_confirm_ok_button_down) {
        if (p->j_confirm_store && p->j_confirm_cell != -1) {
            presetter_handle_store(p, p->j_confirm_cell);
        } else if (p->j_confirm_delete && p->j_confirm_cell != -1) {
            presetter_handle_delete(p, p->j_confirm_cell);
        }
        presetter_clear_confirm(p);
        jbox_redraw((t_jbox *)p);
        return;
    }

    if (p->j_confirm_cancel_button_down) {
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
}

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
                    p->j_preset_hover_status_text, sizeof(p->j_preset_hover_status_text), "Preset %d: %s",
                    p->j_hovered_preset_cell, name->s_name
                );
            } else {
                snprintf_zero(
                    p->j_preset_hover_status_text, sizeof(p->j_preset_hover_status_text), "Preset %d",
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
            p->j_filter_status = PRESETTER_IDLE_STATUS;
            p->j_hovered_filter_cell = -1;
            p->j_last_hovered_filter_cell = -2;
        }
    }

    jbox_redraw((t_jbox *)p);
}

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
            presetter_handle_filter_name(p);
            return 1;
        }

        // Escape
        if (keycode == -3) {
            p->j_editing_filter_name = false;
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
// Drawing
//------------------------------------------------------------------------------

/* Write Name */

void presetter_draw_write_name(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_bounds bounds = presetter_get_name_bounds(p, rect);

    char text[512];

    if (p->j_selected_tab == gensym("presets")) {
        if (p->j_editing_preset_name) {
            snprintf_zero(text, sizeof(text), "> %s_", p->j_preset_name);
        } else {
            if (p->j_preset_name[0] != '\0') {
                snprintf_zero(text, sizeof(text), "%s", p->j_preset_name);
            } else {
                snprintf_zero(text, sizeof(text), "%s", "No preset selected");
            }
        }
    } else {
        if (p->j_editing_filter_name) {
            snprintf_zero(text, sizeof(text), "> %s_", p->j_filter_name);
        } else {
            if (p->j_filter_name[0] != '\0') {
                snprintf_zero(text, sizeof(text), "%s", p->j_filter_name);
            } else if (p->j_selected_filter_cell != -1) {
                snprintf_zero(text, sizeof(text), "%s", "Click to create new");
            } else {
                snprintf_zero(text, sizeof(text), "%s", "No filter selected");
            }
        }
    }

    jgraphics_select_font_face(g, "Menlo", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(g, WRITE_NAME_FONT_SIZE);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(g, &extents);

    double max_width = bounds.width;
    double text_width;
    double text_height;
    char visible[512];

    jgraphics_text_measure(g, text, &text_width, &text_height);

    if ((p->j_editing_preset_name || p->j_editing_filter_name) && text_width > max_width) {
        const char *prefix = "> ";

        double prefix_width;
        double prefix_height;
        jgraphics_text_measure(g, prefix, &prefix_width, &prefix_height);

        const char *content = text + strlen(prefix);
        double content_max = max_width - prefix_width;

        double content_width;
        double content_height;
        jgraphics_text_measure(g, content, &content_width, &content_height);

        while (content_width > content_max && *content != '\0') {
            content++;
            jgraphics_text_measure(g, content, &content_width, &content_height);
        }

        snprintf_zero(visible, sizeof(visible), "%s%s", prefix, content);
        jgraphics_text_measure(g, visible, &text_width, &text_height);
    } else if (!(p->j_editing_preset_name || p->j_editing_filter_name) && text_width > max_width) {
        snprintf_zero(visible, sizeof(visible), "%s", text);

        char with_ellipsis[512];
        snprintf_zero(with_ellipsis, sizeof(with_ellipsis), "%s...", visible);
        jgraphics_text_measure(g, with_ellipsis, &text_width, &text_height);

        while (text_width > max_width && strlen(visible) > 0) {
            visible[strlen(visible) - 1] = '\0';
            snprintf_zero(with_ellipsis, sizeof(with_ellipsis), "%s...", visible);
            jgraphics_text_measure(g, with_ellipsis, &text_width, &text_height);
        }

        snprintf_zero(visible, sizeof(visible), "%s...", visible);
    } else {
        snprintf_zero(visible, sizeof(visible), "%s", text);
    }

    // Draw editing background
    if (p->j_editing_preset_name || p->j_editing_filter_name) {
        t_jrgba bg_color;
        t_jrgba _bg_color_off;

        jcolor_getcolor(PRESET_NAME_EDITING_BG_COLOR_SYM, &bg_color, &_bg_color_off);

        jgraphics_set_source_jrgba(g, &bg_color);
        jgraphics_rectangle(g, bounds.x, bounds.y, text_width, bounds.height);
        jgraphics_fill(g);
    }

    double text_x = bounds.x;
    double text_y = bounds.y + extents.ascent;

    t_jrgba name_color;
    t_jrgba _name_color_off;

    if (p->j_selected_tab == gensym("presets") && p->j_preset_name[0] != '\0') {
        if (p->j_editing_preset_name) {
            jcolor_getcolor(PRESET_NAME_TEXT_EDITING_COLOR_SYM, &name_color, &_name_color_off);
        } else {
            jcolor_getcolor(PRESET_NAME_TEXT_DEFAULT_COLOR_SYM, &name_color, &_name_color_off);
        }
    } else if (p->j_selected_tab == gensym("filters") && p->j_filter_name[0] != '\0') {
        if (p->j_editing_filter_name) {
            jcolor_getcolor(PRESET_NAME_TEXT_EDITING_COLOR_SYM, &name_color, &_name_color_off);
        } else {
            jcolor_getcolor(PRESET_NAME_TEXT_DEFAULT_COLOR_SYM, &name_color, &_name_color_off);
        }
    } else {
        presetter_hex_to_rgba(&name_color, PRESET_NAME_TEXT_UNSELECTED_COLOR_HEX, 1);
    }

    jgraphics_set_source_jrgba(g, &name_color);
    jgraphics_move_to(g, text_x, text_y);
    jgraphics_text_path(g, visible);
    jgraphics_fill(g);
}

/* Write Button */

void presetter_draw_write_button(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_bounds bounds = presetter_get_write_button_bounds(p, rect);

    t_jrgba bg_color;
    t_jrgba text_color;
    t_jrgba bg_color_off;
    t_jrgba text_color_off;

    if (p->j_write_filter_button_down) {
        jcolor_getcolor(WRITE_BUTTON_ON_BG_COLOR_SYM, &bg_color, &bg_color_off);
        jcolor_getcolor(WRITE_BUTTON_ON_TEXT_COLOR_SYM, &text_color, &text_color_off);

    } else if (p->j_editing_preset_name) {
        jcolor_getcolor(WRITE_BUTTON_UP_BG_COLOR_SYM, &bg_color, &bg_color_off);
        jcolor_getcolor(WRITE_BUTTON_UP_TEXT_COLOR_SYM, &text_color, &text_color_off);

    } else {
        presetter_hex_to_rgba(&bg_color, WRITE_BUTTON_INACTIVE_BG_COLOR_HEX, 1);
        presetter_hex_to_rgba(&text_color, WRITE_BUTTON_INACTIVE_TEXT_COLOR_HEX, 1);
    }

    jgraphics_rectangle(g, bounds.x, bounds.y, bounds.width, bounds.height);

    if (!p->j_write_filter_button_down) {
        jgraphics_set_source_jrgba(g, &text_color);
        jgraphics_stroke_preserve(g);
    }

    jgraphics_set_source_jrgba(g, &bg_color);
    jgraphics_fill(g);

    jgraphics_select_font_face(g, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(g, BUTTON_FONT_SIZE);
    jgraphics_set_source_jrgba(g, &text_color);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(g, &extents);

    double text_y = bounds.y + (bounds.height + extents.ascent - extents.descent) / 2;
    jgraphics_move_to(g, bounds.x + BUTTON_PADDING_X, text_y);
    jgraphics_text_path(g, WRITE_BUTTON_TEXT);
    jgraphics_fill(g);
}

/* Grid */

void presetter_draw_preset_grid_row(t_presetter *p, t_jgraphics *g, t_grid_dim *dim, int row_idx, double y) {
    for (int i = 0; i < dim->columns; i++) {
        double x = (double)i * CELL_TOTAL_SIZE + CELL_PADDING + GRID_OFFSET_X;
        int cell_idx = (i + dim->columns * row_idx + 1) + presetter_get_preset_grid_offset(p, dim);

        t_jrgba color;
        t_symbol *name = presetter_lookup_preset_slot(p, cell_idx);
        bool filtered_cell = presetter_filtered_cell(p, cell_idx) && name;

        if (p->j_selected_preset_cell == cell_idx) {
            t_jrgba color_off;
            jcolor_getcolor(GRID_SELECTED_CELL_COLOR_SYM, &color, &color_off);
        } else if (p->j_hovered_preset_cell == cell_idx) {
            presetter_hex_to_rgba(&color, GRID_HOVERED_CELL_COLOR_HEX, 1);
        } else if (filtered_cell) {
            t_jrgba color_off;
            jcolor_getcolor(GRID_SELECTED_CELL_COLOR_SYM, &color, &color_off);
        } else if (name) {
            presetter_hex_to_rgba(&color, GRID_STORED_CELL_COLOR_HEX, 1);
        } else {
            presetter_hex_to_rgba(&color, GRID_DEFAULT_CELL_COLOR_HEX, 1);
        }

        jgraphics_rectangle_rounded(g, x, y, CELL_SIZE, CELL_SIZE, 3, 3);
        jgraphics_set_source_jrgba(g, &color);

        if (filtered_cell && p->j_selected_preset_cell != cell_idx) {
            jgraphics_fill_with_alpha(g, filtered_cell ? .5 : 1);
        }

        jgraphics_fill(g);
    }
}

void presetter_draw_preset_grid(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_bounds bounds = presetter_get_preset_grid_bounds(p, rect);
    t_grid_dim dim = presetter_get_preset_grid_dim(p, rect);

    for (int i = 0; i < dim.rows; i++) {
        double y = (double)i * CELL_TOTAL_SIZE + CELL_PADDING + bounds.y;
        presetter_draw_preset_grid_row(p, g, &dim, i, y);
    }
}

/* Status */

const char *presetter_get_status_text(t_presetter *p) {
    t_status effective =
        (p->j_preset_status_override != PRESETTER_NO_STATUS) ? p->j_preset_status_override : p->j_preset_status;

    if (effective == PRESETTER_IDLE_STATUS && p->j_preset_idle_status_text[0] != '\0')
        return p->j_preset_idle_status_text;
    if (effective == PRESETTER_HOVER_STATUS && p->j_preset_hover_status_text[0] != '\0')
        return p->j_preset_hover_status_text;
    if (effective == PRESETTER_CONFIRM_STATUS && p->j_preset_confirm_status_text[0] != '\0')
        return p->j_preset_confirm_status_text;

    return "Ready";
}

void presetter_draw_status(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_bounds bounds = presetter_get_status_bounds(p, rect);

    jgraphics_select_font_face(g, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_NORMAL);
    jgraphics_set_font_size(g, STATUS_FONT_SIZE);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(g, &extents);

    const char *status_text = presetter_get_status_text(p);

    double max_width = bounds.width;
    double text_width;
    double text_height;
    char visible[1048];
    char with_ellipsis[1048];

    jgraphics_text_measure(g, status_text, &text_width, &text_height);

    if (text_width > max_width) {
        snprintf_zero(visible, sizeof(visible), "%s", status_text);
        snprintf_zero(with_ellipsis, sizeof(with_ellipsis), "%s...", visible);

        jgraphics_text_measure(g, with_ellipsis, &text_width, &text_height);

        while (text_width > max_width && strlen(visible) > 0) {
            visible[strlen(visible) - 1] = '\0';
            snprintf_zero(with_ellipsis, sizeof(with_ellipsis), "%s...", visible);
            jgraphics_text_measure(g, with_ellipsis, &text_width, &text_height);
        }

        snprintf_zero(visible, sizeof(visible), "%s...", visible);
    } else {
        snprintf_zero(visible, sizeof(visible), "%s", status_text);
    }

    t_jrgba color;
    t_jrgba color_off;

    if (p->j_preset_status_override != PRESETTER_NO_STATUS) {
        jcolor_getcolor(STATUS_CONFIRM_TEXT_COLOR_SYM, &color, &color_off);
    } else {
        presetter_hex_to_rgba(&color, STATUS_TEXT_COLOR_HEX, 1);
    }

    jgraphics_set_source_jrgba(g, &color);
    jgraphics_move_to(g, bounds.x, bounds.y + extents.height);
    jgraphics_text_path(g, visible);
    jgraphics_fill(g);
}

void presetter_draw_confirm_ok_button(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    if (p->j_preset_confirm_status_text[0] == '\0' || !(p->j_confirm_store || p->j_confirm_delete))
        return;

    t_bounds bounds = presetter_get_confirm_ok_button_bounds(p, rect);

    t_jrgba bg_color;
    t_jrgba bg_color_off;
    t_jrgba text_color;
    t_jrgba text_color_off;

    if (p->j_confirm_ok_button_down) {
        jcolor_getcolor(CONFIRM_BUTTON_ON_BG_COLOR_SYM, &bg_color, &bg_color_off);
        jcolor_getcolor(CONFIRM_BUTTON_ON_TEXT_COLOR_SYM, &text_color, &text_color_off);
    } else {
        jcolor_getcolor(CONFIRM_BUTTON_UP_BG_COLOR_SYM, &bg_color, &bg_color_off);
        jcolor_getcolor(CONFIRM_BUTTON_UP_TEXT_COLOR_SYM, &text_color, &text_color_off);
    }

    jgraphics_rectangle(g, bounds.x, bounds.y, bounds.width, bounds.height);

    if (!p->j_confirm_ok_button_down) {
        jgraphics_set_source_jrgba(g, &text_color);
        jgraphics_stroke_preserve(g);
    }

    jgraphics_set_source_jrgba(g, &bg_color);
    jgraphics_fill(g);

    jgraphics_select_font_face(g, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(g, BUTTON_FONT_SIZE);
    jgraphics_set_source_jrgba(g, &text_color);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(g, &extents);

    double text_y = bounds.y + (bounds.height + extents.ascent - extents.descent) / 2;
    jgraphics_move_to(g, bounds.x + BUTTON_PADDING_X, text_y);
    jgraphics_text_path(g, CONFIRM_OK_BUTTON_TEXT);
    jgraphics_fill(g);
}

void presetter_draw_confirm_cancel_button(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    if (p->j_preset_confirm_status_text[0] == '\0' || !(p->j_confirm_store || p->j_confirm_delete))
        return;

    t_bounds bounds = presetter_get_confirm_cancel_button_bounds(p, rect);

    t_jrgba bg_color;
    t_jrgba bg_color_off;
    t_jrgba text_color;
    t_jrgba text_color_off;

    if (p->j_confirm_cancel_button_down) {
        jcolor_getcolor(CONFIRM_BUTTON_ON_BG_COLOR_SYM, &bg_color, &bg_color_off);
        jcolor_getcolor(CONFIRM_BUTTON_ON_TEXT_COLOR_SYM, &text_color, &text_color_off);
    } else {
        jcolor_getcolor(CONFIRM_BUTTON_UP_BG_COLOR_SYM, &bg_color, &bg_color_off);
        jcolor_getcolor(CONFIRM_BUTTON_UP_TEXT_COLOR_SYM, &text_color, &text_color_off);
    }

    jgraphics_rectangle(g, bounds.x, bounds.y, bounds.width, bounds.height);

    if (!p->j_confirm_cancel_button_down) {
        jgraphics_set_source_jrgba(g, &text_color);
        jgraphics_stroke_preserve(g);
    }

    jgraphics_set_source_jrgba(g, &bg_color);
    jgraphics_fill(g);

    jgraphics_select_font_face(g, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(g, BUTTON_FONT_SIZE);
    jgraphics_set_source_jrgba(g, &text_color);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(g, &extents);

    double text_y = bounds.y + (bounds.height + extents.ascent - extents.descent) / 2;
    jgraphics_move_to(g, bounds.x + BUTTON_PADDING_X, text_y);
    jgraphics_text_path(g, CONFIRM_CANCEL_BUTTON_TEXT);
    jgraphics_fill(g);
}

void presetter_draw_right_arrow(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_bounds bounds = presetter_get_right_arrow_bounds(p, rect);

    t_jrgba color;
    if (p->j_pagination_right_arrow_down) {
        presetter_hex_to_rgba(&color, PAGINATION_ARROW_DOWN_COLOR_HEX, 1);
    } else {
        presetter_hex_to_rgba(&color, PAGINATION_ARROW_ON_COLOR_HEX, 1);
    }

    jgraphics_set_source_jrgba(g, &color);
    jgraphics_select_font_face(g, PAGINATION_ARROW_FONT, JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(g, PAGINATION_ARROW_FONT_SIZE);

    jgraphics_move_to(g, bounds.x, bounds.y + bounds.height);
    jgraphics_text_path(g, PAGINATION_RIGHT_ARROW_TEXT);
    jgraphics_fill(g);
}

void presetter_draw_pagination_number(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_bounds bounds = presetter_get_pagination_number_bounds(p, rect);

    t_jrgba color;
    presetter_hex_to_rgba(&color, PAGINATION_ARROW_ON_COLOR_HEX, 1);
    jgraphics_set_source_jrgba(g, &color);
    jgraphics_select_font_face(g, PAGINATION_NUMBER_FONT, JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(g, PAGINATION_NUMBER_FONT_SIZE);

    char number_text[16];
    if (p->j_selected_tab == gensym("presets")) {
        snprintf_zero(number_text, sizeof(number_text), "%d", p->j_preset_pagination_number);
    } else {
        snprintf_zero(number_text, sizeof(number_text), "%d", p->j_filter_pagination_number);
    }

    jgraphics_move_to(g, bounds.x, bounds.y + bounds.height);
    jgraphics_text_path(g, number_text);
    jgraphics_fill(g);
}

void presetter_draw_left_arrow(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_bounds bounds = presetter_get_left_arrow_bounds(p, rect);

    t_jrgba color;
    if (p->j_pagination_left_arrow_down) {
        presetter_hex_to_rgba(&color, PAGINATION_ARROW_DOWN_COLOR_HEX, 1);
    } else if (p->j_selected_tab == gensym("presets") && p->j_preset_pagination_number > 1) {
        presetter_hex_to_rgba(&color, PAGINATION_ARROW_ON_COLOR_HEX, 1);
    } else if (p->j_selected_tab == gensym("filters") && p->j_filter_pagination_number > 1) {
        presetter_hex_to_rgba(&color, PAGINATION_ARROW_ON_COLOR_HEX, 1);
    } else {
        presetter_hex_to_rgba(&color, PAGINATION_ARROW_OFF_COLOR_HEX, 1);
    }

    jgraphics_set_source_jrgba(g, &color);
    jgraphics_select_font_face(g, PAGINATION_ARROW_FONT, JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(g, PAGINATION_ARROW_FONT_SIZE);

    jgraphics_move_to(g, bounds.x, bounds.y + bounds.height);
    jgraphics_text_path(g, PAGINATION_LEFT_ARROW_TEXT);
    jgraphics_fill(g);
}

void presetter_draw_tab_bar(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_bounds bounds = presetter_get_presets_tab_bounds(p, rect);

    t_jrgba bg_color;
    presetter_hex_to_rgba(&bg_color, TAB_BAR_COLOR_HEX, 1);

    jgraphics_rectangle(g, bounds.x, bounds.y, rect->width, bounds.height);
    jgraphics_set_source_jrgba(g, &bg_color);
    jgraphics_fill(g);
}

void presetter_draw_presets_tab(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_bounds bounds = presetter_get_presets_tab_bounds(p, rect);

    t_jrgba bg_color;
    t_jrgba bg_color_off;
    t_jrgba text_color;
    t_jrgba text_color_off;

    if (p->j_selected_tab == gensym("presets")) {
        jcolor_getcolor(TAB_PRESETS_ON_BG_COLOR_SYM, &bg_color, &bg_color_off);
        jcolor_getcolor(TAB_PRESETS_ON_TEXT_COLOR_SYM, &text_color, &text_color_off);
    } else {
        presetter_hex_to_rgba(&bg_color, TAB_PRESETS_UP_BG_COLOR_HEX, 1);
        presetter_hex_to_rgba(&text_color, TAB_PRESETS_UP_TEXT_COLOR_HEX, 1);
    }

    jgraphics_rectangle(g, bounds.x, bounds.y, bounds.width, bounds.height);
    jgraphics_set_source_jrgba(g, &bg_color);
    jgraphics_fill(g);

    jgraphics_select_font_face(g, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(g, TAB_FONT_SIZE);
    jgraphics_set_source_jrgba(g, &text_color);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(g, &extents);

    double text_y = bounds.y + (bounds.height + extents.ascent - extents.descent) / 2;
    jgraphics_move_to(g, bounds.x + TAB_PADDING_X, text_y);
    jgraphics_text_path(g, TAB_PRESETS_TEXT);
    jgraphics_fill(g);
}

void presetter_draw_filters_tab(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_bounds bounds = presetter_get_filters_tab_bounds(p, rect);

    t_jrgba bg_color;
    t_jrgba text_color;
    t_jrgba text_color_off;

    if (p->j_selected_tab == gensym("filters")) {
        presetter_hex_to_rgba(&bg_color, TAB_FILTERS_ON_BG_COLOR_HEX, 1);
        jcolor_getcolor(TAB_FILTERS_ON_TEXT_COLOR_SYM, &text_color, &text_color_off);
    } else {
        presetter_hex_to_rgba(&bg_color, TAB_FILTERS_UP_BG_COLOR_HEX, 1);
        presetter_hex_to_rgba(&text_color, TAB_FILTERS_UP_TEXT_COLOR_HEX, 1);
    }

    jgraphics_rectangle(g, bounds.x, bounds.y, bounds.width, bounds.height);
    jgraphics_set_source_jrgba(g, &bg_color);
    jgraphics_fill(g);

    jgraphics_select_font_face(g, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(g, TAB_FONT_SIZE);
    jgraphics_set_source_jrgba(g, &text_color);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(g, &extents);

    double text_y = bounds.y + (bounds.height + extents.ascent - extents.descent) / 2;
    jgraphics_move_to(g, bounds.x + TAB_PADDING_X, text_y);
    jgraphics_text_path(g, TAB_FILTERS_TEXT);
    jgraphics_fill(g);
}

/** Filters **/

void presetter_draw_filter_grid_row(
    t_presetter *p, t_jgraphics *g, t_grid_dim *dim, int row_idx, double cell_width, double y
) {
    for (int i = 0; i < dim->columns; i++) {
        double x = (double)i * (cell_width + FILTER_GRID_PADDING) + 8.5;
        int cell_idx = (i + dim->columns * row_idx + 1) + presetter_get_filter_grid_offset(p, dim);

        t_jrgba color;
        if (cell_idx == p->j_hovered_filter_cell) {
            presetter_hex_to_rgba(&color, "#2A2A2A", 1);
        } else {
            presetter_hex_to_rgba(&color, "#222222", 1);
        }

        jgraphics_rectangle_rounded(g, x, y, cell_width, FILTER_CELL_HEIGHT, 5, 5);
        jgraphics_set_source_jrgba(g, &color);
        jgraphics_fill(g);

        jgraphics_select_font_face(g, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
        jgraphics_set_font_size(g, 10);

        t_jgraphics_font_extents extents;
        jgraphics_font_extents(g, &extents);

        t_jrgba stroke_color;
        t_jrgba stroke_color_off;
        jcolor_getcolor(gensym("live_arranger_grid_tiles"), &stroke_color, &stroke_color_off);

        t_symbol *name = NULL;
        t_dictionary *dict = presetter_lookup_filter_slot(p, cell_idx);

        if (dict && dictionary_getsym(dict, gensym("name"), &name) == MAX_ERR_NONE && name) {

            double max_width = cell_width - 24;
            double text_width;
            double text_height;
            char visible[1048];
            char with_ellipsis[1048];

            jgraphics_text_measure(g, name->s_name, &text_width, &text_height);

            if (text_width > max_width) {
                snprintf_zero(visible, sizeof(visible), "%s", name->s_name);
                snprintf_zero(with_ellipsis, sizeof(with_ellipsis), "%s...", visible);

                jgraphics_text_measure(g, with_ellipsis, &text_width, &text_height);

                while (text_width > max_width && strlen(visible) > 0) {
                    visible[strlen(visible) - 1] = '\0';
                    snprintf_zero(with_ellipsis, sizeof(with_ellipsis), "%s...", visible);
                    jgraphics_text_measure(g, with_ellipsis, &text_width, &text_height);
                }

                snprintf_zero(visible, sizeof(visible), "%s...", visible);
            } else {
                snprintf_zero(visible, sizeof(visible), "%s", name->s_name);
            }

            long val = 0;
            hashtab_lookuplong(p->j_applied_filters, presetter_long_to_sym(cell_idx), &val);

            t_jrgba text_color;
            t_jrgba text_color_off;

            if (val) {
                presetter_hex_to_rgba(&text_color, PATCHER_OBJECT_COLOR_HEX, 1);
            } else if (p->j_selected_filter_cell == cell_idx) {
                jcolor_getcolor(gensym("live_arranger_grid_tiles"), &text_color, &text_color_off);
            } else {
                presetter_hex_to_rgba(&text_color, "#999999", 1);
            }

            jgraphics_set_source_jrgba(g, &text_color);

            double text_y = y + (FILTER_CELL_HEIGHT + extents.ascent - extents.descent) / 2;
            jgraphics_move_to(g, x + 18, text_y);
            jgraphics_text_path(g, visible);
            jgraphics_fill(g);

            t_jrgba circle_color;
            t_jrgba circle_color_off;

            if (val) {
                presetter_hex_to_rgba(&circle_color, PATCHER_OBJECT_COLOR_HEX, 1);
            } else if (p->j_selected_filter_cell == cell_idx) {
                jcolor_getcolor(gensym("live_arranger_grid_tiles"), &circle_color, &circle_color_off);
            } else {
                presetter_hex_to_rgba(&circle_color, "#999999", 1);
            }

            jgraphics_set_source_jrgba(g, &circle_color);
            jgraphics_ellipse(g, x + 6, y + ((double)FILTER_CELL_HEIGHT - 8) / 2, 8, 8);
            jgraphics_fill(g);

            if (val) {
                presetter_hex_to_rgba(&stroke_color, PATCHER_OBJECT_COLOR_HEX, 1);
            }
        } else {
            t_jrgba circle_color;
            presetter_hex_to_rgba(&circle_color, "#555555", 1);
            jgraphics_set_source_jrgba(g, &circle_color);
            jgraphics_ellipse(g, x + 6, y + ((double)FILTER_CELL_HEIGHT - 8) / 2, 8, 8);
            jgraphics_fill(g);

            t_jrgba text_color;
            presetter_hex_to_rgba(&text_color, "#555555", 1);
            presetter_hex_to_rgba(&stroke_color, "#555555", 1);

            double text_y = y + (FILTER_CELL_HEIGHT + extents.ascent - extents.descent) / 2;
            jgraphics_set_source_jrgba(g, &text_color);
            jgraphics_move_to(g, x + 18, text_y);
            jgraphics_text_path(g, "Empty");
            jgraphics_fill(g);
        }

        if (p->j_selected_filter_cell == cell_idx) {
            jgraphics_rectangle_rounded(g, x, y, cell_width, FILTER_CELL_HEIGHT, 5, 5);
            jgraphics_set_source_jrgba(g, &stroke_color);
            jgraphics_stroke(g);
        }
    }
}

void presetter_draw_write_filter_button(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_bounds bounds = presetter_get_write_filter_button_bounds(p, rect);

    t_jrgba bg_color;
    t_jrgba text_color;
    t_jrgba bg_color_off;
    t_jrgba text_color_off;

    if (p->j_write_filter_button_down) {
        jcolor_getcolor(WRITE_BUTTON_ON_BG_COLOR_SYM, &bg_color, &bg_color_off);
        jcolor_getcolor(WRITE_BUTTON_ON_TEXT_COLOR_SYM, &text_color, &text_color_off);

    } else if (p->j_editing_filter_name) {
        jcolor_getcolor(WRITE_BUTTON_UP_BG_COLOR_SYM, &bg_color, &bg_color_off);
        jcolor_getcolor(WRITE_BUTTON_UP_TEXT_COLOR_SYM, &text_color, &text_color_off);

    } else {
        presetter_hex_to_rgba(&bg_color, WRITE_BUTTON_INACTIVE_BG_COLOR_HEX, 1);
        presetter_hex_to_rgba(&text_color, WRITE_BUTTON_INACTIVE_TEXT_COLOR_HEX, 1);
    }

    jgraphics_rectangle(g, bounds.x, bounds.y, bounds.width, bounds.height);

    if (!p->j_write_button_down) {
        jgraphics_set_source_jrgba(g, &text_color);
        jgraphics_stroke_preserve(g);
    }

    jgraphics_set_source_jrgba(g, &bg_color);
    jgraphics_fill(g);

    jgraphics_select_font_face(g, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(g, BUTTON_FONT_SIZE);
    jgraphics_set_source_jrgba(g, &text_color);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(g, &extents);

    double text_y = bounds.y + (bounds.height + extents.ascent - extents.descent) / 2;
    jgraphics_move_to(g, bounds.x + BUTTON_PADDING_X, text_y);
    jgraphics_text_path(g, WRITE_FILTER_BUTTON_TEXT);
    jgraphics_fill(g);
}

void presetter_draw_filter_grid(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_grid_dim dim;
    t_bounds pgbounds = presetter_get_preset_grid_bounds(p, rect);

    dim.columns = (long)floor(rect->width / FILTER_CELL_MIN_WIDTH);
    if (dim.columns == 0) {
        dim.columns = 1;
    }

    dim.rows = (long)floor(pgbounds.height / (FILTER_CELL_HEIGHT + FILTER_GRID_PADDING));
    if (dim.rows == 0) {
        dim.rows = 1;
    }

    double cell_width = (rect->width - 8.5 * 2 - (FILTER_GRID_PADDING * (dim.columns - 1))) / dim.columns;

    for (int i = 0; i < dim.rows; i++) {
        double y = pgbounds.y + 3 + i * (FILTER_CELL_HEIGHT + FILTER_GRID_PADDING);
        presetter_draw_filter_grid_row(p, g, &dim, i, cell_width, y);
    }
}

/* Main Paint */

void presetter_paint(t_presetter *p, t_object *patcherview) {
    t_jgraphics *g = (t_jgraphics *)patcherview_get_jgraphics(patcherview);

    t_rect rect;
    jbox_get_rect_for_view((t_object *)p, patcherview, &rect);

    t_jrgba color;
    presetter_hex_to_rgba(&color, BG_COLOR_HEX, 1);

    jgraphics_rectangle_rounded(g, 0, 0, rect.width, rect.height, 4, 4);
    jgraphics_set_source_jrgba(g, &color);
    jgraphics_fill(g);

    presetter_draw_write_name(p, g, &rect);

    if (p->j_selected_tab == gensym("presets")) {
        presetter_draw_write_button(p, g, &rect);
        presetter_draw_preset_grid(p, g, &rect);
        presetter_draw_confirm_ok_button(p, g, &rect);
        presetter_draw_confirm_cancel_button(p, g, &rect);
    } else {
        presetter_draw_write_filter_button(p, g, &rect);
        presetter_draw_filter_grid(p, g, &rect);
    }

    presetter_draw_status(p, g, &rect);
    presetter_draw_right_arrow(p, g, &rect);
    presetter_draw_pagination_number(p, g, &rect);
    presetter_draw_left_arrow(p, g, &rect);

    presetter_draw_tab_bar(p, g, &rect);
    presetter_draw_presets_tab(p, g, &rect);
    presetter_draw_filters_tab(p, g, &rect);
}
