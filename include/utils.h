#ifndef UTILS_H
#define UTILS_H

#include "models.h"

// Prompts for menu selections with min/max return values
int main_prompt();
int admin_prompt();
int rep_prompt();
int student_prompt();
int get_int(int min, int max);

// Gets a non-empty string from user
void get_string(const char *prompt, char *buf, int maxlen);

#endif // UTILS_H
