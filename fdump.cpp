// fdump.cpp: Author Gerallt Franke.
// Date: 24 April 2020 10:24 UTC. 
// Description: Clone flash memory on CFE terminal using fdump commands over serial tty.
// Note: 1. Make sure you have added current $USER to dialout group and have a working serial tty interface.
//		 2. All code needed is in the one file which can make it simpler to compile with 
//			Clang or GCC, and simpler to add to Makefile. 
//			All this may be refactored later if there are other utilities.
// Usage: Boot into CFE using another program like Putty to interact with the serial tty.
//		  When you have a CFE> console, quit Putty and run this program instead.
//		  You can have both programs running on the same tty 
//		  however they will both compete and the data will likely get scrambled.
//
//		  This program will automate the sending of command(s) to CFE to extract flash memory, 
//		  and save it locally in an image. 
//
//		  You may need to change the baud rate (DEFAULT_BAUD B115200), if it's a slow speed you will 
//		  need to set VTIME_APPLIED to VTIME_SLOW and recompile.
//
// 		  It works a bit like Unix dd, to read the flash you will need to at least specify:
//			 1. The device name e.g. flash0, or flash0.boot, or flash0.nvram, etc.
//			 2. Block size and the size (in bytes) of the data to read.
//
//			 3. It is optional to specify the offset and if to output into an output file.
//			 4. Other settings include: verbose, very verbose, and if to print the data.
//
//		  Block size is a multiple of 16 (BYTES_PER_LINE) and can be set to the 
//		  total size of the data to read. Having small values for block size 
//		  with a really big read decreases the efficency of the read, esp on a serial tty. 

//
// References:
// Serial tty interface uart_nix.cpp is based on code from:
// [1] https://github.com/gbmhunter/CppLinuxSerial
// [2] https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/#vmin-and-vtime-c_cc
//
// Trim functions for std::string copied from:
// [1] https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/
//
// Hex character conversion from stack overflow.
// [1] https://stackoverflow.com/questions/17261798/converting-a-hex-string-to-a-byte-array

#include "fdump.h"

#ifdef POSIX
	void sighandler(int sig)
	{
		if(verbose)
		{
			std::cout << "Signal " << sig << " caught..." << std::endl;	
		}

		continue_cfe = false;
	}
#endif

bool serial_read_single_char(uart_dev* uart_device, char** out)
{
	// Allocate memory for read buffer, set size to read just 1 byte.
	char read_buffer[1];
	read_buffer[0] = '\0'; // memset(&read_buf, '\0', sizeof(read_buf));
	void* data = (void*)&read_buffer[0];

	// Read blocks for a real short time since VTIME=1 (1 is the shortest time to block) 
	// Read blocks until any number of characters are returned since VMIN=0.

	unsigned long num_bytes = uart_read(uart_device, &data, sizeof(read_buffer));

	//if (num_bytes < 0) 
	//{
	//	std::cout << "Error reading: " << strerror(errno) << std::endl;
	//    return false;
	//}
	
	if(num_bytes > 0)
	{
		// Just return a single character.
		*out = &read_buffer[0];

		return true;
	}

	// Nothing to read.
	return false;
}

void serial_read(uart_dev* uart_device)
{
	bool read = true;
	while(read)
	{		
		char* c;
		read = serial_read_single_char(uart_device, &c);
		std::cout << *c;
	}
}

std::string ltrim(const std::string& s)
{
	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string& s)
{
	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string& s)
{
	return rtrim(ltrim(s));
}

bool starts_with(std::string source, std::string compare)
{
	return source.rfind(compare, 0) == 0;
}

int hex_to_int(char input)
{
	if(input >= '0' && input <= '9')
	{
		return input - '0';
	}
	else if(input >= 'a' && input <= 'f')
	{
		return input - 'a' + 10;
	}
	else if(input >= 'A' && input <= 'F')
	{
		return input - 'A' + 10;
	}
	throw std::invalid_argument("Invalid input string");
}

