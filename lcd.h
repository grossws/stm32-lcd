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

#ifndef LCD_H_
#define LCD_H_

#define LCD_WIDTH 240
#define LCD_HEIGHT 320

u16 lcd_init(void);

void lcd_set_cursor(u16 x, u16 y);
void lcd_set_window(u16 left, u16 top, u16 right, u16 bottom);

void lcd_fill(u32 color);
void lcd_rect(u16 left, u16 top, u16 right, u16 bottom);
void lcd_put_char_at(u32 data, u16 x, u16 y);

u32 lcd_get_fg(void);
u32 lcd_get_bg(void);
void lcd_set_fg(u32 color);
void lcd_set_bg(u32 color);

#endif /* LCD_H_ */
