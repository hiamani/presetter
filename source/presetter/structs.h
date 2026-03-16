#ifndef PRESETTER_STRUCTS
#define PRESETTER_STRUCTS

#include "ext_dictionary.h"
#include "ext_hashtab.h"
#include "ext_mess.h"
#include "jgraphics.h"
#include "jpatcher_api.h"

typedef struct _slot {
    char name[1024];
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
    char j_confirm_preset_status_text[512];

    double j_status_height;

    // Confirm
    long j_confirm_preset_cell;
    bool j_confirm_preset_store;
    bool j_confirm_preset_delete;
    bool j_confirm_preset_ok_button_down;
    bool j_confirm_preset_cancel_button_down;

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
    char j_confirm_filter_status_text[512];

    // Confirm
    long j_confirm_filter_cell;
    bool j_confirm_filter_delete;
    bool j_confirm_filter_ok_button_down;
    bool j_confirm_filter_cancel_button_down;

    // Pagination
    long j_filter_pagination_number;

    // Clear
    char j_clear_filters_status_text[512];
    bool j_clear_filters_button_down;
} t_presetter;

#endif
