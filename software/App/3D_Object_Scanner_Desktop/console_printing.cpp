
#include <iostream>
#include "console_printing.h"

void print_error(std::string& error) {
	std::cerr << red <<"[ ERROR ]: " << error << reset << "\n";
}