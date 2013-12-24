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

// lcd control lines (with inversion for nCS, nRD, nWR managed by these macros)
void _lcd_select(void) { GPIO_ResetBits(GPIOC, GPIO_Pin_9); }
void _lcd_deselect(void) { GPIO_SetBits(GPIOC, GPIO_Pin_9); }
void _lcd_rs_set(void) { GPIO_SetBits(GPIOC, GPIO_Pin_8); }
void _lcd_rs_reset(void) { GPIO_ResetBits(GPIOC, GPIO_Pin_8); }
void _lcd_rd_en(void) { GPIO_ResetBits(GPIOC, GPIO_Pin_11); }
void _lcd_rd_dis(void) { GPIO_SetBits(GPIOC, GPIO_Pin_11); }
void _lcd_wr_en(void) { GPIO_ResetBits(GPIOC, GPIO_Pin_10); }
void _lcd_wr_dis(void) { GPIO_SetBits(GPIOC, GPIO_Pin_10); }

void _lcd_bl_en(void) { GPIO_SetBits(GPIOC, GPIO_Pin_12); }
void _lcd_bl_dis(void) { GPIO_ResetBits(GPIOC, GPIO_Pin_12); }

void lcd_gpio_conf(GPIOMode_TypeDef mode);

void _lcd_put_data(u16 data) {
    // data[0-7] -> GPIOC[0-7], data[8-15] -> GPIOB[8-15]
    GPIOB->ODR = (GPIOB->ODR&0x00ff)|(data&0xff00);
    GPIOC->ODR = (GPIOC->ODR&0xff00)|(data&0x00ff);
}

u16 _lcd_read_data(void) {
    lcd_gpio_conf(GPIO_Mode_IN_FLOATING);
    u16 result = (GPIOB->IDR&0xff00)|(GPIOC->IDR&0x00ff);
    lcd_gpio_conf(GPIO_Mode_Out_PP);
    return result;
}

// assume that _lcd_select() was done before it
void _lcd_tx_reg(u8 addr) {
    _lcd_put_data(addr);
    _lcd_rs_reset();
    _lcd_wr_en();
    _lcd_wr_dis();
    _lcd_rs_set();
}

// assume that _lcd_tx_reg(u8) was done before it
void _lcd_tx_data(u16 data) {
    _lcd_put_data(data);
    _lcd_wr_en();
    _lcd_wr_dis();
}

// assume that _lcd_tx_reg(u8) was done before it
u16 _lcd_rx_data(void) {
    _lcd_rd_en();
    u16 result = _lcd_read_data();
    _lcd_rd_dis();
    return result;
}

u16 lcd_read_reg(u8 addr) {
    _lcd_select();

    _lcd_tx_reg(addr);
    u16 result = _lcd_rx_data();

    _lcd_deselect();

    return result;
}

void lcd_write_reg(u8 addr, u16 data) {
    _lcd_select();

    _lcd_tx_reg(addr);
    _lcd_tx_data(data);

    _lcd_deselect();
}

void lcd_write_to_gram(u16 *data, u32 len) {
    _lcd_select();

    // write to GRAM operation
    _lcd_tx_reg(0x22);

    for(u32 i = 0; i < len; i++) {
        _lcd_tx_data(data[i]);
    }

    _lcd_deselect();
}

void lcd_gpio_conf(GPIOMode_TypeDef mode) {
    GPIO_InitTypeDef gpio_conf;

    gpio_conf.GPIO_Speed = GPIO_Speed_2MHz;
    gpio_conf.GPIO_Mode = mode;
    gpio_conf.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11
            | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOB, &gpio_conf);

    gpio_conf.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
            GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOC, &gpio_conf);
}

u16 lcd_init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef gpio_conf;

    gpio_conf.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_conf.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_conf.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_Init(GPIOC, &gpio_conf);

    lcd_gpio_conf(GPIO_Mode_Out_PP);

    // to init state (0xffff on db0-15, backlit is disabled, nCS, nWR, nRD and RS are high)
    _lcd_bl_dis();
    _lcd_put_data(0xffff);
    _lcd_deselect();
    _lcd_wr_dis();
    _lcd_rd_dis();
    _lcd_rs_set();

    // osc enable
    _lcd_bl_dis();
    lcd_write_reg(0x00, 0x0001);
    delay_ms(100);
    u16 lcd_code = lcd_read_reg(0x00);
    delay_ms(100);

    // driver output control (S720-S1)
    lcd_write_reg(0x01, 0x0100);

    // driving wave control (line inv)
    lcd_write_reg(0x02, 0x0700);

    // entry mode (horiz, dir(h+,v+), hwm-, bgr+)
    lcd_write_reg(0x03, 0x1030);

    // resize (off)
    lcd_write_reg(0x04, 0x0000);

    // display control 2 (skip 2 lines on front porch and on back porch)
    lcd_write_reg(0x08, 0x0202);

    // display control 3-4 (scan mode normal, fmark off)
    lcd_write_reg(0x09, 0x0000);
    lcd_write_reg(0x0a, 0x0000);

    // RGB disp iface control (int clock, sys int, 16bit)
    lcd_write_reg(0x0c, 0x0001);

    // frame marker  position (isn't used)
    lcd_write_reg(0x0d, 0x0000);

    // RGB disp iface control 2 (all def, we don't use rgb)
    lcd_write_reg(0x0f, 0x0000);

    // power on seq
    lcd_write_reg(0x07, 0x0021);
    delay_ms(10);

    // turn on power supply and configure it (enable sources, set contrast, power supply on)
    lcd_write_reg(0x10, 0x16b0);
    // set normal voltage and max dcdc freq
    lcd_write_reg(0x11, 0x0007);
    // internal vcomh (see 0x29), pon, gray level (0x08)
    lcd_write_reg(0x12, 0x0118);
    // set vcom to 0.92 * vreg1out
    lcd_write_reg(0x13, 0x0b00);
    // vcomh = 0.69 * vreg1out
    lcd_write_reg(0x29, 0x0000);

    // set x and y range
    lcd_write_reg(0x50, 0);
    lcd_write_reg(0x51, LCD_WIDTH-1);
    lcd_write_reg(0x52, 0);
    lcd_write_reg(0x53, LCD_HEIGHT-1);

    // gate scan control (scan direction, display size)
    lcd_write_reg(0x60, 0x2700);
    lcd_write_reg(0x61, 0x0001);
    lcd_write_reg(0x6a, 0x0000);

    // partial displays off
    for(u8 addr = 0x80; addr < 0x86; addr++) {
        lcd_write_reg(addr, 0x0000);
    }

    // panel iface control (19 clock/line)
    lcd_write_reg(0x90, 0x0013);

    // lcd timings
    lcd_write_reg(0x92, 0x0000);
    lcd_write_reg(0x93, 0x0001);
    lcd_write_reg(0x95, 0x0110);
    lcd_write_reg(0x97, 0x0000);
    lcd_write_reg(0x98, 0x0000);

    lcd_write_reg(0x07, 0x0133);

    // turn on backlit after init done
    _lcd_bl_en();

    return lcd_code;
}
