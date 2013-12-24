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

#include "lcd.h"
#include "term.h"
#include "uart.h"
#include "time.h"
#include "misc.h"

#define BG_COLOR 0x0033cc66
#define FG_COLOR 0x00000000

void init(void) {
    uart_init();

    time_init();

    lcd_init();
    delay_ms(100);
    lcd_set_fg(FG_COLOR);
    lcd_set_bg(BG_COLOR);
    lcd_fill(BG_COLOR);

    u8 *str = (u8 *)"stm32-lcd";
    lcd_term_set_cursor(0, 10);
    lcd_term_put_str(str, 9);
}

int main(void) {
    init();

    // all is handled by interrupts (USART1 and TIM2)
    for(;;);
    return 0;
}
