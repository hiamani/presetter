#include "ext.h"
#include "ext_hashtab.h"
#include "ext_mess.h"
#include "ext_obex.h"
#include "ext_obex_util.h"
#include "ext_proto.h"
#include "jgraphics.h"
#include "jpatcher_api.h"
#include "math.h"
#include <string.h>

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
#define STATUS_CONFIRM_TEXT_COLOR_HEX PATCHER_OBJECT_COLOR_HEX

// Refresh
#define REFRESH_ON_COLOR_HEX PATCHER_OBJECT_COLOR_HEX
#define REFRESH_OFF_COLOR_HEX "#333333"

// Confirm
#define CONFIRM_BUTTON_UP_BG_COLOR_SYM gensym("live_contrast_frame")
#define CONFIRM_BUTTON_UP_TEXT_COLOR_HEX PATCHER_OBJECT_COLOR_HEX
#define CONFIRM_BUTTON_ON_BG_COLOR_HEX PATCHER_OBJECT_COLOR_HEX
#define CONFIRM_BUTTON_ON_TEXT_COLOR_SYM gensym("live_contrast_frame")

/* Dimensions */

// Grid
#define GRID_OFFSET_X 5
#define GRID_OFFSET_Y 3
#define CELL_SIZE 7.0
#define CELL_PADDING 3.5
#define CELL_TOTAL_SIZE (CELL_SIZE + CELL_PADDING)

// Preset Name
#define PRESET_NAME_OFFSET_X (GRID_OFFSET_X + CELL_PADDING)
#define PRESET_NAME_OFFSET_Y 9
#define PRESET_NAME_MARGIN_RIGHT 4

// Buttons
#define BUTTON_PADDING_X 4
#define BUTTON_PADDING_Y 1

// Write Button
#define WRITE_BUTTON_TEXT "WRITE NAME"
#define WRITE_BUTTON_OFFSET_X (GRID_OFFSET_X + CELL_PADDING)
#define WRITE_BUTTON_OFFSET_Y 8

// Status
#define STATUS_OFFSET_X 8
#define STATUS_OFFSET_Y 4

// Confirm
#define CONFIRM_OK_BUTTON_TEXT "OK"
#define CONFIRM_OK_BUTTON_MARGIN_LEFT 12
#define CONFIRM_CANCEL_BUTTON_TEXT "CANCEL"
#define CONFIRM_CANCEL_BUTTON_MARGIN_LEFT 8
#define CONFIRM_BUTTON_OFFSET_Y 5

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

typedef struct _presetter {
    // Base
    t_jbox j_box;

    // Inlets
    long j_inlet_num;

    // Outlets
    void *j_outlet1;

    // Attributes
    t_symbol *j_pattrstorage_name;

    // pattrstorage
    t_object *j_pattrstorage;

    // Slots
    t_hashtab *j_slots;

    // Cells
    long j_hovered_cell;
    long j_last_hovered_cell;
    long j_selected_cell;

    // Preset Name
    bool j_editing_name;
    char j_preset_name[512];
    long j_preset_name_font_size;
    double j_preset_name_height;

    // Write Button
    char *j_write_button_text;
    long j_button_font_size;
    double j_write_button_width;
    double j_write_button_height;
    bool j_write_button_down;

    // Status line
    t_status j_status;
    t_status j_status_override;
    char j_idle_status_text[512];
    char j_hover_status_text[1024];
    char j_confirm_status_text[512];
    long j_status_font_size;
    double j_status_height;

    // Confirm
    long j_confirm_cell;
    bool j_confirm_store;
    bool j_confirm_delete;

    double j_confirm_ok_button_width;
    double j_confirm_ok_button_height;
    bool j_confirm_ok_button_down;

    double j_confirm_cancel_button_width;
    double j_confirm_cancel_button_height;
    bool j_confirm_cancel_button_down;
} t_presetter;

// -----------------------------------------------------------------------------
// Forward Declarations
// -----------------------------------------------------------------------------

// Init
t_presetter *presetter_new(t_symbol *s, short argc, t_atom *argv);
void presetter_free(t_presetter *p);

// Attributes
t_max_err presetter_set_pattrstorage(t_presetter *p, t_object *attr, long argc, t_atom *argv);

// Messages
void presetter_loadbang(t_presetter *p, long action);
void presetter_bang(t_presetter *p);
void presetter_assist(t_presetter *x, void *b, long io, long index, char *s);

