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
#include "term.h"
#include "lcd.h"

// terminal cursor position
u8 term_row = 0;
u8 term_col = 0;

// escape sequence handling vars
u8 escape_seq = 0;
u8 buf[10];

// terminal dimensions
#define TERM_COL_NUM 30
#define TERM_ROW_NUM 18

// terminal coords to lcd coords
#define TERM_X(c) ((c)*8)
#define TERM_Y(r) ((2+(r))*16)

// terminal tab size
#define TERM_TAB_SIZE 3

void lcd_term_inc_pos() {
    term_col++;
    if(term_col >= TERM_COL_NUM) {
        term_row++;
        term_col -= TERM_COL_NUM;
    }
    if(term_row >= TERM_ROW_NUM) {
        term_row -= TERM_ROW_NUM;
    }
}

u8 lcd_term_row(void) { return term_row; }
u8 lcd_term_col(void) { return term_col; }

void lcd_term_set_cursor(u8 r, u8 c) {
    term_row = r % TERM_ROW_NUM;
    term_col = c % TERM_COL_NUM;
}

void lcd_term_put_str(u8 *d, u8 len) {
    for(u8 i = 0; i < len; i++) {
        if(d[i] > 0x1f) {
            lcd_put_char_at(d[i], TERM_X(term_col), TERM_Y(term_row));
            lcd_term_inc_pos();
        } else if(d[i] == 0x0a) {
            lcd_term_set_cursor(term_row + 1, term_col);
        } else if (d[i] == 0x0d) {
            lcd_term_set_cursor(term_row, 0);
        } else if (d[i] == 0x09) {
            u8 tab_stop = term_col + (TERM_TAB_SIZE - term_col % TERM_TAB_SIZE);
            if(tab_stop < TERM_COL_NUM) {
                lcd_term_set_cursor(term_row, tab_stop);
            } else {
                lcd_term_set_cursor(term_row + 1, 0);
            }
        }
    }
}

void lcd_term_clear(void) {
    lcd_fill(lcd_get_bg());
}

void lcd_term_flush_str(void) {
    u32 saved_fg_color = lcd_get_fg();
    lcd_set_fg(lcd_get_bg());
    lcd_rect(TERM_X(term_col), TERM_Y(term_row), TERM_X(TERM_COL_NUM), TERM_Y(term_row+1));
    lcd_set_fg(saved_fg_color);
}

void handle_byte(u8 data) {
    if((!escape_seq) && (data == 0x1b)) {
        escape_seq = 1;
    } else if (escape_seq == 1) {
        buf[escape_seq] = data;
        escape_seq++;
        if(data != '[') {
            escape_seq = 0;
        }
    } else if (escape_seq == 2) {
        switch(data) {
        case 'A':
            lcd_term_set_cursor(lcd_term_row()-1, lcd_term_col());
            break;
        case 'B':
            lcd_term_set_cursor(lcd_term_row()+1, lcd_term_col());
            break;
        case 'C':
            lcd_term_set_cursor(lcd_term_row(), lcd_term_col()+1);
            break;
        case 'D':
            lcd_term_set_cursor(lcd_term_row(), lcd_term_col()-1);
            break;
        case 'H':
            lcd_term_set_cursor(0, 0);
            break;
        case 'J':
            lcd_term_clear();
            break;
        case 'K':
            lcd_term_flush_str();
            break;
        case 'X':
        case 'Y':
            buf[escape_seq] = data;
            escape_seq++;
            return;
        }
        escape_seq = 0;
    } else if(escape_seq == 3) {
        buf[escape_seq] = data;
        escape_seq++;
    } else if(escape_seq == 4) {
        u8 row = (buf[2] == 'Y') ? buf[3] - 037 : data - 037;
        u8 col = (buf[2] == 'Y') ? data - 037 : buf[3] - 037;
        lcd_term_set_cursor(row, col);
        escape_seq = 0;
    } else {
        lcd_term_put_str(&data, 1);
    }
}
