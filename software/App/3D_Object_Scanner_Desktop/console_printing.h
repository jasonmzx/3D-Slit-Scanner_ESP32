#ifndef CONSOLE_PRINTING_H
#define CONSOLE_PRINTING_H

#include<iostream>

//Defined Colors

const std::string black("\033[0;30m");
const std::string red("\033[0;31m");
const std::string green("\033[0;32m");
const std::string yellow("\033[0;33m");
const std::string blue("\033[0;34m");
const std::string magenta("\033[0;35m");
const std::string cyan("\033[0;36m");
const std::string white("\033[0;37m");

const std::string bright_black("\033[1;30m");
const std::string bright_red("\033[1;31m");
const std::string bright_green("\033[1;32m");
const std::string bright_yellow("\033[1;33m");
const std::string bright_blue("\033[1;34m");
const std::string bright_magenta("\033[1;35m");
const std::string bright_cyan("\033[1;36m");
const std::string bright_white("\033[1;37m");

const std::string reset("\033[0m");

//Defined Print Methods

void print_error(std::string& error);
void programCredit();
void cinPromptMessage(std::string prompt, bool isMandatory);
void printBanner(size_t length);

#endif