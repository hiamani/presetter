#include "colors.h"
#include "ext_proto.h"
#include "jgraphics.h"

void presetter_hex_to_rgba(t_jrgba *color, const char *hex, double alpha) {
    unsigned int r, g, b;
    const char *h = (hex[0] == '#') ? hex + 1 : hex;
    sscanf(h, "%2x%2x%2x", &r, &g, &b);
    jrgba_set(color, r / 255.0, g / 255.0, b / 255.0, alpha);
}

void presetter_resolve_color(p_color pc, t_jrgba *color, double alpha) {
    if (pc.type == COLOR_HEX) {
        presetter_hex_to_rgba(color, pc.value, alpha);
    } else {
        t_jrgba off;
        jcolor_getcolor(gensym(pc.value), color, &off);
        color->alpha = alpha;
    }
}
