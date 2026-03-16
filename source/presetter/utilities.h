#include "structs.h"

t_symbol *presetter_long_to_sym(long i);

/* Redraw Utilities */

void presetter_redraw_deferred(t_presetter *p, t_symbol *s, short arc, t_atom *argv);

/* Find pattrstorage */

t_object *presetter_find_pattrstorage(t_presetter *p);
