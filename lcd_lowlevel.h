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

#ifndef LCD_LOWLEVEL_H_
#define LCD_LOWLEVEL_H_

u16 lcd_read_reg(u8 addr);
void lcd_write_reg(u8 addr, u16 data);
void lcd_write_to_gram(u16 *data, u32 len);

void _lcd_bl_en(void);
void _lcd_bl_dis(void);

void _lcd_select(void);
void _lcd_deselect(void);
void _lcd_tx_reg(u8 addr);
void _lcd_tx_data(u16 data);
u16 _lcd_rx_data(void);

#endif /* LCD_LOWLEVEL_H_ */
