#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "models.h"
#include "fileio.h"
#include "utils.h"
#include "admin.h"
#include "rep.h"
#include "student.h"

int authenticate(const char *username, const char *password, User *outUser) {
    User *users;
    int count = load_users(&users);
    for (int i = 0; i < count; i++) {
        if (strcmp(username, users[i].username) == 0 &&
            strcmp(password, users[i].password) == 0) {
            *outUser = users[i];
            free(users);
            return 1;
        }
    }
    free(users);
    return 0;
}

int main(void) {
    while (1) {
        int opt = main_prompt();
        if (opt == 0) break;
        //! we are getting the username and pssword for all the choices and then process the login or registration
        char uname[USERNAME_LEN], pass[PASS_LEN];
        get_string("Username", uname, USERNAME_LEN);
        get_string("Password", pass, PASS_LEN);

        User current;
        if (opt == 2 && authenticate(uname, pass, &current)) {
            if (current.role == ROLE_ADMIN) {
                admin_menu(&current);
            } else if (current.role == ROLE_REP) {
                rep_menu(&current);
            } else {
                student_menu(&current);
            }
        } 
        else if (opt == 1) {
            User *users = NULL;
            int cnt = load_users(&users);
            users = realloc(users, (cnt + 1) * sizeof *users);
            strcpy(users[cnt].username, uname);
            strcpy(users[cnt].password, pass);
            // For simplicity, all new registrations are Students:
            users[cnt].role = ROLE_STUDENT;
            save_users(users, cnt + 1);
            free(users);
            printf("Registration complete. You are now a STUDENT.\n");
        } 
        else {
            printf("❌ Invalid credentials—try again.\n");
        }
    }
    printf("🛑 Exiting, goodbye!\n");
    return 0;
}
