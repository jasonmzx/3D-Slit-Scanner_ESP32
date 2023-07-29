#ifndef CONSOLE_PRINTING_H
#define CONSOLE_PRINTING_H

#include<iostream>

//Defined Colors

const std::string red("\033[0;31m");
const std::string green("\033[1;32m");
const std::string yellow("\033[1;33m");
const std::string cyan("\033[0;36m");
const std::string reset("\033[0m");

//Defined Print Methods

void print_error(std::string& error);
void programCredit();
#endif