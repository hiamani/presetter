#ifndef PRESETTER_FILTERS
#define PRESETTER_FILTERS

#include "ext_dictionary.h"

#include "structs.h"

void presetter_filters_clear_deferred(t_presetter *p, t_symbol *s, short arc, t_atom *argv);

t_dictionary *presetter_lookup_filter_slot(t_presetter *p, long index);

bool presetter_find_filter_by_name(t_presetter *p, t_symbol *s, t_filter_result *result);

/* Add / Remove Filters */

bool presetter_add_filter_sym(t_presetter *p, t_symbol *name, long index);

bool presetter_rename_filter_sym(t_presetter *p, t_symbol *so, t_symbol *sn);

bool presetter_rename_filter_idx(t_presetter *p, long idx, t_symbol *sn);

bool presetter_clear_filter_sym(t_presetter *p, t_symbol *s);

bool presetter_remove_filter_sym(t_presetter *p, t_symbol *s);

bool presetter_remove_filter_idx(t_presetter *p, long idx);

bool presetter_set_filter_slot_sym(t_presetter *p, t_symbol *s, long slot);

bool presetter_set_filter_slot_idx(t_presetter *p, long idx, long slot);

bool presetter_drop_filter_slot_sym(t_presetter *p, t_symbol *s, long slot);

bool presetter_drop_filter_slot_idx(t_presetter *p, long idx, long slot);

/* Apply / Reset Filters*/

void presetter_set_clear_filter_status(t_presetter *p);

bool presetter_apply_filter_sym(t_presetter *p, t_symbol *s);

bool presetter_apply_filter_idx(t_presetter *p, long idx);

bool presetter_reset_filter_sym(t_presetter *p, t_symbol *s);

bool presetter_reset_filter_idx(t_presetter *p, long idx);

void presetter_reset_filter_all(t_presetter *p);

bool presetter_filtered_cell(t_presetter *p, long cell_idx);

void presetter_handle_filter_rename(t_presetter *p);

#endif
