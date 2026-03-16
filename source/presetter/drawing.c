#include "ext_strings.h"
#include "jgraphics.h"

void presetter_trim_text_right(t_jgraphics *g, const char *text, double max_width, char *out, long outsize) {
    double text_width;
    double text_height;

    jgraphics_text_measure(g, text, &text_width, &text_height);

    if (text_width > max_width) {
        snprintf_zero(out, outsize, "%s", text);

        char with_ellipsis[1048];
        snprintf_zero(with_ellipsis, sizeof(with_ellipsis), "%s...", out);
        jgraphics_text_measure(g, with_ellipsis, &text_width, &text_height);

        while (text_width > max_width && strlen(out) > 0) {
            out[strlen(out) - 1] = '\0';
            snprintf_zero(with_ellipsis, sizeof(with_ellipsis), "%s...", out);
            jgraphics_text_measure(g, with_ellipsis, &text_width, &text_height);
        }

        snprintf_zero(out, outsize, "%s...", out);
    } else {
        snprintf_zero(out, outsize, "%s", text);
    }
}
