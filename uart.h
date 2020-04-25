// uart.h: Minimal C++ Uart library. Author Gerallt Franke.
// Date: 24 April 2020 10:24 UTC. 

// This code is using minimal C++ features.

#ifndef UART_H
#define UART_H

// C++ headers.
#include <iostream>
#include <string>

enum FlowControl
{
	FC_NONE = 0,
	FC_XON_XOFF = 1,
	FC_RTS_CTS = 2,
	FC_DSR_DTR = 3
};

enum ParityMode
{
    PM_NONE,
    PM_EVEN,
    PM_ODD
};

struct uart_dev;

// Interface between platforms.
void uart_init(uart_dev** dev);
void uart_set_baud(uart_dev* dev, uint32_t baud_rate);
void uart_set_flowctrl(uart_dev* dev, int32_t flow_control);
void uart_set_parity(uart_dev* dev, bool parity, int32_t parity_mode);
void uart_set_stopbits(uart_dev* dev, uint32_t stop_bits);
void uart_set_databits(uart_dev* dev, uint32_t data_bits);
void uart_set_verbosity(uart_dev* dev, bool verbosity);
bool uart_open(uart_dev* dev, std::string port_name);
bool uart_config(uart_dev* dev);
unsigned long uart_write(uart_dev* dev, void* data, unsigned long bytes_to_write);
unsigned long uart_read(uart_dev* dev, void** data, unsigned long bytes_to_read);
void uart_close(uart_dev* dev);
void uart_free(uart_dev* dev);

// Platforms.
#include "uart_nix.h" // POSIX implementation.
#include "uart_win.h" // Windows implementation.

#endif
