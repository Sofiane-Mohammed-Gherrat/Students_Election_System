#ifndef ADMIN_H
#define ADMIN_H
#include "models.h"

void initial_admin_setup(void);
int check_default_admin_at_top(void);
int enforce_default_admin_top(void);
int verify_and_clean_admins(void);
void admin_actions(void);
void admin_menu(const User *current);
#endif