// This function assumes src to be a zero terminated sanitized string with
// an even number of [0-9a-f] characters, and target to be sufficiently large
void hex_to_buffer(const char* src, char* target)
{
	// Convert hex data to a byte buffer.
	while(*src && src[1])
	{
		*(target++) = hex_to_int(*src)*16 + hex_to_int(src[1]);
		src += 2;
	}
}

bool is_printable_ascii_char(char c)
{
	return (c > 31) && (c < 127);
}

void hexbuffer_to_friendlystring(const char* src, char* target)
{
	// Convert hex data to printable c-string.
	while(*src && src[1])
	{
		char byte = hex_to_int(*src)*16 + hex_to_int(src[1]);

		if(!is_printable_ascii_char(byte))
		{
			byte = ' ';
		}

		*(target++) = byte;
		src += 2;
	}
}

void output_file_open()
{
	if(!output_to_file) return;

	// Open and create file if not exist and in binary overwrite mode.
	_of_flash.open((*of_name).c_str(), std::ios::out | std::ios::binary); // Overwrite mode.

	if (_of_flash.fail())
    {
    	throw std::ios_base::failure(std::strerror(errno));
    }

	assert(_of_flash.is_open());
}

void output_file_write(char* buffer, uint32_t real_bytes_per_line)
{
	if(!output_to_file) return;

	//make sure write fails with exception if something is wrong.
    _of_flash.exceptions(_of_flash.exceptions() | std::ios::failbit | std::ifstream::badbit);

	_of_flash.write(buffer, real_bytes_per_line);
}

void output_file_close()
{
	if(!output_to_file) return;

	if(_of_flash.is_open())
	{
		if(verbose)
		{
			std::cout << "Closing handle to file " << (*of_name);
		}
		
		_of_flash.close();	

		if(verbose)
		{
			std::cout << "	[done]" << std::endl;
		}
	}
}

void parse_data_line(std::string line, uint32_t offset, uint32_t* total_bytes_read,
	uint32_t data_length, uint32_t data_character_len, uint32_t block_id)
{
	std::string seq_id = "";
	std::string hex_data = "";

	// Trim just in case.
	line = trim(line);

	if(starts_with(line, FDUMP_CMD + " "))
	{
		line = "";
	}
	else if(starts_with(line, "*** command status ="))
	{
		line = "";
	}
	else if(!line.empty()) 
	{
		// Parse the line to get the hex values, then convert them into bytes.
		//std::cout << line << std::endl;
		try 
		{
			// Get the sequence id from the line (useful for debugging)
			std::smatch match_seq;
			if (std::regex_search(line, match_seq, re_seqid) && match_seq.size() > 1) 
			{
				seq_id = match_seq.str(1);
			}
			
			std::sregex_iterator next(line.begin(), line.end(), re_data);
			std::sregex_iterator end;
			while (next != end) 
			{
				std::smatch match = *next;

				hex_data += match.str();

				next++;
			}
		} 
		catch (std::regex_error& e) 
		{
		  	// Syntax error in the regular expression?
			std::cout << "Regex syntax error?" << std::endl;
		}

		if(hex_data.length() > data_character_len)
		{
			// Ensure length of hex data is data_character_len characters long (32 by default). 
			// Otherwise truncate the remainder. 
			// This is done because the regex matches 
			// extra parts of the line that aren't valid data.
			hex_data.erase(data_character_len, std::string::npos);	
		}

		if(!hex_data.empty())
		{
			uint32_t real_bytes_per_line = hex_data.length() / 2;
			
			if(print_data)
			{
				char* printable_buffer = new char[real_bytes_per_line];

				hexbuffer_to_friendlystring(hex_data.c_str(), printable_buffer);
				printable_buffer[real_bytes_per_line-1] = '\0';

				// Print the seq_id in decimal format.
				uint32_t seq_id_dec = offset + ((block_id-1) * data_length);
				printf("%010u", seq_id_dec);

				// Uncomment to print the seq_id in hex format.
				//std::cout << seq_id;

				std::cout << " " << hex_data;

				printf(" %s", printable_buffer);
				

				std::cout << std::endl;

				delete[] printable_buffer;
			}

			if(output_to_file)
			{
				//char* buffer = new char[BYTES_PER_LINE];
				char* buffer = new char[real_bytes_per_line];

				hex_to_buffer(hex_data.c_str(), &buffer[0]);	

				//printf(" %c", buffer[real_bytes_per_line-1]); // Have a look at last byte in line of data to check if sane.

				output_file_write(&buffer[0], real_bytes_per_line);	

				delete[] buffer;
			}
			
			*total_bytes_read += real_bytes_per_line;
		}
	}
}

