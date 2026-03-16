#include "structs.h"

// -----------------------------------------------------------------------------
// Element Bounds
// -----------------------------------------------------------------------------

bool presetter_generic_in_bounds(t_bounds *bounds, t_pt *pt);

/* Tabs */

void presetter_measure_tab(t_presetter *p, char *text, double *outwidth, double *outheight);

t_bounds presetter_get_presets_tab_bounds(t_presetter *p, t_rect *rect);

bool presetter_in_presets_tab_bounds(t_presetter *p, t_rect *rect, t_pt *pt);

t_bounds presetter_get_filters_tab_bounds(t_presetter *p, t_rect *rect);

bool presetter_in_filters_tab_bounds(t_presetter *p, t_rect *rect, t_pt *pt);

/* Write Buttons */

void presetter_measure_button(t_presetter *p, char *text, double *outwidth, double *outheight);

/* Write Button Bounds */

t_bounds presetter_get_write_button_bounds(t_presetter *p, t_rect *rect);

bool presetter_in_write_button_bounds(t_presetter *p, t_rect *rect, t_pt *pt);

/* Preset Name Bounds */

t_bounds presetter_get_write_filter_button_bounds(t_presetter *p, t_rect *rect);

t_bounds presetter_get_name_bounds(t_presetter *p, t_rect *rect);

bool presetter_in_name_bounds(t_presetter *p, t_rect *rect, t_pt *pt);

/* Grid Bounds */

t_grid_dim presetter_get_preset_grid_dim(t_presetter *p, t_rect *rect);

t_bounds presetter_get_preset_grid_bounds(t_presetter *p, t_rect *rect);

bool presetter_in_grid_bounds(t_presetter *p, t_rect *rect, t_pt *pt);

/* Grid Utilities */

t_cell_pos presetter_get_preset_cell_pos(t_presetter *p, t_rect *rect, t_pt *pt);

long presetter_get_preset_grid_offset(t_presetter *p, t_grid_dim *dim);

long presetter_get_preset_cell_idx(t_presetter *p, t_rect *rect, t_pt *pt);

/* Right Arrow Bounds */

t_bounds presetter_get_right_arrow_bounds(t_presetter *p, t_rect *rect);

bool presetter_in_right_arrow_bounds(t_presetter *p, t_rect *rect, t_pt *pt);

/* Pagination Number Bounds */

t_bounds presetter_get_pagination_number_bounds(t_presetter *p, t_rect *rect);

/* Left Arrow Bounds */

t_bounds presetter_get_left_arrow_bounds(t_presetter *p, t_rect *rect);

bool presetter_in_left_arrow_bounds(t_presetter *p, t_rect *rect, t_pt *pt);

/* Status Bounds */

t_bounds presetter_get_preset_status_bounds(t_presetter *p, t_rect *rect);

bool presetter_in_preset_status_bounds(t_presetter *p, t_rect *rect, t_pt *pt);

/* Confirm Button Bounds */

t_bounds presetter_get_confirm_preset_ok_button_bounds(t_presetter *p, t_rect *rect);

bool presetter_in_confirm_preset_ok_button_bounds(t_presetter *p, t_rect *rect, t_pt *pt);

t_bounds presetter_get_confirm_preset_cancel_button_bounds(t_presetter *p, t_rect *rect);

bool presetter_in_confirm_preset_cancel_button_bounds(t_presetter *p, t_rect *rect, t_pt *pt);

/** Filters **/

long presetter_get_filter_grid_offset(t_presetter *p, t_grid_dim *dim);

t_bounds presetter_get_filter_grid_bounds(t_presetter *p, t_rect *rect);

t_cell_pos presetter_get_filter_cell_pos(t_presetter *p, t_rect *rect, t_pt *pt);

long presetter_get_filter_cell_idx(t_presetter *p, t_rect *rect, t_pt *pt);

bool presetter_in_filter_circle_bounds(t_presetter *p, t_rect *rect, t_pt *pt, t_cell_pos *pos);

/* Write Button Bounds */

t_bounds presetter_get_write_filter_button_bounds(t_presetter *p, t_rect *rect);

bool presetter_in_write_filter_button_bounds(t_presetter *p, t_rect *rect, t_pt *pt);

/* Status Bounds */

t_bounds presetter_get_filter_status_bounds(t_presetter *p, t_rect *rect);

bool presetter_in_filter_status_bounds(t_presetter *p, t_rect *rect, t_pt *pt);

/* Confirm Button Bounds */

t_bounds presetter_get_confirm_filter_ok_button_bounds(t_presetter *p, t_rect *rect);

bool presetter_in_confirm_filter_ok_button_bounds(t_presetter *p, t_rect *rect, t_pt *pt);

t_bounds presetter_get_confirm_filter_cancel_button_bounds(t_presetter *p, t_rect *rect);

bool presetter_in_confirm_filter_cancel_button_bounds(t_presetter *p, t_rect *rect, t_pt *pt);

t_bounds presetter_get_clear_filters_button_bounds(t_presetter *p, t_rect *rect);

bool presetter_in_clear_filters_button_bounds(t_presetter *p, t_rect *rect, t_pt *pt);
