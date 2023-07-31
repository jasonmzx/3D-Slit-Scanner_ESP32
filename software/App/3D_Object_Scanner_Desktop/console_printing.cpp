
#include <iostream>
#include "console_printing.h"

void print_error(std::string& error) {
	std::cerr << red <<"[ ERROR ]: " << error << reset << "\n";
}

void print_banner(std::string banner_txt) {
	//TODO: This 
}

void programCredit() {
	std::string text_decor = " #################### ";
	std::string r_p = " -> ";

	std::cout << yellow << text_decor << "Lazer Scanner / Profiler into 3D Project - DESKTOP APP - " << text_decor << reset << std::endl;
	std::cout << r_p << "Run TCP Server | rtcp [INT port]" << std::endl;
	std::cout << r_p << "Render Dataset | render <STR path/to/dataset> [INT midpoint] [INT cutoff] [STR pipeline]\n" << std::endl;

	std::cout << "Project by: jasonmzx , hope you enjoyed :-)\n\n\n" << std::endl;
}

void cinPromptMessage(std::string prompt, bool isMandatory) {
	if (isMandatory) {
		std::cout << red << "!! MANDATORY PARAMETER !!" << std::endl;
	}
	else {
		std::cout << cyan << "This Parameter is Optional, Just leave it Blank & Enter to Skip." << std::endl;
	}
	
	std::cout << bright_yellow << ">|>|>|> " << prompt << ":" << white << std::endl;
}