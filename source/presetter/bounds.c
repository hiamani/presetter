#include "ext_proto.h"
#include "ext_strings.h"
#include "jgraphics.h"
#include "jpatcher_api.h"

#include "constants.h"
#include "structs.h"

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

// Measure Status Height

double presetter_get_status_height(t_presetter *p) {
    jgraphics_select_font_face(p->offscreen, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(p->offscreen, STATUS_FONT_SIZE);

    t_jgraphics_font_extents extents;
    jgraphics_font_extents(p->offscreen, &extents);
    return extents.height;
}

/* Grid Bounds */

t_grid_dim presetter_get_preset_grid_dim(t_presetter *p, t_rect *rect) {
    t_bounds pnbounds = presetter_get_name_bounds(p, rect);
    t_bounds ptbounds = presetter_get_presets_tab_bounds(p, rect);

    t_grid_dim dim;
    dim.columns = (int)floor((rect->width - CELL_PADDING - GRID_OFFSET_X) / CELL_TOTAL_SIZE);

    double status_height = presetter_get_status_height(p);
    double grid_y_pos = WRITE_NAME_OFFSET_Y + pnbounds.height + GRID_OFFSET_Y + ptbounds.height + TAB_MARGIN;
    double grid_space = rect->height - grid_y_pos - status_height - STATUS_OFFSET_Y - CELL_PADDING;
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

    bool in_x = pt->x >= bounds.x + CELL_PADDING && pt->x <= bounds.x + bounds.width;
    bool in_y = pt->y >= bounds.y + CELL_PADDING && pt->y <= bounds.y + bounds.height;

    return in_x && in_y;
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
    snprintf_zero(number_text, sizeof(number_text), "%ld", p->j_preset_pagination_number);

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

t_bounds presetter_get_preset_status_bounds(t_presetter *p, t_rect *rect) {
    t_bounds gbounds = presetter_get_preset_grid_bounds(p, rect);
    // t_bounds ra_bounds = presetter_get_right_arrow_bounds(p, rect);
    t_bounds la_bounds = presetter_get_left_arrow_bounds(p, rect);

    t_bounds bounds;
    bounds.x = STATUS_OFFSET_X;
    bounds.y = gbounds.y + gbounds.height + STATUS_OFFSET_Y;
    bounds.width = rect->width - STATUS_OFFSET_X - STATUS_PADDING_RIGHT - (rect->width - la_bounds.x);
    bounds.height = presetter_get_status_height(p);
    return bounds;
}

bool presetter_in_preset_status_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_preset_status_bounds(p, rect);
    return presetter_generic_in_bounds(&bounds, pt);
}

/* Confirm Button Bounds */

t_bounds presetter_get_confirm_preset_ok_button_bounds(t_presetter *p, t_rect *rect) {
    t_bounds gbounds = presetter_get_preset_grid_bounds(p, rect);

    double text_width;
    double text_height;

    jgraphics_select_font_face(p->offscreen, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(p->offscreen, STATUS_FONT_SIZE);
    jgraphics_text_measure(p->offscreen, p->j_confirm_preset_status_text, &text_width, &text_height);

    t_bounds bounds;
    bounds.x = text_width + CONFIRM_OK_BUTTON_MARGIN_LEFT;
    bounds.y = gbounds.y + gbounds.height + CONFIRM_BUTTON_OFFSET_Y;
    presetter_measure_button(p, CONFIRM_OK_BUTTON_TEXT, &bounds.width, &bounds.height);
    return bounds;
}

bool presetter_in_confirm_preset_ok_button_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_confirm_preset_ok_button_bounds(p, rect);
    return presetter_generic_in_bounds(&bounds, pt);
}

t_bounds presetter_get_confirm_preset_cancel_button_bounds(t_presetter *p, t_rect *rect) {
    t_bounds cobounds = presetter_get_confirm_preset_ok_button_bounds(p, rect);

    t_bounds bounds;
    bounds.x = cobounds.x + cobounds.width + CONFIRM_CANCEL_BUTTON_MARGIN_LEFT;
    bounds.y = cobounds.y;
    presetter_measure_button(p, CONFIRM_CANCEL_BUTTON_TEXT, &bounds.width, &bounds.height);
    return bounds;
}

bool presetter_in_confirm_preset_cancel_button_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_confirm_preset_cancel_button_bounds(p, rect);
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

    return pt->x >= circle_x && pt->x <= circle_x + 8 && pt->y >= circle_y && pt->y <= circle_y + 8;
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

/* Status Bounds */

t_bounds presetter_get_filter_status_bounds(t_presetter *p, t_rect *rect) {
    t_bounds gbounds = presetter_get_preset_grid_bounds(p, rect);
    t_bounds la_bounds = presetter_get_left_arrow_bounds(p, rect);

    t_bounds bounds;
    bounds.x = STATUS_OFFSET_X;
    bounds.y = gbounds.y + gbounds.height + STATUS_OFFSET_Y;
    bounds.width = rect->width - STATUS_OFFSET_X - STATUS_PADDING_RIGHT - (rect->width - la_bounds.x);
    bounds.height = presetter_get_status_height(p);
    return bounds;
}

bool presetter_in_filter_status_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_preset_status_bounds(p, rect);
    return presetter_generic_in_bounds(&bounds, pt);
}

