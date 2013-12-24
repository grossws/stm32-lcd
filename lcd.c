/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Konstantin Gribov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stm32f10x.h>
#include "stm32f10x_conf.h"
#include "misc.h"
#include "lcd.h"
#include "lcd_lowlevel.h"
#include "lcd_char.h"

u32 orig_fg_color = 0x00ffffff;
u32 orig_bg_color = 0x00000000;
u16 fg_color = 0xffff;
u16 bg_color = 0x0000;


static inline u16 lcd_pixel_from_rgb(u8 r, u8 g, u8 b) {
    return (((u16)(r&0xf8))<<8)|(((u16)(g&0xfc))<<3)|((b>>3)&0x1f);
}

static inline u16 lcd_pixel_from_rgb32(u32 rgb) {
    return lcd_pixel_from_rgb((u8)((rgb>>16)&0xff), (u8)((rgb>>8)&0xff), (u8)(rgb&0xff));
}

u32 lcd_get_fg(void) { return orig_fg_color; }
u32 lcd_get_bg(void) { return orig_bg_color; }

void lcd_set_fg(u32 color) {
    orig_fg_color = color;
    fg_color = lcd_pixel_from_rgb32(color);
}

void lcd_set_bg(u32 color) {
    orig_bg_color = color;
    bg_color = lcd_pixel_from_rgb32(color);
}

void lcd_set_cursor(u16 x, u16 y) {
    lcd_write_reg(0x20, x);
    lcd_write_reg(0x21, y);
}

void lcd_set_window(u16 left, u16 top, u16 right, u16 bottom) {
    lcd_write_reg(0x50, sat(left, 0, LCD_WIDTH-1));
    lcd_write_reg(0x51, sat(right-1, 0, LCD_WIDTH-1));
    lcd_write_reg(0x52, sat(top, 0, LCD_HEIGHT-1));
    lcd_write_reg(0x53, sat(bottom-1, 0, LCD_HEIGHT-1));
}

void lcd_fill(u32 color) {
    u16 data = lcd_pixel_from_rgb32(color);

    lcd_set_window(0, 0, 0xffff, 0xffff);
    lcd_set_cursor(0, 0);

    _lcd_select();

    _lcd_tx_reg(0x22);
    for(u32 i = 0; i < LCD_WIDTH*LCD_HEIGHT; i++) {
        _lcd_tx_data(data);
    }

    _lcd_deselect();
}

void lcd_rect(u16 left, u16 top, u16 right, u16 bottom) {
    u16 xs = right - left;
    u16 ys = bottom - top;

    lcd_set_window(left, top, right, bottom);
    lcd_set_cursor(left, top);

    _lcd_select();
    _lcd_tx_reg(0x22);
    for(u32 i = 0; i < xs*ys; i++) {
        _lcd_tx_data(fg_color);
    }
    _lcd_deselect();
}

void lcd_put_char_at(u32 data, u16 x, u16 y) {
    u8 xsize, ysize;
    u8 *char_img;
    lcd_get_char(data, &xsize, &ysize, &char_img);

    lcd_set_cursor(x, y);
    lcd_set_window(x, y, x + xsize, y + ysize);

    _lcd_select();
    _lcd_tx_reg(0x22);
    // works only for 8xN fonts
    for(u8 i = 0; i < ysize; i++) {
        u8 str = char_img[i];
        for(u8 j = 0; j < xsize; j++) {
            _lcd_tx_data((str&(1<<(xsize-j-1)))?fg_color:bg_color);
        }
    }
    _lcd_deselect();
}
