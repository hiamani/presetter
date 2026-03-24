#include "ext_hashtab.h"
#include "ext_proto.h"
#include "ext_strings.h"
#include "jgraphics.h"
#include "jpatcher_api.h"

#include "bounds.h"
#include "colors.h"
#include "constants.h"
#include "filters.h"
#include "presets.h"
#include "structs.h"
#include "utilities.h"

/* Utilities */

static void presetter_trim_text_right(t_jgraphics *g, const char *text, double max_width, char *out, long outsize) {
    double text_width;
    double text_height;

    jgraphics_text_measure(g, text, &text_width, &text_height);

    if (text_width > max_width) {
        snprintf_zero(out, outsize, "%s", text);

        char with_ellipsis[1048];

        while (text_width > max_width && strlen(out) > 0) {
            out[strlen(out) - 1] = '\0';
            snprintf_zero(with_ellipsis, sizeof(with_ellipsis), "%s...", out);
            jgraphics_text_measure(g, with_ellipsis, &text_width, &text_height);
        }

        snprintf_zero(out, outsize, "%s", with_ellipsis);
    } else {
        snprintf_zero(out, outsize, "%s", text);
    }
}

/* Button Drawing */

void presetter_draw_button(
    t_jgraphics *g, t_bounds *bounds, const char *text, bool button_down, t_jrgba *bg_color, t_jrgba *text_color
) {
    jgraphics_rectangle(g, bounds->x, bounds->y, bounds->width, bounds->height);

    if (!button_down) {
        jgraphics_set_source_jrgba(g, text_color);
        jgraphics_stroke_preserve(g);
    }

    jgraphics_set_source_jrgba(g, bg_color);
    jgraphics_fill(g);

    jgraphics_select_font_face(g, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(g, BUTTON_FONT_SIZE);
    jgraphics_set_source_jrgba(g, text_color);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(g, &extents);

    double text_y = bounds->y + (bounds->height + extents.ascent - extents.descent) / 2;
    jgraphics_move_to(g, bounds->x + BUTTON_PADDING_X, text_y);
    jgraphics_text_path(g, text);
    jgraphics_fill(g);
}

/* Background */

void presetter_draw_background(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_jrgba color;
    presetter_resolve_color(BG_COLOR, &color, 1);

    jgraphics_rectangle_rounded(g, 0, 0, rect->width, rect->height, 4, 4);
    jgraphics_set_source_jrgba(g, &color);
    jgraphics_fill(g);
}

/* Write Name */

void presetter_draw_write_name(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_bounds bounds = presetter_get_name_bounds(p, rect);

    char text[512];

    bool empty_filter_cell = false;

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
                empty_filter_cell = true;
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

    bool editing = p->j_editing_preset_name || p->j_editing_filter_name;

    if (editing && text_width > max_width) {
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
    } else {
        presetter_trim_text_right(g, text, bounds.width, visible, sizeof(visible));
    }

    if (editing) {
        t_jrgba bg_color;
        presetter_resolve_color(PRESET_NAME_EDITING_BG_COLOR, &bg_color, 1);

        jgraphics_set_source_jrgba(g, &bg_color);
        jgraphics_rectangle(g, bounds.x, bounds.y, text_width, bounds.height);
        jgraphics_fill(g);
    }

    double text_x = bounds.x;
    double text_y = bounds.y + extents.ascent;

    t_jrgba name_color;

    if (p->j_selected_tab == gensym("presets") && p->j_preset_name[0] != '\0') {
        if (p->j_editing_preset_name) {
            presetter_resolve_color(PRESET_NAME_TEXT_EDITING_COLOR, &name_color, 1);
        } else {
            presetter_resolve_color(PRESET_NAME_TEXT_DEFAULT_COLOR, &name_color, 1);
        }
    } else if (p->j_selected_tab == gensym("filters") && p->j_filter_name[0] != '\0') {
        if (p->j_editing_filter_name) {
            presetter_resolve_color(PRESET_NAME_TEXT_EDITING_COLOR, &name_color, 1);
        } else {
            presetter_resolve_color(PRESET_NAME_TEXT_DEFAULT_COLOR, &name_color, 1);
        }
    } else if (empty_filter_cell) {
        presetter_resolve_color(PATCHER_OBJECT_COLOR, &name_color, 1);
    } else {
        presetter_resolve_color(PRESET_NAME_TEXT_UNSELECTED_COLOR, &name_color, 1);
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

    if (p->j_write_button_down) {
        presetter_resolve_color(WRITE_BUTTON_ON_BG_COLOR, &bg_color, 1);
        presetter_resolve_color(WRITE_BUTTON_ON_TEXT_COLOR, &text_color, 1);
    } else if (p->j_editing_preset_name) {
        presetter_resolve_color(WRITE_BUTTON_UP_BG_COLOR, &bg_color, 1);
        presetter_resolve_color(WRITE_BUTTON_UP_TEXT_COLOR, &text_color, 1);
    } else {
        presetter_resolve_color(WRITE_BUTTON_INACTIVE_BG_COLOR, &bg_color, 1);
        presetter_resolve_color(WRITE_BUTTON_INACTIVE_TEXT_COLOR, &text_color, 1);
    }

    presetter_draw_button(g, &bounds, WRITE_BUTTON_TEXT, p->j_write_button_down, &bg_color, &text_color);
}

/* Grid */

void presetter_draw_preset_grid_row(t_presetter *p, t_jgraphics *g, t_grid_dim *dim, int row_idx, double y) {
    long applied_filters = hashtab_getsize(p->j_applied_filters);

    for (int i = 0; i < dim->columns; i++) {
        double x = (double)i * CELL_TOTAL_SIZE + CELL_PADDING + GRID_OFFSET_X;
        int cell_idx = (i + dim->columns * row_idx + 1) + presetter_get_preset_grid_offset(p, dim);

        t_jrgba color;
        bool use_stroke = false;

        t_symbol *name = presetter_lookup_preset_slot(p, cell_idx);
        bool cell_selected = p->j_selected_preset_cell == cell_idx;
        bool cell_hovered = p->j_hovered_preset_cell == cell_idx;
        bool cell_filtered = applied_filters > 0 && name && presetter_filtered_cell(p, cell_idx);
        bool cell_unfiltered = applied_filters > 0 && name && !cell_filtered;

        if (cell_filtered && cell_selected) {
            presetter_resolve_color(GRID_SELECTED_CELL_COLOR, &color, 1);
        } else if (cell_filtered && cell_hovered) {
            presetter_resolve_color(GRID_HOVERED_CELL_COLOR, &color, 1);
            use_stroke = true;
        } else if (cell_filtered) {
            presetter_resolve_color(GRID_FILTERED_CELL_COLOR, &color, 1);
            use_stroke = true;
        } else if (cell_selected) {
            presetter_resolve_color(GRID_SELECTED_CELL_COLOR, &color, 1);
        } else if (cell_hovered) {
            presetter_resolve_color(GRID_HOVERED_CELL_COLOR, &color, 1);
        } else if (cell_unfiltered) {
            presetter_resolve_color(GRID_STORED_CELL_COLOR, &color, 1);
        } else if (name) {
            presetter_resolve_color(GRID_STORED_CELL_COLOR, &color, 1);
        } else {
            presetter_resolve_color(GRID_DEFAULT_CELL_COLOR, &color, 1);
        }

        jgraphics_rectangle_rounded(g, x, y, CELL_SIZE, CELL_SIZE, 3, 3);
        jgraphics_set_source_jrgba(g, &color);

        if (use_stroke) {
            jgraphics_stroke(g);
        } else {
            jgraphics_fill(g);
        }
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

const char *presetter_get_preset_status_text(t_presetter *p) {
    t_status effective =
        (p->j_preset_status_override != PRESETTER_NO_STATUS) ? p->j_preset_status_override : p->j_preset_status;

    if (effective == PRESETTER_IDLE_STATUS && p->j_preset_idle_status_text[0] != '\0')
        return p->j_preset_idle_status_text;
    if (effective == PRESETTER_HOVER_STATUS && p->j_preset_hover_status_text[0] != '\0')
        return p->j_preset_hover_status_text;
    if (effective == PRESETTER_CONFIRM_STATUS && p->j_confirm_preset_status_text[0] != '\0')
        return p->j_confirm_preset_status_text;

    return "Ready";
}

const char *presetter_get_filter_status_text(t_presetter *p) {
    t_status effective =
        (p->j_filter_status_override != PRESETTER_NO_STATUS) ? p->j_filter_status_override : p->j_filter_status;

    if (effective == PRESETTER_IDLE_STATUS && p->j_filter_idle_status_text[0] != '\0')
        return p->j_filter_idle_status_text;
    if (effective == PRESETTER_HOVER_STATUS && p->j_filter_hover_status_text[0] != '\0')
        return p->j_filter_hover_status_text;
    if (effective == PRESETTER_CONFIRM_STATUS && p->j_confirm_filter_status_text[0] != '\0')
        return p->j_confirm_filter_status_text;

    return "Ready";
}

void presetter_draw_preset_status(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_bounds bounds = presetter_get_preset_status_bounds(p, rect);

    jgraphics_select_font_face(g, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_NORMAL);
    jgraphics_set_font_size(g, STATUS_FONT_SIZE);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(g, &extents);

    char visible[1048];
    presetter_trim_text_right(g, presetter_get_preset_status_text(p), bounds.width, visible, sizeof(visible));

    t_jrgba color;

    if (p->j_preset_status_override != PRESETTER_NO_STATUS) {
        presetter_resolve_color(STATUS_CONFIRM_TEXT_COLOR, &color, 1);
    } else {
        presetter_resolve_color(STATUS_TEXT_COLOR, &color, 1);
    }

    jgraphics_set_source_jrgba(g, &color);
    jgraphics_move_to(g, bounds.x, bounds.y + extents.height);
    jgraphics_text_path(g, visible);
    jgraphics_fill(g);
}

void presetter_draw_confirm_preset_ok_button(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    if (hashtab_getsize(p->j_applied_filters) > 0)
        return;

    if (!(p->j_confirm_preset_store || p->j_confirm_preset_delete))
        return;

    t_bounds bounds = presetter_get_confirm_preset_ok_button_bounds(p, rect);

    t_jrgba bg_color;
    t_jrgba text_color;

    if (p->j_confirm_preset_ok_button_down) {
        presetter_resolve_color(CONFIRM_BUTTON_ON_BG_COLOR, &bg_color, 1);
        presetter_resolve_color(CONFIRM_BUTTON_ON_TEXT_COLOR, &text_color, 1);
    } else {
        presetter_resolve_color(CONFIRM_BUTTON_UP_BG_COLOR, &bg_color, 1);
        presetter_resolve_color(CONFIRM_BUTTON_UP_TEXT_COLOR, &text_color, 1);
    }

    presetter_draw_button(
        g, &bounds, CONFIRM_OK_BUTTON_TEXT, p->j_confirm_preset_ok_button_down, &bg_color, &text_color
    );
}

void presetter_draw_confirm_preset_cancel_button(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    if (hashtab_getsize(p->j_applied_filters) > 0)
        return;

    if (!(p->j_confirm_preset_store || p->j_confirm_preset_delete))
        return;

    t_bounds bounds = presetter_get_confirm_preset_cancel_button_bounds(p, rect);

    t_jrgba bg_color;
    t_jrgba text_color;

    if (p->j_confirm_preset_cancel_button_down) {
        presetter_resolve_color(CONFIRM_BUTTON_ON_BG_COLOR, &bg_color, 1);
        presetter_resolve_color(CONFIRM_BUTTON_ON_TEXT_COLOR, &text_color, 1);
    } else {
        presetter_resolve_color(CONFIRM_BUTTON_UP_BG_COLOR, &bg_color, 1);
        presetter_resolve_color(CONFIRM_BUTTON_UP_TEXT_COLOR, &text_color, 1);
    }

    presetter_draw_button(
        g, &bounds, CONFIRM_CANCEL_BUTTON_TEXT, p->j_confirm_preset_cancel_button_down, &bg_color, &text_color
    );
}

void presetter_draw_right_arrow(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_bounds bounds = presetter_get_right_arrow_bounds(p, rect);

    t_jrgba color;
    if (p->j_pagination_right_arrow_down) {
        presetter_resolve_color(PAGINATION_ARROW_DOWN_COLOR, &color, 1);
    } else {
        presetter_resolve_color(PAGINATION_ARROW_ON_COLOR, &color, 1);
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
    presetter_resolve_color(PAGINATION_ARROW_ON_COLOR, &color, 1);
    jgraphics_set_source_jrgba(g, &color);
    jgraphics_select_font_face(g, PAGINATION_NUMBER_FONT, JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(g, PAGINATION_NUMBER_FONT_SIZE);

    char number_text[16];
    if (p->j_selected_tab == gensym("presets")) {
        snprintf_zero(number_text, sizeof(number_text), "%ld", p->j_preset_pagination_number);
    } else {
        snprintf_zero(number_text, sizeof(number_text), "%ld", p->j_filter_pagination_number);
    }

    jgraphics_move_to(g, bounds.x, bounds.y + bounds.height);
    jgraphics_text_path(g, number_text);
    jgraphics_fill(g);
}

void presetter_draw_left_arrow(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_bounds bounds = presetter_get_left_arrow_bounds(p, rect);

    t_jrgba color;
    if (p->j_pagination_left_arrow_down) {
        presetter_resolve_color(PAGINATION_ARROW_DOWN_COLOR, &color, 1);
    } else if (p->j_selected_tab == gensym("presets") && p->j_preset_pagination_number > 1) {
        presetter_resolve_color(PAGINATION_ARROW_ON_COLOR, &color, 1);
    } else if (p->j_selected_tab == gensym("filters") && p->j_filter_pagination_number > 1) {
        presetter_resolve_color(PAGINATION_ARROW_ON_COLOR, &color, 1);
    } else {
        presetter_resolve_color(PAGINATION_ARROW_OFF_COLOR, &color, 1);
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
    presetter_resolve_color(TAB_BAR_COLOR, &bg_color, 1);

    jgraphics_rectangle(g, bounds.x, bounds.y, rect->width, bounds.height);
    jgraphics_set_source_jrgba(g, &bg_color);
    jgraphics_fill(g);
}

void presetter_draw_presets_tab(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_bounds bounds = presetter_get_presets_tab_bounds(p, rect);

    t_jrgba bg_color;
    t_jrgba text_color;

    if (p->j_selected_tab == gensym("presets")) {
        presetter_resolve_color(TAB_PRESETS_ON_BG_COLOR, &bg_color, 1);
        presetter_resolve_color(TAB_PRESETS_ON_TEXT_COLOR, &text_color, 1);
    } else {
        presetter_resolve_color(TAB_PRESETS_UP_BG_COLOR, &bg_color, 1);
        presetter_resolve_color(TAB_PRESETS_UP_TEXT_COLOR, &text_color, 1);
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

    if (p->j_selected_tab == gensym("filters")) {
        presetter_resolve_color(TAB_FILTERS_ON_BG_COLOR, &bg_color, 1);
        presetter_resolve_color(TAB_FILTERS_ON_TEXT_COLOR, &text_color, 1);
    } else {
        presetter_resolve_color(TAB_FILTERS_UP_BG_COLOR, &bg_color, 1);
        presetter_resolve_color(TAB_FILTERS_UP_TEXT_COLOR, &text_color, 1);
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

        t_jrgba bg_color;
        if (cell_idx == p->j_hovered_filter_cell) {
            presetter_resolve_color(FILTER_GRID_HOVERED_CELL_BG_COLOR, &bg_color, 1);
        } else {
            presetter_resolve_color(FILTER_GRID_CELL_BG_COLOR, &bg_color, 1);
        }

        jgraphics_rectangle_rounded(g, x, y, cell_width, FILTER_CELL_HEIGHT, 5, 5);
        jgraphics_set_source_jrgba(g, &bg_color);
        jgraphics_fill(g);

        jgraphics_select_font_face(g, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
        jgraphics_set_font_size(g, FILTER_CELL_FONT_SIZE);

        t_jgraphics_font_extents extents;
        jgraphics_font_extents(g, &extents);

        t_jrgba stroke_color;
        presetter_resolve_color(FILTER_GRID_SELECTED_CELL_STROKE_COLOR, &stroke_color, 1);

        t_symbol *name = NULL;
        t_dictionary *dict = presetter_lookup_filter_slot(p, cell_idx);

        if (dict && dictionary_getsym(dict, gensym("name"), &name) == MAX_ERR_NONE && name) {

            char visible[1048];
            presetter_trim_text_right(g, name->s_name, cell_width - 24, visible, sizeof(visible));

            long val = 0;
            hashtab_lookuplong(p->j_applied_filters, presetter_long_to_sym(cell_idx), &val);

            t_jrgba text_color;

            if (val) {
                presetter_resolve_color(FILTER_GRID_APPLIED_SELECTED_TEXT_COLOR, &text_color, 1);
            } else if (p->j_selected_filter_cell == cell_idx) {
                presetter_resolve_color(FILTER_GRID_SELECTED_TEXT_COLOR, &text_color, 1);
            } else {
                presetter_resolve_color(FILTER_GRID_DEFAULT_TEXT_COLOR, &text_color, 1);
            }

            jgraphics_set_source_jrgba(g, &text_color);

            double text_y = y + (FILTER_CELL_HEIGHT + extents.ascent - extents.descent) / 2;
            jgraphics_move_to(g, x + 18, text_y);
            jgraphics_text_path(g, visible);
            jgraphics_fill(g);

            t_jrgba circle_color;

            if (val) {
                presetter_resolve_color(FILTER_GRID_APPLIED_SELECTED_CIRCLE_COLOR, &circle_color, 1);
            } else if (p->j_selected_filter_cell == cell_idx) {
                presetter_resolve_color(FILTER_GRID_SELECTED_CIRCLE_COLOR, &circle_color, 1);
            } else {
                presetter_resolve_color(FILTER_GRID_DEFAULT_CIRCLE_COLOR, &circle_color, 1);
            }

            jgraphics_set_source_jrgba(g, &circle_color);
            jgraphics_rectangle_rounded(g, x + 6, y + ((double)FILTER_CELL_HEIGHT - 8) / 2, 8, 8, 4, 4);
            jgraphics_fill(g);

            if (val) {
                presetter_resolve_color(FILTER_GRID_APPLIED_SELECTED_CELL_STROKE_COLOR, &stroke_color, 1);
            }
        } else {
            t_jrgba circle_color;
            presetter_resolve_color(FILTER_GRID_EMPTY_CIRCLE_COLOR, &circle_color, 1);
            jgraphics_set_source_jrgba(g, &circle_color);
            jgraphics_rectangle_rounded(g, x + 6, y + ((double)FILTER_CELL_HEIGHT - 8) / 2, 8, 8, 4, 4);
            jgraphics_fill(g);

            t_jrgba text_color;
            presetter_resolve_color(FILTER_GRID_EMPTY_TEXT_COLOR, &text_color, 1);
            presetter_resolve_color(FILTER_GRID_EMPTY_CELL_STROKE_COLOR, &stroke_color, 1);

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

    if (p->j_write_filter_button_down) {
        presetter_resolve_color(WRITE_BUTTON_ON_BG_COLOR, &bg_color, 1);
        presetter_resolve_color(WRITE_BUTTON_ON_TEXT_COLOR, &text_color, 1);
    } else if (p->j_editing_filter_name) {
        presetter_resolve_color(WRITE_BUTTON_UP_BG_COLOR, &bg_color, 1);
        presetter_resolve_color(WRITE_BUTTON_UP_TEXT_COLOR, &text_color, 1);
    } else {
        presetter_resolve_color(WRITE_BUTTON_INACTIVE_BG_COLOR, &bg_color, 1);
        presetter_resolve_color(WRITE_BUTTON_INACTIVE_TEXT_COLOR, &text_color, 1);
    }

    presetter_draw_button(g, &bounds, WRITE_FILTER_BUTTON_TEXT, p->j_write_filter_button_down, &bg_color, &text_color);
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

void presetter_draw_confirm_filter_ok_button(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    if (hashtab_getsize(p->j_applied_filters) > 0)
        return;

    if (!p->j_confirm_filter_delete)
        return;

    t_bounds bounds = presetter_get_confirm_filter_ok_button_bounds(p, rect);

    t_jrgba bg_color;
    t_jrgba text_color;

    if (p->j_confirm_filter_ok_button_down) {
        presetter_resolve_color(FILTER_CONFIRM_BUTTON_ON_BG_COLOR, &bg_color, 1);
        presetter_resolve_color(FILTER_CONFIRM_BUTTON_ON_TEXT_COLOR, &text_color, 1);
    } else {
        presetter_resolve_color(FILTER_CONFIRM_BUTTON_UP_BG_COLOR, &bg_color, 1);
        presetter_resolve_color(FILTER_CONFIRM_BUTTON_UP_TEXT_COLOR, &text_color, 1);
    }

    presetter_draw_button(
        g, &bounds, CONFIRM_OK_BUTTON_TEXT, p->j_confirm_filter_ok_button_down, &bg_color, &text_color
    );
}

void presetter_draw_confirm_filter_cancel_button(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    if (hashtab_getsize(p->j_applied_filters) > 0)
        return;

    if (!p->j_confirm_filter_delete)
        return;

    t_bounds bounds = presetter_get_confirm_filter_cancel_button_bounds(p, rect);

    t_jrgba bg_color;
    t_jrgba text_color;

    if (p->j_confirm_filter_cancel_button_down) {
        presetter_resolve_color(FILTER_CONFIRM_BUTTON_ON_BG_COLOR, &bg_color, 1);
        presetter_resolve_color(FILTER_CONFIRM_BUTTON_ON_TEXT_COLOR, &text_color, 1);
    } else {
        presetter_resolve_color(FILTER_CONFIRM_BUTTON_UP_BG_COLOR, &bg_color, 1);
        presetter_resolve_color(FILTER_CONFIRM_BUTTON_UP_TEXT_COLOR, &text_color, 1);
    }

    presetter_draw_button(
        g, &bounds, CONFIRM_CANCEL_BUTTON_TEXT, p->j_confirm_filter_cancel_button_down, &bg_color, &text_color
    );
}

void presetter_draw_filter_status(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    t_bounds bounds = presetter_get_filter_status_bounds(p, rect);

    jgraphics_select_font_face(g, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_NORMAL);
    jgraphics_set_font_size(g, STATUS_FONT_SIZE);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(g, &extents);

    char visible[1048];
    presetter_trim_text_right(g, presetter_get_filter_status_text(p), bounds.width, visible, sizeof(visible));

    t_jrgba color;

    if (p->j_filter_status_override != PRESETTER_NO_STATUS) {
        presetter_resolve_color(PATCHER_OBJECT_COLOR, &color, 1);
    } else {
        presetter_resolve_color(STATUS_TEXT_COLOR, &color, 1);
    }

    jgraphics_set_source_jrgba(g, &color);
    jgraphics_move_to(g, bounds.x, bounds.y + extents.height);
    jgraphics_text_path(g, visible);
    jgraphics_fill(g);
}

void presetter_draw_clear_filters_button(t_presetter *p, t_jgraphics *g, t_rect *rect) {
    if (hashtab_getsize(p->j_applied_filters) == 0)
        return;

    t_bounds bounds = presetter_get_clear_filters_button_bounds(p, rect);

    t_jrgba bg_color;
    t_jrgba text_color;

    if (p->j_selected_tab == gensym("presets")) {
        if (p->j_clear_filters_button_down) {
            presetter_resolve_color(CONFIRM_BUTTON_ON_BG_COLOR, &bg_color, 1);
            presetter_resolve_color(CONFIRM_BUTTON_ON_TEXT_COLOR, &text_color, 1);
        } else {
            presetter_resolve_color(CONFIRM_BUTTON_UP_BG_COLOR, &bg_color, 1);
            presetter_resolve_color(CONFIRM_BUTTON_UP_TEXT_COLOR, &text_color, 1);
        }
    } else {
        if (p->j_clear_filters_button_down) {
            presetter_resolve_color(FILTER_CONFIRM_BUTTON_ON_BG_COLOR, &bg_color, 1);
            presetter_resolve_color(FILTER_CONFIRM_BUTTON_ON_TEXT_COLOR, &text_color, 1);
        } else {
            presetter_resolve_color(FILTER_CONFIRM_BUTTON_UP_BG_COLOR, &bg_color, 1);
            presetter_resolve_color(FILTER_CONFIRM_BUTTON_UP_TEXT_COLOR, &text_color, 1);
        }
    }

    presetter_draw_button(
        g, &bounds, p->j_clear_filters_button_text, p->j_clear_filters_button_down, &bg_color, &text_color
    );
}
