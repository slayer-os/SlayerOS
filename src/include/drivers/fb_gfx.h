#ifndef FB_GFX_DRIVER_H
#define FB_GFX_DRIVER_H
#include <libc/types.h>

void init_fb_gfx();
void gfx_pixel(u32 x, u32 y, u32 color);
void gfx_line(u32 x1, u32 y1, u32 x2, u32 y2, u32 color);
void gfx_rect(u32 x, u32 y, u32 w, u32 h, u32 color);
void gfx_fill_rect(u32 x, u32 y, u32 w, u32 h, u32 color);
void gfx_fill(u32 color);

u32 gfx_screen_width();
u32 gfx_screen_height();

#endif