/* Confirm Button Bounds */

t_bounds presetter_get_confirm_filter_ok_button_bounds(t_presetter *p, t_rect *rect) {
    t_bounds gbounds = presetter_get_preset_grid_bounds(p, rect);

    double text_width;
    double text_height;

    jgraphics_select_font_face(p->offscreen, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(p->offscreen, STATUS_FONT_SIZE);
    jgraphics_text_measure(p->offscreen, p->j_confirm_filter_status_text, &text_width, &text_height);

    t_bounds bounds;
    bounds.x = text_width + CONFIRM_OK_BUTTON_MARGIN_LEFT;
    bounds.y = gbounds.y + gbounds.height + CONFIRM_BUTTON_OFFSET_Y;
    presetter_measure_button(p, CONFIRM_OK_BUTTON_TEXT, &bounds.width, &bounds.height);
    return bounds;
}

bool presetter_in_confirm_filter_ok_button_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_confirm_filter_ok_button_bounds(p, rect);
    return presetter_generic_in_bounds(&bounds, pt);
}

t_bounds presetter_get_confirm_filter_cancel_button_bounds(t_presetter *p, t_rect *rect) {
    t_bounds cobounds = presetter_get_confirm_filter_ok_button_bounds(p, rect);

    t_bounds bounds;
    bounds.x = cobounds.x + cobounds.width + CONFIRM_CANCEL_BUTTON_MARGIN_LEFT;
    bounds.y = cobounds.y;
    presetter_measure_button(p, CONFIRM_CANCEL_BUTTON_TEXT, &bounds.width, &bounds.height);
    return bounds;
}

bool presetter_in_confirm_filter_cancel_button_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_confirm_filter_cancel_button_bounds(p, rect);
    return presetter_generic_in_bounds(&bounds, pt);
}

t_bounds presetter_get_clear_filters_button_bounds(t_presetter *p, t_rect *rect) {
    t_bounds gbounds = presetter_get_preset_grid_bounds(p, rect);

    double text_width;
    double text_height;

    jgraphics_select_font_face(p->offscreen, "Arial", JGRAPHICS_FONT_SLANT_NORMAL, JGRAPHICS_FONT_WEIGHT_BOLD);
    jgraphics_set_font_size(p->offscreen, STATUS_FONT_SIZE);
    jgraphics_text_measure(p->offscreen, p->j_clear_filters_status_text, &text_width, &text_height);

    t_bounds bounds;
    bounds.x = text_width + CONFIRM_OK_BUTTON_MARGIN_LEFT;
    bounds.y = gbounds.y + gbounds.height + CONFIRM_BUTTON_OFFSET_Y;
    presetter_measure_button(p, CLEAR_FILTERS_BUTTON_TEXT, &bounds.width, &bounds.height);
    return bounds;
}

bool presetter_in_clear_filters_button_bounds(t_presetter *p, t_rect *rect, t_pt *pt) {
    t_bounds bounds = presetter_get_clear_filters_button_bounds(p, rect);
    return presetter_generic_in_bounds(&bounds, pt);
}