void flash_read_block(uart_dev* uart_device, uint32_t offset, uint32_t* total_bytes_read)
{
	uint32_t data_length = BYTES_PER_LINE; // There should be 16 bytes returned per line. So block_size must be a multiple of 16.
	uint32_t data_character_len = data_length * 2; // There should be 32 characters making up the hex data in the line.
	bool read = true;
	std::string line = "";
	uint32_t block_id = 0;

	while(read && continue_cfe)
	{		
		char* c;
		read = serial_read_single_char(uart_device, &c);

		if(read)
		{
			if(*c =='\n' || *c == '\r')
			{
				// Parse lines.
				if(!line.empty())
				{
					// Parse line to strip commands and feedback that is not data.
					parse_data_line(line, offset, total_bytes_read, data_length, data_character_len, block_id);
					
					// Be ready for next line.
					line = "";
					block_id ++;
				}
			}
			else
			{
				// Append the single character to the line parser.
				line += *c;
			}

		}
	}

	if(!continue_cfe) 
	{
		read = false;
	}
}

constexpr uint32_t arg_hash(const char* entropy)
{
	uint32_t iv = 0xF81FFFF;
	uint32_t ik = 0x45;
	uint32_t ik2 = 11;
	uint32_t hash = iv;
	bool hashing = true;
	uint32_t index = -1;
	char c = '\0';

	while(hashing)
	{
		c = entropy[++index];

		if(c != '\0')
		{
			hash ^= (c * ik) ^ ik2;

			if(c == '=')
			{
				// Skip the actual argument value.
				hashing = false;
			}
		}
		else
		{
			hashing = false;
		}
	}
	return hash;
}

std::string* arg_get_value(char* arg)
{
	std::string* value = new std::string(""); // Memory leak if not freed later.
	uint32_t index = 0;
	char c = 0xFF;
	bool searching = true;
	bool ignore_key_name = true;
	int arg_len = strlen(arg);

	arg[arg_len] = '\0';

	while(searching && index < arg_len)
	{
		c = arg[index];

		if(c == '\0')
		{
			searching = false;
		}
		else if (c == '=')
		{
			ignore_key_name = false;
		}
		else
		{
			if(!ignore_key_name)
			{
				if(is_printable_ascii_char(c))
				{
					(*value) += c;
				}
				else
				{
					(*value) += 'Z';
					//searching = false;
				}
				
			}

		}
		index++;
	}

	return value; 
}

typedef void(*OnParseFn)();

void parse_string_arg(char* arg, OnParseFn onParsed, std::string** set)
{
	if(*set != nullptr)
	{
		delete *set;
	}

	//*set = "Stub value";
	(*set) = arg_get_value(arg);
	onParsed();

	if(very_verbose)
	{
		std::cout << " set=" << *arg_get_value(arg);	
	}
}

void parse_uint_arg(char *arg, OnParseFn onParsed, uint32_t* set)
{
	*set = std::stoul(*arg_get_value(arg));
	onParsed();

	if(very_verbose)
	{
		std::cout << " set=" << std::to_string(*set);
	}
}

void display_title()
{
	std::cout << "fdump: Dump flash memory through CFE using tty serial interface." << std::endl;
	std::cout << " Clone flash memory on CFE terminal using " +FDUMP_CMD+ " commands over serial tty." << std::endl << std::endl;
	std::cout << " Make sure you have added current user to dialout group." << std::endl;
	std::cout << " There must be no other running programs accessing tty." << std::endl << std::endl;
}