void presetter_read(t_presetter *p, t_symbol *s, long argc, t_atom *argv);
void presetter_slotname(t_presetter *p, t_symbol *s, long argc, t_atom *argv);
void presetter_anything(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

// Pointer events
void presetter_mousedown(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers);
void presetter_mouseup(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers);
void presetter_mousemove(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers);
void presetter_mouseleave(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers);

// Keyboard events
long presetter_key(t_presetter *p, t_object *patcherview, long keycode, long modifiers, long textcharacter);

// Drawing
void presetter_paint(t_presetter *p, t_object *patcherview);

// -----------------------------------------------------------------------------
// Main entry
// -----------------------------------------------------------------------------

static t_class *s_presetter_class = NULL;

//

void ext_main(void *r) {
    t_class *c;

    c = class_new(
        "presetter",
        (method)presetter_new,
        (method)presetter_free,
        sizeof(t_presetter),
        (method)NULL,
        A_GIMME,
        0
    );

    c->c_flags |= CLASS_FLAG_NEWDICTIONARY;

    jbox_initclass(c, 0);

    // Methods

    class_addmethod(c, (method)presetter_bang, "bang", 0);
    class_addmethod(c, (method)presetter_loadbang, "loadbang", 0);
    class_addmethod(c, (method)presetter_assist, "assist", A_CANT, 0);

    class_addmethod(c, (method)presetter_read, "read", A_GIMME, 0);
    class_addmethod(c, (method)presetter_slotname, "slotname", A_GIMME, 0);
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

    CLASS_ATTR_DEFAULT(c, "patching_rect", 0, "0. 0. 255. 153.");

    class_register(CLASS_BOX, c);
    s_presetter_class = c;
}

// -----------------------------------------------------------------------------
// Lifecycle methods
// -----------------------------------------------------------------------------

/* Element Measurements */

// Generic Button Dimensions

void presetter_get_init_button_dim(t_presetter *p, char *text, double *outwidth, double *outheight) {
    t_jsurface *surface = jgraphics_image_surface_create(JGRAPHICS_FORMAT_A8, 1, 1);
    t_jgraphics *g = jgraphics_create(surface);

    jgraphics_select_font_face(g, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(g, p->j_button_font_size);

    double text_width;
    double text_height;
    jgraphics_text_measure(g, text, &text_width, &text_height);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(g, &extents);

    *outwidth = text_width + BUTTON_PADDING_X * 2;
    *outheight = extents.ascent + extents.descent + BUTTON_PADDING_Y * 2;

    jgraphics_destroy(g);
    jgraphics_surface_destroy(surface);
}

// Dimension Definitions

void presetter_init_preset_name_dim(t_presetter *p) {
    t_jsurface *surface = jgraphics_image_surface_create(JGRAPHICS_FORMAT_A8, 1, 1);
    t_jgraphics *g = jgraphics_create(surface);

    jgraphics_select_font_face(g, "Menlo", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(g, p->j_preset_name_font_size);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(g, &extents);
    p->j_preset_name_height = extents.height;

    jgraphics_destroy(g);
    jgraphics_surface_destroy(surface);
}

void presetter_init_write_button_dim(t_presetter *p) {
    presetter_get_init_button_dim(p, WRITE_BUTTON_TEXT, &p->j_write_button_width, &p->j_write_button_height);
}

void presetter_init_status_dim(t_presetter *p) {
    t_jsurface *surface = jgraphics_image_surface_create(JGRAPHICS_FORMAT_A8, 1, 1);
    t_jgraphics *g = jgraphics_create(surface);

    jgraphics_select_font_face(g, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(g, p->j_status_font_size);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(g, &extents);
    p->j_status_height = extents.height;

    jgraphics_destroy(g);
    jgraphics_surface_destroy(surface);
}

void presetter_init_confirm_ok_button_dim(t_presetter *p) {
    presetter_get_init_button_dim(
        p, CONFIRM_OK_BUTTON_TEXT,
        &p->j_confirm_ok_button_width,
        &p->j_confirm_ok_button_height
    );
}

void presetter_init_confirm_cancel_button_dim(t_presetter *p) {
    presetter_get_init_button_dim(
        p, CONFIRM_CANCEL_BUTTON_TEXT,
        &p->j_confirm_cancel_button_width,
        &p->j_confirm_cancel_button_height
    );
}

/* Jbox Init */

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

        // Inlets
        p->j_box.b_firstin = (t_object *)p;

        // Outlets
        p->j_outlet1 = outlet_new((t_object *)p, NULL);

        // Slots
        p->j_slots = hashtab_new(0);

        // Grid
        p->j_selected_cell = -1;
        p->j_hovered_cell = -1;
        p->j_last_hovered_cell = -2;

        // Preset Name
        p->j_editing_name = false;
        p->j_preset_name_font_size = 10;
        presetter_init_preset_name_dim(p);

        // Write Button
        p->j_button_font_size = 9;
        p->j_write_button_down = false;
        presetter_init_write_button_dim(p);

        // Status
        p->j_status = PRESETTER_NO_STATUS;
        p->j_status_override = PRESETTER_NO_STATUS;
        p->j_status_font_size = 9;
        presetter_init_status_dim(p);

        // Confirm
        p->j_confirm_cell = -1;
        p->j_confirm_store = false;
        p->j_confirm_delete = false;

        presetter_init_confirm_ok_button_dim(p);
        p->j_confirm_ok_button_down = false;

        presetter_init_confirm_cancel_button_dim(p);
        p->j_confirm_cancel_button_down = false;

        attr_dictionary_process(p, d);
        jbox_ready(&p->j_box);
    }

    return p;
}

void presetter_free(t_presetter *p) {
    hashtab_clear(p->j_slots);
    jbox_free((t_jbox *)p);
}

// -----------------------------------------------------------------------------
// Utility Methods
// -----------------------------------------------------------------------------

/* Colors Utilities */

void presetter_hex_to_rgba(t_jrgba *color, const char *hex, double alpha) {
    unsigned int r, g, b;
    const char *h = (hex[0] == '#') ? hex + 1 : hex;
    sscanf(h, "%2x%2x%2x", &r, &g, &b);
    jrgba_set(color, r / 255.0, g / 255.0, b / 255.0, alpha);
}

/* Hashtab Utilities */

t_symbol *presetter_lookup_slot(t_presetter *p, long index) {
    t_symbol *name = NULL;
    char key[24];
    snprintf_zero(key, sizeof(key), "%ld", index);
    hashtab_lookup(p->j_slots, gensym(key), (t_object **)&name);
    return name;
}

/* Text Measurement (inside of paint) */

void presetter_measure_text(t_jgraphics *g, char *text, double *outwidth, double *outheight) {
    jgraphics_select_font_face(g, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(g, 9);
    jgraphics_text_measure(g, text, outwidth, outheight);
}

/* Text Measurement (outside of paint) */

void presetter_measure_surface_text(t_presetter *p, char *text, double *outwidth, double *outheight) {
    t_jsurface *surface = jgraphics_image_surface_create(JGRAPHICS_FORMAT_A8, 1, 1);
    t_jgraphics *g = jgraphics_create(surface);

    jgraphics_select_font_face(g, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(g, 9);
    jgraphics_text_measure(g, text, outwidth, outheight);

    jgraphics_destroy(g);
    jgraphics_surface_destroy(surface);
}

/* Find pattrstorage */

t_object *presetter_find_pattrstorage(t_presetter *p) {
    if (p->j_pattrstorage) {
        return p->j_pattrstorage;
    }

    if (!p->j_pattrstorage_name || p->j_pattrstorage_name == gensym(""))
        return NULL;

    t_object *patcher = NULL;
    object_obex_lookup(p, gensym("#P"), &patcher);
    if (!patcher)
        return NULL;

    t_object *box = (t_object *)object_method(
        patcher,
        gensym("getnamedbox"),
        p->j_pattrstorage_name
    );

    if (box) {
        t_object *ps = jbox_get_object(box);
        p->j_pattrstorage = ps;
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
        p->j_pattrstorage = NULL; // clear cached pointer

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

void presetter_slotname(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    long inlet = proxy_getinlet((t_object *)p);
    if (inlet != 0)
        return;

    if (atom_gettype(argv) == A_LONG && atom_getlong(argv) == 0) {
        hashtab_clear(p->j_slots);
        return;
    }

    if (atom_gettype(argv) == A_SYM && atom_getsym(argv) == gensym("done")) {
        jbox_redraw((t_jbox *)p);
        if (!presetter_lookup_slot(p, p->j_selected_cell)) {
            p->j_selected_cell = -1;
        }
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

    char key[24];
    snprintf_zero(key, sizeof(key), "%ld", index);
    hashtab_store(p->j_slots, gensym(key), (t_object *)name);
}

// Pass through unknown messages silently
void presetter_anything(t_presetter *p, t_symbol *s, long argc, t_atom *argv) {
    return;
}

// -----------------------------------------------------------------------------
// Callback Methods
// -----------------------------------------------------------------------------

void presetter_handle_recall(t_presetter *p, long cell_idx, bool update_status) {
    t_object *ps = presetter_find_pattrstorage(p);
    if (!ps)
        return;

    t_symbol *slot = presetter_lookup_slot(p, cell_idx);

    if (slot) {
        p->j_selected_cell = cell_idx;
        snprintf_zero(p->j_preset_name, sizeof(p->j_preset_name), "%s", slot->s_name);

        if (update_status) {
            snprintf_zero(p->j_idle_status_text, sizeof(p->j_idle_status_text), "Selected Preset %d", cell_idx);
        }

        p->j_status = PRESETTER_IDLE_STATUS;

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

    snprintf_zero(p->j_idle_status_text, sizeof(p->j_idle_status_text), "Stored Preset %d", cell_idx);
    p->j_status = PRESETTER_IDLE_STATUS;

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

    snprintf_zero(p->j_idle_status_text, sizeof(p->j_idle_status_text), "Deleted Preset %d", cell_idx);
    p->j_status = PRESETTER_IDLE_STATUS;

    if (p->j_selected_cell == cell_idx) {
        p->j_preset_name[0] = '\0';
    }

    p->j_selected_cell = -1;

    t_atom a;
    atom_setlong(&a, cell_idx);
    object_method_typed(ps, gensym("delete"), 1, &a, NULL);
    object_method_typed(ps, gensym("getslotnamelist"), 0, NULL, NULL);
    outlet_bang(p->j_outlet1);
    jbox_redraw((t_jbox *)p);
}

void presetter_handle_rename(t_presetter *p) {
    t_object *ps = presetter_find_pattrstorage(p);
    if (!ps)
        return;

    snprintf_zero(p->j_idle_status_text, sizeof(p->j_idle_status_text), "Renamed Preset %d", p->j_selected_cell);
    p->j_editing_name = false;

    t_atom args[2];
    atom_setlong(&args[0], p->j_selected_cell);
    atom_setsym(&args[1], gensym(p->j_preset_name));

    object_method_typed(ps, gensym("slotname"), 2, args, NULL);
    object_method_typed(ps, gensym("store"), 1, args, NULL);
    object_method_typed(ps, gensym("getslotnamelist"), 0, NULL, NULL);

    outlet_bang(p->j_outlet1);

    jbox_redraw((t_jbox *)p);
}

// -----------------------------------------------------------------------------
// Element Bounds
// -----------------------------------------------------------------------------

/* Write Button Bounds */

t_bounds presetter_get_write_button_bounds(t_presetter *p, t_rect *rect) {
    t_bounds bounds;
    bounds.x = rect->width - p->j_write_button_width - WRITE_BUTTON_OFFSET_X;
    bounds.y = WRITE_BUTTON_OFFSET_Y;
    bounds.width = p->j_write_button_width;
    bounds.height = p->j_write_button_height;
    return bounds;
}

bool presetter_in_write_button_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_write_button_bounds(p, rect);

    bool in_x = pt->x >= bounds.x && pt->x <= bounds.width + bounds.x;
    bool in_y = pt->y >= bounds.y && pt->y <= bounds.height + bounds.y;

    return in_x && in_y;
}

/* Preset Name Bounds */

t_bounds presetter_get_preset_name_bounds(t_presetter *p, t_rect *rect) {
    t_bounds wbbounds = presetter_get_write_button_bounds(p, rect);

    t_bounds bounds;
    bounds.x = PRESET_NAME_OFFSET_X;
    bounds.y = PRESET_NAME_OFFSET_Y;
    bounds.width = rect->width -
                   wbbounds.width -
                   PRESET_NAME_OFFSET_X -
                   WRITE_BUTTON_OFFSET_X -
                   PRESET_NAME_MARGIN_RIGHT;
    bounds.height = p->j_preset_name_height;
    return bounds;
}

bool presetter_in_preset_name_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_preset_name_bounds(p, rect);

    bool in_x = pt->x >= bounds.x && pt->x <= bounds.width + bounds.x;
    bool in_y = pt->y >= bounds.y && pt->y <= bounds.height + bounds.y;

    return in_x && in_y;
}

/* Grid Bounds */

t_grid_dim presetter_get_grid_dim(t_presetter *p, t_rect *rect) {
    t_grid_dim dim;
    dim.columns = (int)floor((rect->width - CELL_PADDING - GRID_OFFSET_X) / CELL_TOTAL_SIZE);

    double grid_y_pos = PRESET_NAME_OFFSET_Y + p->j_preset_name_height + GRID_OFFSET_Y;
    double grid_space = rect->height -
                        grid_y_pos -
                        p->j_status_height -
                        STATUS_OFFSET_Y -
                        CELL_PADDING;
    dim.rows = (int)floor(grid_space / CELL_TOTAL_SIZE);
    if (dim.rows < 0)
        dim.rows = 0;

    return dim;
}

t_bounds presetter_get_grid_bounds(t_presetter *p, t_rect *rect) {
    t_bounds pnbounds = presetter_get_preset_name_bounds(p, rect);

    t_bounds bounds;
    t_grid_dim dim = presetter_get_grid_dim(p, rect);

    bounds.x = GRID_OFFSET_X;
    bounds.y = pnbounds.y + pnbounds.height + GRID_OFFSET_Y;
    bounds.width = dim.columns * CELL_TOTAL_SIZE + CELL_PADDING / 2;
    bounds.height = dim.rows * CELL_TOTAL_SIZE + CELL_PADDING / 2;

    return bounds;
}

bool presetter_in_grid_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_grid_bounds(p, rect);

    bool in_x = pt->x >= bounds.x + CELL_PADDING && pt->x <= bounds.width + bounds.x;
    bool in_y = pt->y >= bounds.y + CELL_PADDING && pt->y <= bounds.height + bounds.y;

    return in_x && in_y;
}

/* Grid Utilities */

t_cell_pos presetter_get_cell_pos(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_grid_bounds(p, rect);

    t_cell_pos pos;
    double rel_x = pt->x - bounds.x - CELL_PADDING / 2;
    double rel_y = pt->y - bounds.y - CELL_PADDING / 2;
    pos.x = (int)floor(rel_x / CELL_TOTAL_SIZE);
    pos.y = (int)floor(rel_y / CELL_TOTAL_SIZE);
    return pos;
}

/* Status Bounds */

t_bounds presetter_get_status_bounds(t_presetter *p, t_rect *rect) {
    t_bounds gbounds = presetter_get_grid_bounds(p, rect);

    t_bounds bounds;
    bounds.x = STATUS_OFFSET_X;
    bounds.y = gbounds.y + gbounds.height + STATUS_OFFSET_Y;
    bounds.width = rect->width - STATUS_OFFSET_X * 2;
    bounds.height = p->j_status_height;
    return bounds;
}

bool presetter_in_preset_status_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_status_bounds(p, rect);

    bool in_x = pt->x >= bounds.x && pt->x <= bounds.width + bounds.x;
    bool in_y = pt->y >= bounds.y && pt->y <= bounds.height + bounds.y;

    return in_x && in_y;
}

/* Confirm Ok Button Bounds */

t_bounds presetter_get_confirm_ok_button_bounds(t_presetter *p, t_rect *rect, t_jgraphics *g) {
    t_bounds gbounds = presetter_get_grid_bounds(p, rect);

    double text_width;
    double text_height;

    if (g) {
        presetter_measure_text(g, p->j_confirm_status_text, &text_width, &text_height);
    } else {
        presetter_measure_surface_text(p, p->j_confirm_status_text, &text_width, &text_height);
    }

    t_bounds bounds;
    bounds.x = text_width + CONFIRM_OK_BUTTON_MARGIN_LEFT;
    bounds.y = gbounds.y + gbounds.height + CONFIRM_BUTTON_OFFSET_Y;
    bounds.width = p->j_confirm_ok_button_width;
    bounds.height = p->j_confirm_ok_button_height;
    return bounds;
}

bool presetter_in_confirm_ok_button_bounds(t_presetter *p, t_rect *rect, t_pt *pt, t_jgraphics *g) {
    t_bounds bounds = presetter_get_confirm_ok_button_bounds(p, rect, g);

    bool in_x = pt->x >= bounds.x && pt->x <= bounds.width + bounds.x;
    bool in_y = pt->y >= bounds.y && pt->y <= bounds.height + bounds.y;

    return in_x && in_y;
}

t_bounds presetter_get_confirm_cancel_button_bounds(t_presetter *p, t_rect *rect, t_jgraphics *g) {
    t_bounds cobounds = presetter_get_confirm_ok_button_bounds(p, rect, g);

    t_bounds bounds;
    bounds.x = cobounds.x + cobounds.width + CONFIRM_CANCEL_BUTTON_MARGIN_LEFT;
    bounds.y = cobounds.y;
    bounds.width = p->j_confirm_cancel_button_width;
    bounds.height = p->j_confirm_cancel_button_height;
    return bounds;
}

bool presetter_in_confirm_cancel_button_bounds(t_presetter *p, t_rect *rect, t_pt *pt, t_jgraphics *g) {
    t_bounds bounds = presetter_get_confirm_cancel_button_bounds(p, rect, g);

    bool in_x = pt->x >= bounds.x && pt->x <= bounds.width + bounds.x;
    bool in_y = pt->y >= bounds.y && pt->y <= bounds.height + bounds.y;

    return in_x && in_y;
}

// -----------------------------------------------------------------------------
// Pointer Event Methods
// -----------------------------------------------------------------------------

void presetter_clear_confirm(t_presetter *p) {
    p->j_confirm_cell = -1;
    p->j_confirm_store = false;
    p->j_confirm_delete = false;
    p->j_status_override = PRESETTER_NO_STATUS;
    p->j_confirm_status_text[0] = '\0';
    p->j_confirm_ok_button_down = false;
    p->j_confirm_cancel_button_down = false;
}

void presetter_mousedown(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers) {
    t_rect rect;
    jbox_get_rect_for_view((t_object *)p, patcherview, &rect);

    if (presetter_in_preset_name_bounds(p, &rect, &pt)) {
        if (p->j_selected_cell != -1) {
            p->j_editing_name = !p->j_editing_name;
            presetter_clear_confirm(p);
            jbox_redraw((t_jbox *)p);
        }
        return;
    }

    if (presetter_in_write_button_bounds(p, &rect, &pt)) {
        if (p->j_editing_name) {
            p->j_write_button_down = true;
            jbox_redraw((t_jbox *)p);
            return;
        }
    }

    if (presetter_in_grid_bounds(p, &rect, &pt)) {
        t_grid_dim dim = presetter_get_grid_dim(p, &rect);
        t_cell_pos pos = presetter_get_cell_pos(p, &rect, &pt);
        long cell_idx = pos.x + dim.columns * pos.y + 1;

        if (modifiers & eLeftButton &&
            (modifiers & eShiftKey) &&
            ((modifiers & eCommandKey) || (modifiers & eControlKey)) &&
            !(modifiers & eAltKey)) {
            p->j_editing_name = false;
            p->j_confirm_delete = true;
            p->j_confirm_cell = cell_idx;
            p->j_status_override = PRESETTER_CONFIRM_STATUS;
            snprintf_zero(p->j_confirm_status_text, sizeof(p->j_confirm_status_text), "Delete Preset %d?", cell_idx);
            jbox_redraw((t_jbox *)p);
            return;
        }

        if (modifiers & eLeftButton &&
            !(modifiers & eShiftKey) &&
            // Allow for interaction in unlocked patches
            // !(modifiers & eCommandKey) &&
            // !(modifiers & eControlKey) &&
            !(modifiers & eAltKey)) {
            presetter_clear_confirm(p);
            presetter_handle_recall(p, cell_idx, true);
            jbox_redraw((t_jbox *)p);
            return;
        }

        if (modifiers & eLeftButton &&
            modifiers & eShiftKey &&
            // Allow for interaction in unlocked patches
            // !(modifiers & eCommandKey) &&
            // !(modifiers & eControlKey) &&
            !(modifiers & eAltKey)) {
            p->j_editing_name = false;
            p->j_confirm_store = true;
            p->j_confirm_cell = cell_idx;
            p->j_status_override = PRESETTER_CONFIRM_STATUS;
            snprintf_zero(p->j_confirm_status_text, sizeof(p->j_confirm_status_text), "Store Preset %d?", cell_idx);
            jbox_redraw((t_jbox *)p);
            return;
        }
    }

    if ((p->j_confirm_store || p->j_confirm_delete) && p->j_confirm_status_text[0] != '\0' && p->j_confirm_cell != -1) {
        if (presetter_in_confirm_ok_button_bounds(p, &rect, &pt, NULL)) {
            p->j_confirm_ok_button_down = true;
        }

        if (presetter_in_confirm_cancel_button_bounds(p, &rect, &pt, NULL)) {
            p->j_confirm_cancel_button_down = true;
        }
    }
}

void presetter_mouseup(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers) {
    if (p->j_write_button_down) {
        presetter_handle_rename(p);
        p->j_write_button_down = false;
        jbox_redraw((t_jbox *)p);
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
}

void presetter_mousemove(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers) {
    t_rect rect;
    jbox_get_rect_for_view((t_object *)p, patcherview, &rect);

    if (presetter_in_grid_bounds(p, &rect, &pt)) {
        t_grid_dim dim = presetter_get_grid_dim(p, &rect);
        t_cell_pos pos = presetter_get_cell_pos(p, &rect, &pt);

        p->j_hovered_cell = pos.x + dim.columns * pos.y + 1;

        t_symbol *name = presetter_lookup_slot(p, p->j_hovered_cell);

        if (name && name != gensym("<(unnamed)>")) {
            snprintf_zero(
                p->j_hover_status_text, sizeof(p->j_hover_status_text),
                "Preset %d: %s", p->j_hovered_cell, name->s_name
            );
        } else {
            snprintf_zero(
                p->j_hover_status_text, sizeof(p->j_hover_status_text),
                "Preset %d", p->j_hovered_cell
            );
        }

        if (p->j_status == PRESETTER_HOVER_STATUS || p->j_hovered_cell != p->j_last_hovered_cell) {
            p->j_status = PRESETTER_HOVER_STATUS;
        }

        p->j_last_hovered_cell = p->j_hovered_cell;
    } else {
        p->j_status = PRESETTER_IDLE_STATUS;
        p->j_hovered_cell = -1;
        p->j_last_hovered_cell = -2;
    }

    jbox_redraw((t_jbox *)p);
}

void presetter_mouseleave(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers) {
    p->j_hovered_cell = -1;
    p->j_last_hovered_cell = -2;
    jbox_redraw((t_jbox *)p);
}

//------------------------------------------------------------------------------
// Keyboard Methods
//------------------------------------------------------------------------------

long presetter_key(t_presetter *p, t_object *patcherview, long keycode, long modifiers, long textcharacter) {
    if (p->j_editing_name) {
        size_t len = strlen(p->j_preset_name);

        // Backspace
        if (keycode == -7 && len > 0) {
            p->j_preset_name[len - 1] = '\0';
            jbox_redraw((t_jbox *)p);
            return 1;
        }

        // Enter / Return
        if (keycode == -4) {
            presetter_handle_rename(p);
            return 1;
        }

        // Escape
        if (keycode == -3) {
            p->j_editing_name = false;
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

    return 0;
}

//------------------------------------------------------------------------------
// Drawing
//------------------------------------------------------------------------------

/* Preset Name */

void presetter_draw_preset_name(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_bounds bounds = presetter_get_preset_name_bounds(p, rect);

    char text[512];

    if (p->j_editing_name) {
        snprintf_zero(text, sizeof(text), "> %s_", p->j_preset_name);
    } else {
        if (p->j_preset_name[0] != '\0') {
            snprintf_zero(text, sizeof(text), "%s", p->j_preset_name);
        } else {
            snprintf_zero(text, sizeof(text), "%s", "No preset selected");
        }
    }

    jgraphics_select_font_face(g, "Menlo", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(g, p->j_preset_name_font_size);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(g, &extents);

    double max_width = bounds.width;
    double text_width;
    double text_height;
    char visible[512];

    jgraphics_text_measure(g, text, &text_width, &text_height);

    if (p->j_editing_name && text_width > max_width) {
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
    } else if (!p->j_editing_name && text_width > max_width) {
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
    if (p->j_editing_name) {
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

    if (p->j_preset_name[0] != '\0') {
        if (p->j_editing_name) {
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

    if (p->j_write_button_down) {
        jcolor_getcolor(WRITE_BUTTON_ON_BG_COLOR_SYM, &bg_color, &bg_color_off);
        jcolor_getcolor(WRITE_BUTTON_ON_TEXT_COLOR_SYM, &text_color, &text_color_off);

    } else if (p->j_editing_name) {
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
    jgraphics_set_font_size(g, p->j_button_font_size);
    jgraphics_set_source_jrgba(g, &text_color);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(g, &extents);

    double text_y = bounds.y + (bounds.height + extents.ascent - extents.descent) / 2;
    jgraphics_move_to(g, bounds.x + BUTTON_PADDING_X, text_y);
    jgraphics_text_path(g, WRITE_BUTTON_TEXT);
    jgraphics_fill(g);
}

/* Grid */

void presetter_draw_row(t_presetter *p, t_jgraphics *g, t_grid_dim *dim, int row_idx, double y) {
    for (int i = 0; i < dim->columns; i++) {
        double x = (double)i * CELL_TOTAL_SIZE + CELL_PADDING + GRID_OFFSET_X;
        int cell_idx = i + dim->columns * row_idx + 1;

        t_jrgba color;
        t_symbol *name = presetter_lookup_slot(p, cell_idx);

        if (p->j_selected_cell == cell_idx) {
            t_jrgba color_off;
            jcolor_getcolor(GRID_SELECTED_CELL_COLOR_SYM, &color, &color_off);
        } else if (p->j_hovered_cell == cell_idx) {
            presetter_hex_to_rgba(&color, GRID_HOVERED_CELL_COLOR_HEX, 1);
        } else if (name) {
            presetter_hex_to_rgba(&color, GRID_STORED_CELL_COLOR_HEX, 1);
        } else {
            presetter_hex_to_rgba(&color, GRID_DEFAULT_CELL_COLOR_HEX, 1);
        }

        jgraphics_rectangle_rounded(g, x, y, CELL_SIZE, CELL_SIZE, 3, 3);
        jgraphics_set_source_jrgba(g, &color);
        jgraphics_fill(g);
    }
}

void presetter_draw_grid(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_bounds bounds = presetter_get_grid_bounds(p, rect);

    t_grid_dim dim = presetter_get_grid_dim(p, rect);
    for (int i = 0; i < dim.rows; i++) {
        double y = (double)i * CELL_TOTAL_SIZE + CELL_PADDING + bounds.y;
        presetter_draw_row(p, g, &dim, i, y);
    }
}

/* Status */

const char *presetter_get_status_text(t_presetter *p) {
    t_status effective = (p->j_status_override != PRESETTER_NO_STATUS) ? p->j_status_override : p->j_status;

    if (effective == PRESETTER_IDLE_STATUS && p->j_idle_status_text[0] != '\0')
        return p->j_idle_status_text;
    if (effective == PRESETTER_HOVER_STATUS && p->j_hover_status_text[0] != '\0')
        return p->j_hover_status_text;
    if (effective == PRESETTER_CONFIRM_STATUS && p->j_confirm_status_text[0] != '\0')
        return p->j_confirm_status_text;

    return "Ready";
}

void presetter_draw_status(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_bounds bounds = presetter_get_status_bounds(p, rect);

    jgraphics_select_font_face(g, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_NORMAL);
    jgraphics_set_font_size(g, p->j_status_font_size);

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

    if (p->j_status_override != PRESETTER_NO_STATUS) {
        presetter_hex_to_rgba(&color, STATUS_CONFIRM_TEXT_COLOR_HEX, 1);
    } else {
        presetter_hex_to_rgba(&color, STATUS_TEXT_COLOR_HEX, 1);
    }

    jgraphics_set_source_jrgba(g, &color);
    jgraphics_move_to(g, bounds.x, bounds.y + extents.height);
    jgraphics_text_path(g, visible);
    jgraphics_fill(g);
}

void presetter_draw_confirm_ok_button(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    if (p->j_confirm_status_text[0] == '\0' || !(p->j_confirm_store || p->j_confirm_delete))
        return;

    t_bounds bounds = presetter_get_confirm_ok_button_bounds(p, rect, g);

    t_jrgba bg_color;
    t_jrgba text_color;
    t_jrgba text_color_off;

    if (p->j_confirm_ok_button_down) {
        presetter_hex_to_rgba(&bg_color, CONFIRM_BUTTON_ON_BG_COLOR_HEX, 1);
        jcolor_getcolor(CONFIRM_BUTTON_ON_TEXT_COLOR_SYM, &text_color, &text_color_off);
    } else {
        jcolor_getcolor(CONFIRM_BUTTON_UP_BG_COLOR_SYM, &bg_color, &text_color_off);
        presetter_hex_to_rgba(&text_color, CONFIRM_BUTTON_UP_TEXT_COLOR_HEX, 1);
    }

    jgraphics_rectangle(g, bounds.x, bounds.y, bounds.width, bounds.height);

    if (!p->j_confirm_ok_button_down) {
        jgraphics_set_source_jrgba(g, &text_color);
        jgraphics_stroke_preserve(g);
    }

    jgraphics_set_source_jrgba(g, &bg_color);
    jgraphics_fill(g);

    jgraphics_select_font_face(g, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(g, p->j_button_font_size);
    jgraphics_set_source_jrgba(g, &text_color);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(g, &extents);

    double text_y = bounds.y + (bounds.height + extents.ascent - extents.descent) / 2;
    jgraphics_move_to(g, bounds.x + BUTTON_PADDING_X, text_y);
    jgraphics_text_path(g, CONFIRM_OK_BUTTON_TEXT);
    jgraphics_fill(g);
}

void presetter_draw_confirm_cancel_button(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    if (p->j_confirm_status_text[0] == '\0' || !(p->j_confirm_store || p->j_confirm_delete))
        return;

    t_bounds bounds = presetter_get_confirm_cancel_button_bounds(p, rect, g);

    t_jrgba bg_color;
    t_jrgba text_color;
    t_jrgba text_color_off;

    if (p->j_confirm_cancel_button_down) {
        presetter_hex_to_rgba(&bg_color, CONFIRM_BUTTON_ON_BG_COLOR_HEX, 1);
        jcolor_getcolor(CONFIRM_BUTTON_ON_TEXT_COLOR_SYM, &text_color, &text_color_off);
    } else {
        jcolor_getcolor(CONFIRM_BUTTON_UP_BG_COLOR_SYM, &bg_color, &text_color_off);
        presetter_hex_to_rgba(&text_color, CONFIRM_BUTTON_UP_TEXT_COLOR_HEX, 1);
    }

    jgraphics_rectangle(g, bounds.x, bounds.y, bounds.width, bounds.height);

    if (!p->j_confirm_cancel_button_down) {
        jgraphics_set_source_jrgba(g, &text_color);
        jgraphics_stroke_preserve(g);
    }

    jgraphics_set_source_jrgba(g, &bg_color);
    jgraphics_fill(g);

    jgraphics_select_font_face(g, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(g, p->j_button_font_size);
    jgraphics_set_source_jrgba(g, &text_color);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(g, &extents);

    double text_y = bounds.y + (bounds.height + extents.ascent - extents.descent) / 2;
    jgraphics_move_to(g, bounds.x + BUTTON_PADDING_X, text_y);
    jgraphics_text_path(g, CONFIRM_CANCEL_BUTTON_TEXT);
    jgraphics_fill(g);
}

/* Main Paint */

void presetter_paint(t_presetter *p, t_object *patcherview) {
    t_jgraphics *g = (t_jgraphics *)patcherview_get_jgraphics(patcherview);

    t_rect rect;
    jbox_get_rect_for_view((t_object *)p, patcherview, &rect);

    t_jrgba color;
    presetter_hex_to_rgba(&color, BG_COLOR_HEX, 1);

    jgraphics_rectangle(g, 0, 0, rect.width, rect.height);
    jgraphics_set_source_jrgba(g, &color);
    jgraphics_fill(g);

    presetter_draw_preset_name(p, g, &rect);
    presetter_draw_write_button(p, g, &rect);
    presetter_draw_grid(p, g, &rect);
    presetter_draw_status(p, g, &rect);

    presetter_draw_confirm_ok_button(p, g, &rect);
    presetter_draw_confirm_cancel_button(p, g, &rect);
}
