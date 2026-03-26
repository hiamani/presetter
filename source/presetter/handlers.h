#ifndef PRESETTER_HANDLERS
#define PRESETTER_HANDLERS

#include "structs.h"

/// Attribute Methods

t_max_err presetter_set_pattrstorage(t_presetter *p, t_object *attr, long argc, t_atom *argv);

t_max_err presetter_set_filename(t_presetter *p, t_object *attr, long argc, t_atom *argv);

void presetter_assist(t_presetter *x, void *b, long io, long index, char *s);

/// Notify

t_max_err presetter_notify(t_presetter *p, t_symbol *s, t_symbol *msg, void *sender, void *data);

/// Message Methods

void presetter_loadbang(t_presetter *p, long action);

void presetter_bang(t_presetter *p);

void presetter_read(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

void presetter_recall(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

void presetter_slotname(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

// Filters

void presetter_addfilter(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

void presetter_renamefilter(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

void presetter_clearfilter(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

void presetter_removefilter(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

void presetter_addfilterslot(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

void presetter_removefilterslot(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

void presetter_applyfilter(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

void presetter_resetfilter(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

void presetter_resetfilters(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

void presetter_readfilters(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

void presetter_readfiltersagain(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

void presetter_writefilters(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

void presetter_writefiltersagain(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

/// Catchall

void presetter_anything(t_presetter *p, t_symbol *s, long argc, t_atom *argv);

/// Callback Methods

void presetter_handle_preset_recall(t_presetter *p, long cell_idx);

void presetter_handle_preset_store(t_presetter *p, long cell_idx);

void presetter_handle_preset_delete(t_presetter *p, long cell_idx);

void presetter_handle_preset_rename(t_presetter *p);

void presetter_handle_filter_rename(t_presetter *p);

/// Pointer Event Methods

void presetter_clear_confirm(t_presetter *p);

void presetter_mousedown(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers);

void presetter_mouseup(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers);

void presetter_mousemove(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers);

void presetter_mouseleave(t_presetter *p, t_object *patcherview, t_pt pt, long modifiers);

/// Keyboard Methods

long presetter_key(t_presetter *p, t_object *patcherview, long keycode, long modifiers, long textcharacter);

/// Paint

void presetter_paint(t_presetter *p, t_object *patcherview);

#endif
