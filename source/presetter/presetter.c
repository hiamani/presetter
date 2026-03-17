#include "ext.h"

#include "ext_obex_util.h"
#include "ext_proto.h"
#include "handlers.h"
#include "structs.h"
#include "utilities.h"

/// Lifecycle Declarations

t_presetter *presetter_new(t_symbol *s, short argc, t_atom *argv);
void presetter_free(t_presetter *p);

/// Main

static t_class *s_presetter_class = NULL;

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

    class_addmethod(c, (method)presetter_readfilters, "readfilters", A_GIMME, 0);
    class_addmethod(c, (method)presetter_writefilters, "writefilters", A_GIMME, 0);
    class_addmethod(c, (method)presetter_savefilters, "savefilters", A_GIMME, 0);

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
    CLASS_ATTR_LABEL(c, "pattrstorage", 0, "pattrstorage Object Name");
    CLASS_ATTR_SAVE(c, "pattrstorage", 0);

    CLASS_ATTR_CHAR(c, "autowrite", 0, t_presetter, j_filters_autowrite);
    CLASS_ATTR_LABEL(c, "autowrite", 0, "Autowrite Filters JSON");
    CLASS_ATTR_STYLE(c, "autowrite", 0, "onoff");
    CLASS_ATTR_SAVE(c, "autowrite", 0);

    CLASS_ATTR_SYM(c, "filename", 0, t_presetter, j_filters_filename);
    CLASS_ATTR_LABEL(c, "filename", 0, "Filename");
    CLASS_ATTR_SAVE(c, "filename", 0);

    // Patching Rect
    CLASS_ATTR_DEFAULT(c, "patching_rect", 0, "0. 0. 289. 170.");

    class_register(CLASS_BOX, c);
    s_presetter_class = c;
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

        // Preset Confirm
        p->j_confirm_preset_cell = -1;
        p->j_confirm_preset_store = false;
        p->j_confirm_preset_delete = false;
        p->j_confirm_preset_ok_button_down = false;
        p->j_confirm_preset_cancel_button_down = false;

        // Pagination
        p->j_preset_pagination_number = 1;
        p->j_pagination_left_arrow_down = false;
        p->j_pagination_right_arrow_down = false;

        p->j_patcher_path = presetter_get_patcher_path(p);
        t_dictionary *dict = dictionary_new();
        p->j_filters = dict;
        p->j_applied_filters = hashtab_new(0);

        p->j_selected_tab = gensym("presets");

        p->j_selected_filter_cell = -1;
        p->j_hovered_filter_cell = -1;
        p->j_last_hovered_filter_cell = -1;

        // Status
        p->j_filter_status = PRESETTER_NO_STATUS;
        p->j_filter_status_override = PRESETTER_NO_STATUS;

        // Filter name
        p->j_editing_filter_name = false;
        p->j_write_filter_button_down = false;

        // Filter Confirm
        p->j_confirm_filter_cell = -1;
        p->j_confirm_filter_delete = false;
        p->j_confirm_filter_ok_button_down = false;
        p->j_confirm_filter_cancel_button_down = false;

        // Pagination
        p->j_filter_pagination_number = 1;

        presetter_read_filters_dictionary(p);

        // Clear
        p->j_clear_filters_button_down = false;

        jbox_ready(&p->j_box);

        presetter_connect_pattrstorage(p);
    }

    return p;
}

void presetter_free(t_presetter *p) {
    object_free(p->j_slots);
    object_free(p->j_filters);
    if (p->j_pattrstorage) {
        object_detach_byptr(p, p->j_pattrstorage);
    }
    jgraphics_destroy(p->offscreen);
    jgraphics_surface_destroy(p->surface);
    jbox_free((t_jbox *)p);
}
