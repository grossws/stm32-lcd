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
#include "uart.h"
#include "term.h"

void uart_init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitTypeDef gpio_conf;
    gpio_conf.GPIO_Speed = GPIO_Speed_50MHz;

    // Rx pin
    gpio_conf.GPIO_Pin = GPIO_Pin_10;
    gpio_conf.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio_conf);

    // Tx pin
    gpio_conf.GPIO_Pin = GPIO_Pin_9;
    gpio_conf.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpio_conf);

    USART_InitTypeDef uart_conf;
    uart_conf.USART_BaudRate = 9600;
    uart_conf.USART_WordLength = USART_WordLength_8b;
    uart_conf.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uart_conf.USART_Parity = USART_Parity_No;
    uart_conf.USART_StopBits = USART_StopBits_1;
    uart_conf.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &uart_conf);
    USART_Cmd(USART1, ENABLE);

    NVIC_InitTypeDef nvic_conf;
    nvic_conf.NVIC_IRQChannel = USART1_IRQn;
    nvic_conf.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_conf.NVIC_IRQChannelSubPriority = 2;
    nvic_conf.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_conf);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void USART1_IRQHandler(void) {
    u8 data = USART1->DR;
    uart_write_byte(data);
    handle_byte(data);
}

void uart_write_byte(u8 b) {
    while (!(USART1->SR & USART_SR_TC));
    USART1->DR = b;
}