void show_help()
{
	display_title();
	// Console is 80x24. Sticking to 80 wide output.
    const char* help_view =
    " Description: Clone flash memory on CFE terminal" NEW_LINE 
    "              using fdump commands over serial tty." NEW_LINE
    " Note: 1. Make sure you have added current $USER to dialout group" NEW_LINE 
    "          and have a working serial tty interface." NEW_LINE
    "       2. On Windows you may initially get the weird ERROR_FILE_NOT_FOUND" NEW_LINE
    "          some combination of uninstalling the serial driver and reinstalling" NEW_LINE
    "          unsure if setting the SECURITY_DESCRIPTOR to Everyone group" NEW_LINE
    "          will let you run as a local user instead of as Administrator." NEW_LINE
    "          Reinstalling driver seemed to fix this issue." NEW_LINE
    " Usage: Boot into CFE using another program like Putty" NEW_LINE 
    "        to interact with the serial tty." NEW_LINE
    "  When you have a CFE> console, quit Putty and run this program instead." NEW_LINE
    "  You can have both programs running on the same tty" NEW_LINE
    "  however they will both compete and the data will likely get scrambled." NEW_LINE
    NEW_LINE
    "  This program will automate the sending of command(s)" NEW_LINE
    "  to CFE to extract flash memory, and save it locally in an image." NEW_LINE
    NEW_LINE
    "  You may need to change the baud rate (DEFAULT_BAUD B115200)," NEW_LINE 
    "  if it's a slow speed you will need to set VTIME_APPLIED" NEW_LINE 
    "  to VTIME_SLOW and recompile. There are Comm Timeout settings for Windows." NEW_LINE
    NEW_LINE
    "  It works a bit like Unix dd, to read the flash you will need to" NEW_LINE 
    "  at least specify:" NEW_LINE
    "   1. The device name e.g. flash0, or flash0.boot, or flash0.nvram, etc." NEW_LINE
    "   2. Block size and the size (in bytes) of the data to read." NEW_LINE
    NEW_LINE
    "   3. It is optional to specify the offset and if to output into an output file." NEW_LINE
    "   4. Other settings include: verbose, very verbose, and if to print the data." NEW_LINE
    NEW_LINE
    "Block size is a multiple of 16 (BYTES_PER_LINE) and can be set to the" NEW_LINE 
    "total size of the data to read. Having small values for block size" NEW_LINE
    "with a really big read decreases the efficency of the read, esp on a serial tty." NEW_LINE
    NEW_LINE
    "Required options are: if=, offset=, bs=, size=" NEW_LINE
    " 1. if    - Input flash device name e.g. flash0.boot, flash0.boot2, flasho0.trx," NEW_LINE
    "            flasho0.os, flash0.nvram, etc." NEW_LINE
    " 2. offset/skip - The offset (in bytes) into memory to use." NEW_LINE 
    "                  Most of the time it is fine" NEW_LINE 
    "                  to set offset=0 to read from the beginning." NEW_LINE
    " 3. bs          - The block size. Must be set to multiples of 16 to help parser." NEW_LINE
    "                  Small block sizes and a really large 'size' set" NEW_LINE
    "                  is really slow for reads." NEW_LINE
    " 4. size/count  - The count in bytes of memory to copy." NEW_LINE
    "                  All values are in decimal." NEW_LINE
    "                  Importantly you will likely need to know the exact size" NEW_LINE
    "                  you need to copy which can be hard to figure out." NEW_LINE
    "Valid options are:" NEW_LINE 
    " -h / -help / --help, Display the help and exit." NEW_LINE
    " -of for output file, -v for verbose, -vv for very verbose," NEW_LINE 
    " -l to print the data like hexdump." NEW_LINE
    " -tty=/dev/ttyUSB0   To change the tty serial device on Linux." NEW_LINE
    " -tty=/dev/ttyS0" NEW_LINE
    " -tty=/dev/ttyS1" NEW_LINE 
    NEW_LINE
    " -tty=/dev/ttyU0     To change the tty usb serial device on the BSDs." NEW_LINE 
    "                     Maybe also try ttyU1, or cuaU0, etc." NEW_LINE
	" -tty=COM1           To change the tty usb serial device on Windows." NEW_LINE
	"                       Maybe also try COM2, or anything above COM10 to COM256." NEW_LINE
    NEW_LINE
    " You may also need to change the baud rate and settings which are: 115200 8/N/1" NEW_LINE
    " *To do that you will have to change the code and recompile." NEW_LINE
    NEW_LINE
    " Examples:" NEW_LINE
    "  To list the first 640 bytes of flash0.nvram without saving to file use:" NEW_LINE
    "    ./fdump if=flash0.nvram offset=0 bs=64 size=640 -vv -l" NEW_LINE
    "    ./fdump tty=/dev/ttyS0 if=flash0.nvram offset=0 bs=64 size=640 -vv -l" NEW_LINE
    NEW_LINE
    " To list and store a large block of nvram in a file" NEW_LINE 
    " (large block size to make reads quicker):" NEW_LINE
    "    ./fdump if=flash0.nvram of=f0.nvram.bin offset=0 bs=65536 size=65536 -v -l" NEW_LINE
    NEW_LINE
    " Known Issues: You may press ctrl-c to cancel, however it likely will not" NEW_LINE 
    "               cancel the operation on the CFE console." NEW_LINE
    NEW_LINE
	" Tested on: Ubuntu, FreeBSD, OpenBSD, NetBSD, and Windows."
    NEW_LINE 
	NEW_LINE
    "    Date:   " MY_DATE NEW_LINE
    "  Author:   " MY_NAME NEW_LINE
    " Version:   " MY_VERSION NEW_LINE;

	std::cout << help_view;
}

