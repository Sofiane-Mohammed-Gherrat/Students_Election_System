#ifndef UTILS_H
#define UTILS_H

#include "models.h"
#include <stdbool.h>
#include <ctype.h>

// Prompts for menu selections with min/max return values
int main_prompt();
int admin_prompt();
int rep_prompt();
int student_prompt();
int get_int(int min, int max);
//! authentification
int authenticate(const char *username, const char *password, User *outUser);
//! Preventing Duplicate usernames 
bool username_exists(User *users, int count, const char *uname);
//! Password
bool is_strong_password(const char *p);
void password_guideline();
//! valid chars in username
bool valid_username(const char *s);
void username_guideline();
// Gets a non-empty string from user
void get_string(const char *prompt, char *buf, int maxlen);
int isValidCandidate(const char *name);

#endif // UTILS_H
