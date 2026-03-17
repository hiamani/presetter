#ifndef PRESETTER_DRAWING
#define PRESETTER_DRAWING

#include "jgraphics.h"

#include "structs.h"

void presetter_draw_background(t_presetter *p, t_jgraphics *g, t_rect *rect);

void presetter_draw_write_name(t_presetter *p, t_jgraphics *g, t_rect *rect);

void presetter_draw_write_button(t_presetter *p, t_jgraphics *g, t_rect *rect);

void presetter_draw_preset_grid_row(t_presetter *p, t_jgraphics *g, t_grid_dim *dim, int row_idx, double y);

void presetter_draw_preset_grid(t_presetter *p, t_jgraphics *g, t_rect *rect);

void presetter_draw_preset_status(t_presetter *p, t_jgraphics *g, t_rect *rect);

void presetter_draw_confirm_preset_ok_button(t_presetter *p, t_jgraphics *g, t_rect *rect);

void presetter_draw_confirm_preset_cancel_button(t_presetter *p, t_jgraphics *g, t_rect *rect);

void presetter_draw_right_arrow(t_presetter *p, t_jgraphics *g, t_rect *rect);

void presetter_draw_pagination_number(t_presetter *p, t_jgraphics *g, t_rect *rect);

void presetter_draw_left_arrow(t_presetter *p, t_jgraphics *g, t_rect *rect);

void presetter_draw_tab_bar(t_presetter *p, t_jgraphics *g, t_rect *rect);

void presetter_draw_presets_tab(t_presetter *p, t_jgraphics *g, t_rect *rect);

void presetter_draw_filters_tab(t_presetter *p, t_jgraphics *g, t_rect *rect);

void presetter_draw_filter_grid_row(
    t_presetter *p, t_jgraphics *g, t_grid_dim *dim, int row_idx, double cell_width, double y
);

void presetter_draw_write_filter_button(t_presetter *p, t_jgraphics *g, t_rect *rect);

void presetter_draw_filter_grid(t_presetter *p, t_jgraphics *g, t_rect *rect);

void presetter_draw_confirm_filter_ok_button(t_presetter *p, t_jgraphics *g, t_rect *rect);

void presetter_draw_confirm_filter_cancel_button(t_presetter *p, t_jgraphics *g, t_rect *rect);

void presetter_draw_filter_status(t_presetter *p, t_jgraphics *g, t_rect *rect);

void presetter_draw_clear_filters_button(t_presetter *p, t_jgraphics *g, t_rect *rect);

#endif
