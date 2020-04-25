// // Simple uart serial read/write test.

// #include <iostream>
// #include <string>

// #include "uart.h"

// int main()
// {
// 	std::string port_name = "COM1";

// 	// Create a uart device:
// 	uart_dev* uart_device;
// 	uart_init(&uart_device);
// 	uart_set_baud(uart_device, 115200);
// 	uart_set_flowctrl(uart_device, FC_NONE);
//     uart_set_parity(uart_device, false, PM_NONE);
//     uart_set_stopbits(uart_device, 1);
//     uart_set_databits(uart_device, 8);
//     uart_set_verbosity(uart_device, false);

// 	// Open the uart device at the specified port/device name:
// 	uart_open(uart_device, port_name);

// 	if (uart_config(uart_device))
// 	{
// 		std::string s_help = "help\r";
// 		const char* c_data = s_help.c_str();
// 		void* out_buffer = (void*)c_data;
// 		unsigned long out_size = s_help.length();

// 		uart_write(uart_device, out_buffer, out_size);

// 		bool reading;
// 		do
// 		{
// 			char read_buf[1];
// 			read_buf[0] = '\0';

// 			void* data = (void*)(&read_buf[0]);

// 			reading = uart_read(uart_device, &data, 1);

// 			std::cout << *((char*)data);

// 		} while (reading);
// 	}

	
// 	uart_close(uart_device);
// 	uart_free(uart_device);

// 	return 0;
// }
