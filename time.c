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
#include "time.h"

u32 time = 0;

void time_init(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitTypeDef timer_conf;
    timer_conf.TIM_Prescaler = 72; // if fck = 8 MHz => 1 MHz
    timer_conf.TIM_CounterMode = TIM_CounterMode_Up;
    timer_conf.TIM_Period = 1000; // 1 kHz
    timer_conf.TIM_ClockDivision = TIM_CKD_DIV1;
    timer_conf.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM2, &timer_conf);
    TIM_Cmd(TIM2, ENABLE);

    NVIC_InitTypeDef nvic_conf;
    nvic_conf.NVIC_IRQChannel = TIM2_IRQn;
    nvic_conf.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_conf.NVIC_IRQChannelSubPriority = 1;
    nvic_conf.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_conf);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

void TIM2_IRQHandler(void) {
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        time++;
    }
}