void free_memory()
{
	if(tty_interface != nullptr)
	{
		delete tty_interface;
	}

	if(device_name != nullptr)
	{
		delete device_name;	
	}
	if(of_name != nullptr)
	{
		delete of_name;	
	}
}

bool parse_program_arguments(int argc, char** argv)
{
	// 1. The device name e.g. flash0, or flash0.boot, or flash0.nvram, etc.
	// 2. Block size and the size (in bytes) of the data to read.
	// 3. It is optional to specify the offset and if to output into an output file.
	// 4. Other settings include: verbose, very verbose, and if to print the data.

	// device_name   if=							Required
	// of_name		 of=							Optional
	// block_size  	 bs=				>16			Required
	// size_in_bytes count= or size=				Required
	// offset 		 skip=  or offset=				Optional
	// tty_interface tty=							optional  default value is DEFAULT_TTY
	// blocks_to_copy = size_in_bytes / block_size; Automatically calculated.
	// output_to_file = true, if of is specified.	Automatically calculated.
	// verbose 		 -v 							Optional
	// very_verbose	 -vv 							Optional
	// print_data 	 -l 							Optional
	
	if(very_verbose)
	{
		std::cout << "There are " << std::to_string(argc) << " arguments specified:" << std::endl; 
	}
    
	OnParseFn show_parsed = []()
	{ 
		if(very_verbose)
		{
			std::cout << "\t\t[parsed]";
		}
	};

	bool got_if = false;
	bool got_size = false;
	bool got_bs = false;
	bool got_offset = false;

	// Do a quick check if help screen selected, and if verbose or very_verbose(-vv) are set.
	for (int i = 0; i < argc; ++i) 
	{
		char* arg = argv[i];
		switch(arg_hash(arg))
		{
			case arg_hash("-h"):
			case arg_hash("-help"):
			case arg_hash("--help"):
			case arg_hash("-version"):
			case arg_hash("--version"):
				// Quit everything pretending to fail to show just the help screen.
				show_help();
				return false; 
				break;
    		case arg_hash("-v"):
    			verbose = true;
    		break;
    		case arg_hash("-vv"):
    			verbose = true;
    			very_verbose = true;
    		break;
		}
	}

    for (int i = 0; i < argc; ++i) 
    {
    	char* arg = argv[i];

    	if(very_verbose)
    	{
    		std::cout << arg << " hash=" << arg_hash(arg);	
    	}
    	
    	//const uint ARG_IF = arg_hash("if=");
    	//const uint ARG_OF = arg_hash("of=");

    	switch(arg_hash(arg))
    	{
    		case arg_hash("if="):
    			// Parse input file from next argument.
    			parse_string_arg(arg, show_parsed, &device_name);
    			got_if = true;
    		break;
    		case arg_hash("of="):
    			// Parse output file from next argument.
    			parse_string_arg(arg, show_parsed, &of_name);
    			if(of_name != nullptr) output_to_file = true;
    		break;
    		case arg_hash("bs="):
				// Parse block size from next argument.
    			parse_uint_arg(arg, show_parsed, &block_size);
    			got_bs = true;
    		break;
    		case arg_hash("count="):
    		case arg_hash("size="):
    			// Parse count from next argument.
    			parse_uint_arg(arg, show_parsed, &size_in_bytes);
    			got_size = true;
    		break;
    		case arg_hash("skip="):
    		case arg_hash("offset="):
    			// Parse offset from next argument.
    			parse_uint_arg(arg, show_parsed, &offset);
    			got_offset = true;
    		break;
    		case arg_hash("-tty="):
    		case arg_hash("tty="):
				std::cout << "###### tty selected " << std::endl;
    			// Parse tty from next argument.
    			parse_string_arg(arg, show_parsed, &tty_interface);
    		break;
    		case arg_hash("-l"):
    			print_data = true;
    		break;
			case arg_hash("-h"): // Already parsed.
			case arg_hash("-help"):
			case arg_hash("--help"):
			case arg_hash("-version"):
			case arg_hash("--version"):
    		case arg_hash("-v"):
    		case arg_hash("-vv"):
			break;
    		default:
    			if(i > 0 && very_verbose) // Might also contain environment info also if i==1 on Windows and Unix too.
    			{
    				std::cout << "\t\tInvalid argument specified.";	
    			}
    	}

    	if(very_verbose)
    	{
    		std::cout << std::endl;	
    	}
    }

    // Input validation.
	if(got_if && got_size && got_bs && got_offset)
	{
		//TODO: Actually validate that argument values are correct before PASS here.

		blocks_to_copy = size_in_bytes / block_size; 

		if(tty_interface == nullptr)
		{
			tty_interface = new std::string(DEFAULT_TTY);
		}

		return true; // PASS
	}
	else
	{
		if(!got_if) std::cout << "Missing if= argument" << std::endl;
		if(!got_size) std::cout << "Missing size= argument" << std::endl;
		if(!got_bs) std::cout << "Missing bs= argument" << std::endl;
		if(!got_offset) std::cout << "Missing offset= argument" << std::endl;

		// FAIL
		std::cout << "FAIL: Not enough parameters set to do a complete run." << std::endl;

		return false;
	}
}

