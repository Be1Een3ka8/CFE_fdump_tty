// uart_win.h: Minimal C++ Uart library. Author Gerallt Franke.
// Date: 24 April 2020 10:24 UTC. 

#ifndef WIN_UART_H
#define WIN_UART_H

#ifdef WIN32

// Windows headers.
#include <windows.h>

//#define UART_TRACING

const bool OVERLAPPED_IO = false; // If true, enables overlapped IO to allow threading on device or asynchronous I/O.

// Redefine uart_dev with windows implementation.
struct uart_dev
{
	bool overlapped_io;
	HANDLE win_handle;
    bool com_opened;
	DWORD baud;
	int32_t flow_control;
	bool parity;
    int32_t parity_mode;
	uint32_t stop_bits;
	uint32_t data_bits;
	std::string port_name;
    bool verbose;
};

std::string win32_get_error_msg(DWORD last_error);

#endif

#endif
