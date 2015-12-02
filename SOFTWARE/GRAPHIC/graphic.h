#ifndef _H_GRAPHIC
#define _H_GRAPHIC
#include "sys.h"
void gui_draw_dialog(u16 x, u16 y, u16 width, u16 height, u8 size, u8 * str, u32 charColor, u32 dialogColor);
void gui_clear(u32 color);
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color);

#endif