int main(int argc, char **argv)
{
	bool fail = false; // Assume the best.
	uint32_t total_bytes_read = 0;
	
	device_name = new std::string(DEFAULT_DEV_NAME);
	offset = 0;
	output_to_file = false;
	//of_name = new std::string((*device_name) + DEFAULT_FILE_EXT);

	// Increase the block size so as to make read and writes 
	// over serial to be more efficient.
	// e.g a block size of 1 would do 1-byte reads and writes 
	// which is very slow over serial.
	// Set block_size to the same size_in_bytes you need 
	// to copy and it will all be all done in one command.

	//block_size = 0x10000; // must be multiples of 16 to help line parser.
	//size_in_bytes = 0x10000; // The size in bytes to clone from the flash memory.

	// block_size = 0x40000; // must be multiples of 16 to help line parser.
	// size_in_bytes = 0x40000; // The size in bytes to clone from the flash memory.

	// block_size = 18192; // must be multiples of 16 to help line parser.
	// size_in_bytes = 18192; // The size in bytes to clone from the flash memory.
	
	// block_size = 640; // must be multiples of 16 to help line parser.
	// size_in_bytes = 640; // The size in bytes to clone from the flash memory.

	// blocks_to_copy = size_in_bytes / block_size; 

	fail = !parse_program_arguments(argc, argv);

	if(verbose)
	{
		display_title();
	}

	// Instantiate a new uart device and configure it:
	uart_dev* uart_device;
	uart_init(&uart_device);
	uart_set_baud(uart_device, DEFAULT_BAUD);
	uart_set_flowctrl(uart_device, DEFAULT_FLOW_CONTROL);
	uart_set_parity(uart_device, parity, DEFAULT_PARITY_MODE);
	uart_set_stopbits(uart_device, stop_bits);
	uart_set_databits(uart_device, data_bits);
	uart_set_verbosity(uart_device, verbose);

	if(!fail)
	{
		// Open the uart device at the specified port/device name:
		if (uart_open(uart_device, *tty_interface))
		{
			if(uart_config(uart_device))
			{
#ifdef POSIX
				signal(SIGABRT, &sighandler);
				signal(SIGTERM, &sighandler);
				signal(SIGINT, &sighandler);
#endif

				if(verbose)
				{
					std::cout << "Serial tty is configured and ready." << std::endl << std::endl;	
				}

				if(very_verbose)
				{
					std::string help_cmd = HELP_CMD + NEW_LINE;
					std::string showdevs_cmd = SHOW_DEVICES_CMD + NEW_LINE;

					uart_write(uart_device, (void*)help_cmd.c_str(), help_cmd.length());
					serial_read(uart_device);

					uart_write(uart_device, (void*)showdevs_cmd.c_str(), showdevs_cmd.length());
					serial_read(uart_device);
				}

				output_file_open();

				std::cout << "Reading device " << *device_name << std::endl;
				for(uint32_t i=1; i <= blocks_to_copy && continue_cfe; i++)
				{
					std::string s_cmd = FDUMP_CMD + " " + FDUMP_CMD_ARG_OFFSET 
						+ std::to_string(offset) 
						+ " " + FDUMP_CMD_ARG_SIZE
						+ std::to_string(block_size) 
						+ " " + *device_name 
						+ "\r";

					const char* cstr_cmd = s_cmd.c_str();
					uart_write(uart_device, (void*)cstr_cmd, s_cmd.length());
					
					flash_read_block(uart_device, offset, &total_bytes_read);

					offset += block_size;
				}

				output_file_close();

				std::cout << "Done." << std::endl;
				std::cout << "Size in bytes read: " << std::to_string(total_bytes_read) << std::endl;

				if(!continue_cfe)
				{
					if(verbose)
					{
						std::cout << "Broke out of read loop." << std::endl;
						std::cout << "Data retrieved likely incomplete." << std::endl << std::endl;
						std::cout << "Also trying to send ctrl-c to tty " << (*tty_interface) << "..." << std::endl;	
					}
					
					// Write ctrl-c to tty (EXT_CTRL_C is etx - ASCII code 3)
					uart_write(uart_device, (void*)(&EXT_CTRL_C), 1);

					bool cfe_has_quit = true; // Assume the best.
					char* ext;
					if(serial_read_single_char(uart_device, &ext))
					{
						// line might start with CFE> prompt or ext code.
						if(*ext == EXT_CTRL_C || *ext == 'C') 
						{
							if(verbose)
							{
								std::cout << "I think CFE accepted the ctrl-c." << std::endl;	
							}
						}
						else
						{
							if(verbose)
							{
								// Line probably starts with a '0' so CFE is still running flash dump.
								std::cout << "\tLast read: DATA " << *ext << std::endl;	
							}
							
							cfe_has_quit = false;
						}
					}
					else
					{
						cfe_has_quit = false;
					}

					if(!cfe_has_quit)
					{
						if(verbose)
						{
							std::cout << "CFE is Busy." << std::endl << "CFE on my system keeps running program until it quits - ignoring ctrl-c - so this solution may vary." << std::endl;		
						}
					}
				}
			}
			else
			{
				fail = true;
			}

			// Close handle to tty.
			uart_close(uart_device);
		}
		else
		{
			fail = true;
		}
	}

	uart_free(uart_device);

	// Free all the memory used.
	free_memory();

	if(!continue_cfe && verbose)
	{
		std::cout << "Quitting like told.." << std::endl;
	}

	if(fail)
	{
		return EXIT_FAILURE;	
	}
	
	return EXIT_SUCCESS;	
}

