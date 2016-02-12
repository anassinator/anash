#ifndef __OUTPUT_H
#define __OUTPUT_H

#include <stdio.h>
#include <stdarg.h>

#define SHELL "anash"

// Define prompt style and colors.
#define PS1 ">> "
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

// Prototypes.
void prompt();
void bad_prompt();
void print_error(const char *format, ...);

#endif
