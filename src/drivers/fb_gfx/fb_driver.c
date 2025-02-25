#include <drivers/fb_gfx.h>
#include <bootloader/limine.h>
#include <libc/assert.h>
#include <libc/math.h>

struct limine_framebuffer *fb;
void *fb_addr;

void gfx_pixel(u32 x, u32 y, u32 color) {
  u32 *pixel = (u32 *)(fb_addr + (y * fb->pitch) + (x * 4));
  *pixel = color;
}

void gfx_line(u32 x1, u32 y1, u32 x2, u32 y2, u32 color) {
  int dx = x2 - x1;
  int dy = y2 - y1;
  int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
  int xinc = dx / steps;
  int yinc = dy / steps;
  int x = x1;
  int y = y1;
  for (int i = 0; i <= steps; i++) {
    gfx_pixel(x, y, color);
    x += xinc;
    y += yinc;
  }
}

void gfx_rect(u32 x, u32 y, u32 w, u32 h, u32 color) {
  gfx_line(x, y, x + w, y, color);
  gfx_line(x, y, x, y + h, color);
  gfx_line(x + w, y, x + w, y + h, color);
  gfx_line(x, y + h, x + w, y + h, color);
}

void gfx_fill_rect(u32 x, u32 y, u32 w, u32 h, u32 color) {
  for (u32 i = 0; i < h; i++) {
    for (u32 j = 0; j < w; j++) {
      gfx_pixel(x + j, y + i, color);
    }
  }
}

void gfx_fill(u32 color) {
  for (u32 i = 0; i < fb->height; i++) {
    for (u32 j = 0; j < fb->width; j++) {
      gfx_pixel(j, i, color);
    }
  }
}

u32 gfx_screen_width() {
  return fb->width;
}
u32 gfx_screen_height() {
  return fb->height;
}

void init_fb_gfx(){
  assert(boot_ctx.fb_info->framebuffer_count > 0, "No framebuffer found");
  fb = boot_ctx.fb_info->framebuffers[0];
  fb_addr = (void *)fb->address;
}
