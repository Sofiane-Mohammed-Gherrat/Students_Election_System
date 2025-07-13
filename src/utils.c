#include <stdio.h>
#include <string.h>
#include "utils.h"
#include <stdbool.h>
#include <ctype.h>
#include "fileio.h"
#include <stdlib.h>
#include <locale.h>

//setlocale(LC_CTYPE, "");

int get_int(int min, int max);  // Private helper

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

void get_string(const char *prompt, char *buf, int maxlen) {
    printf("%s: ", prompt);
    fgets(buf, maxlen, stdin);
    buf[strcspn(buf, "\n")] = '\0';
}

int main_prompt() {
    printf("Choose below:");
    printf("\n 1. Register\n 2. Login\n 0. Exit\nSelect: ");
    return get_int(0, 2);
}

int admin_prompt() {
    printf("\nAdmin Menu:\n  1. View Votes\n  2. Publish Results\n  0. Logout\nSelect: ");
    return get_int(0, 2);
}

int rep_prompt() {
    printf("\nRep Menu:\n  1. Submit/Update Manifesto\n  0. Logout\nSelect: ");
    return get_int(0, 1);
}

int student_prompt() {
    printf("\nStudent Menu:\n  1. View Manifestos\n  2. Cast Vote\n  3. View Results\n  0. Logout\nSelect: ");
    return get_int(0, 3);
}

int get_int(int min, int max) {
    int choice;
    while (scanf("%d", &choice) != 1 || choice < min || choice > max) {
        printf("Please enter a valid number (%d–%d): ", min, max);
        while (getchar() != '\n');
    }
    while (getchar() != '\n');
    return choice;
}

bool username_exists(User *users, int count, const char *uname) {
    for (int i = 0; i < count; i++) {
        if (strcmp(users[i].username, uname) == 0) return true;
    }
    return false;
}
//! password 
bool is_strong_password(const char *p) {
    if (strlen(p) < 6) return false;
    bool lower=false, upper=false, digit=false;
    for (; *p; p++) {
        if (islower((unsigned char)*p)) lower = true;
        if (isupper((unsigned char)*p)) upper = true;
        if (isdigit((unsigned char)*p)) digit = true;
    }
    return lower && upper && digit;
}
void password_guideline(){
    printf("Your password should be:\n");
    printf("• At least 8 characters long (12+ is even more secure).\n");
    printf("• Contain at least:\n");
    printf("  – 1 lowercase letter (a-z)\n");
    printf("  – 1 uppercase letter (A-Z)\n");
    printf("  – 1 digit (0-9)\n");
    printf("OPTIONAL:\n");
    printf("• Use symbols for extra security.\n\n");
}
//! Username
bool valid_username(const char *s) {
    int len = strlen(s);
    if (len < 3 || len >= USERNAME_LEN) {
        printf("Debug: Invalid length %d\n", len);
        return false;
    }

    unsigned char first_char = (unsigned char)s[0];
    if (!isalpha(first_char)) {
        printf("Debug: First character '%c' is not alphabetic\n", s[0]);
        return false;
    }

    for (int i = 1; s[i]; i++) {
        unsigned char c = (unsigned char)s[i];
        if (!isalnum(c) && c != '_') {
            printf("Debug: Invalid character '%c' at position %d\n", s[i], i);
            return false;
        }
    }

    return true;
}

void username_guideline(){
    printf("The user name should:\n");
    printf("• Be 3–20 characters long.\n");
    printf("• Begins with a letter (A–Z or a–z).\n");
    printf("• Contains only letters, digits, underscores (_), or hyphens (-).\n\n");
}