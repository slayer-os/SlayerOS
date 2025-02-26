#include <drivers/fb_gfx.h>

#define PIXEL_SIZE    6
#define HEIGHT_MULT   2

void draw_block(u32 x, u32 y, u32 color) {
  for (int i = 0; i < PIXEL_SIZE; i++) {
    for (int j = 0; j < PIXEL_SIZE; j++) {
      gfx_pixel(x + j, y + i, color);
    }
  }
}

void draw_ascii_logo(u32 x, u32 y, u32 scale, u32 color) {
  const char *logo[] = {
      "                                          ######   ",
      "                                         #*###**#  ",
      "                                        ##    #**# ",
      "                                        #      #**#",
      "                                                #*#",
      "                                               #**#",
      "                                          ####**#  ",
      "                       #                ##**##     ",
      "                     ##*#             ##*##        ",
      "                    #****#          ##*##          ",
      "                  #****##*#        #*##            ",
      "                ##***#   #*#     #**#              ",
      "               #***#      #*#  #**#                 ",
      "             #***#         #*#**#                    ",
      "           #***##          ##**#                     ",
      "  ###    ##**##           #**#**#                    ",
      "###     #***#           #**#  #**#                   ",
      "#*#   #***#            #**#    #**#                  ",
      "#**###**#            #**#        #*##                ",
      "#*****#            ##*##          #**#      #        ",
      " ##*##            #**#             ##**#    #        ",
      "                #**###################***####      ",
      "               #***************************#       ",
      NULL
  };
  
  int x_scale = scale;
  int y_scale = scale * (gfx_screen_width() / gfx_screen_height());
  for (int row = 0; logo[row] != NULL; row++) {
    for (int col = 0; logo[row][col] != '\0'; col++) {
      if (logo[row][col] != ' ') {
        for (int i = 0; i < y_scale; i++) {
          for (int j = 0; j < x_scale; j++) {
            draw_block(x + col * PIXEL_SIZE * x_scale + j * PIXEL_SIZE, y + (row * HEIGHT_MULT + i) * PIXEL_SIZE, color);
          }
        }
      }
    }
  }
}
