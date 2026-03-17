#ifndef PRESETTER_UTILITIES
#define PRESETTER_UTILITIES

#include "structs.h"

t_symbol *presetter_long_to_sym(long i);

void presetter_redraw_deferred(t_presetter *p, t_symbol *s, short arc, t_atom *argv);

short presetter_get_patcher_path(t_presetter *p);

t_object *presetter_find_pattrstorage(t_presetter *p);

bool presetter_resolve_filter_path(t_presetter *p, const char *filename, short *out_path, char *out_name);

void presetter_read_filters_dictionary(t_presetter *p);

void presetter_write_filters_dictionary(t_presetter *p);

#endif
