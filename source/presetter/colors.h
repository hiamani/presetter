#ifndef PRESETTER_COLORS
#define PRESETTER_COLORS

#include "jpatcher_api.h"

/* Types */

typedef enum {
    COLOR_HEX,
    COLOR_SYM
} t_color_type;

typedef struct _p_color {
    t_color_type type;
    const char *value;
} p_color;

/* Constants */

// Global
#define PATCHER_OBJECT_COLOR (p_color){COLOR_HEX, "#9EBEF3"}
#define BG_COLOR (p_color){COLOR_HEX, "#1E1E1E"}

// Preset Name
#define PRESET_NAME_EDITING_BG_COLOR (p_color){COLOR_SYM, "live_arranger_grid_tiles"}
#define PRESET_NAME_TEXT_UNSELECTED_COLOR (p_color){COLOR_HEX, "#555555"}
#define PRESET_NAME_TEXT_EDITING_COLOR (p_color){COLOR_SYM, "live_contrast_frame"}
#define PRESET_NAME_TEXT_DEFAULT_COLOR (p_color){COLOR_SYM, "live_arranger_grid_tiles"}

// Write Button
#define WRITE_BUTTON_INACTIVE_BG_COLOR BG_COLOR
#define WRITE_BUTTON_INACTIVE_TEXT_COLOR (p_color){COLOR_HEX, "#444444"}
#define WRITE_BUTTON_UP_BG_COLOR (p_color){COLOR_SYM, "live_contrast_frame"}
#define WRITE_BUTTON_UP_TEXT_COLOR (p_color){COLOR_SYM, "live_arranger_grid_tiles"}
#define WRITE_BUTTON_ON_BG_COLOR (p_color){COLOR_SYM, "live_arranger_grid_tiles"}
#define WRITE_BUTTON_ON_TEXT_COLOR (p_color){COLOR_SYM, "live_contrast_frame"}

// Grid
#define GRID_SELECTED_CELL_COLOR (p_color){COLOR_SYM, "live_display_handle_two"}
#define GRID_HOVERED_CELL_COLOR (p_color){COLOR_HEX, "#999999"}
#define GRID_STORED_CELL_COLOR (p_color){COLOR_HEX, "#666666"}
#define GRID_DEFAULT_CELL_COLOR (p_color){COLOR_HEX, "#333333"}

// Status
#define STATUS_TEXT_COLOR (p_color){COLOR_HEX, "#999999"}
#define STATUS_CONFIRM_TEXT_COLOR (p_color){COLOR_SYM, "live_display_handle_two"}

// Confirm
#define CONFIRM_BUTTON_UP_BG_COLOR (p_color){COLOR_SYM, "live_contrast_frame"}
#define CONFIRM_BUTTON_UP_TEXT_COLOR (p_color){COLOR_SYM, "live_display_handle_two"}
#define CONFIRM_BUTTON_ON_BG_COLOR (p_color){COLOR_SYM, "live_display_handle_two"}
#define CONFIRM_BUTTON_ON_TEXT_COLOR (p_color){COLOR_SYM, "live_contrast_frame"}

// Pagination
#define PAGINATION_ARROW_OFF_COLOR (p_color){COLOR_HEX, "#444444"}
#define PAGINATION_ARROW_ON_COLOR (p_color){COLOR_HEX, "#666666"}
#define PAGINATION_ARROW_DOWN_COLOR PATCHER_OBJECT_COLOR

// Tabs
#define TAB_PRESETS_UP_BG_COLOR (p_color){COLOR_HEX, "#181818"}
#define TAB_PRESETS_UP_TEXT_COLOR (p_color){COLOR_HEX, "#555555"}
#define TAB_PRESETS_ON_BG_COLOR (p_color){COLOR_SYM, "live_display_handle_two"}
#define TAB_PRESETS_ON_TEXT_COLOR (p_color){COLOR_SYM, "live_contrast_frame"}

#define TAB_FILTERS_UP_BG_COLOR (p_color){COLOR_HEX, "#181818"}
#define TAB_FILTERS_UP_TEXT_COLOR (p_color){COLOR_HEX, "#555555"}
#define TAB_FILTERS_ON_BG_COLOR PATCHER_OBJECT_COLOR
#define TAB_FILTERS_ON_TEXT_COLOR (p_color){COLOR_SYM, "live_contrast_frame"}

#define TAB_BAR_COLOR (p_color){COLOR_HEX, "#181818"}

/// Filters

// Grid
#define FILTER_GRID_CELL_BG_COLOR (p_color){COLOR_HEX, "#222222"}
#define FILTER_GRID_HOVERED_CELL_BG_COLOR (p_color){COLOR_HEX, "#2A2A2A"}
#define FILTER_GRID_SELECTED_CELL_STROKE_COLOR (p_color){COLOR_SYM, "live_arranger_grid_tiles"}
#define FILTER_GRID_APPLIED_SELECTED_CELL_STROKE_COLOR PATCHER_OBJECT_COLOR
#define FILTER_GRID_EMPTY_CELL_STROKE_COLOR (p_color){COLOR_HEX, "#555555"}

#define FILTER_GRID_DEFAULT_TEXT_COLOR (p_color){COLOR_HEX, "#999999"}
#define FILTER_GRID_SELECTED_TEXT_COLOR (p_color){COLOR_SYM, "live_arranger_grid_tiles"}
#define FILTER_GRID_APPLIED_SELECTED_TEXT_COLOR PATCHER_OBJECT_COLOR
#define FILTER_GRID_EMPTY_TEXT_COLOR (p_color){COLOR_HEX, "#555555"}

#define FILTER_GRID_DEFAULT_CIRCLE_COLOR (p_color){COLOR_HEX, "#999999"}
#define FILTER_GRID_SELECTED_CIRCLE_COLOR (p_color){COLOR_SYM, "live_arranger_grid_tiles"}
#define FILTER_GRID_APPLIED_SELECTED_CIRCLE_COLOR PATCHER_OBJECT_COLOR
#define FILTER_GRID_EMPTY_CIRCLE_COLOR (p_color){COLOR_HEX, "#555555"}

// Confirm
#define FILTER_CONFIRM_BUTTON_UP_BG_COLOR (p_color){COLOR_SYM, "live_contrast_frame"}
#define FILTER_CONFIRM_BUTTON_UP_TEXT_COLOR PATCHER_OBJECT_COLOR
#define FILTER_CONFIRM_BUTTON_ON_BG_COLOR PATCHER_OBJECT_COLOR
#define FILTER_CONFIRM_BUTTON_ON_TEXT_COLOR (p_color){COLOR_SYM, "live_contrast_frame"}

/* Utilities */

void presetter_hex_to_rgba(t_jrgba *color, const char *hex, double alpha);
void presetter_resolve_color(p_color pc, t_jrgba *color, double alpha);

#endif
