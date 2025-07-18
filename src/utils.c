#include <stdio.h>
#include <string.h>
#include "utils.h"
#include <stdbool.h>
#include <ctype.h>
#include "fileio.h"
#include <stdlib.h>
#include <locale.h>

int get_int(int min, int max);  // Private helper

/**
 * Authenticate user by username and password.
 *
 * @param username   The username to look up.
 * @param password   The password to verify.
 * @param outUser    Pointer to a User structure where matching user data will be stored.
 * @return           1 on successful authentication (outUser populated), 0 otherwise.
 *
 * How it works:
 *   1. Calls `load_users(&users)` to load all users into a dynamically allocated array `users`,
 *      returning the count of users.
 *   2. Iterates over the `users` array, comparing both username and password.
 *   3. If a match is found:
 *        - Copies the matched user into `*outUser`.
 *        - Frees the `users` array.
 *        - Returns 1.
 *   4. If no match is found after checking all users:
 *        - Frees the `users` array.
 *        - Returns 0.
 *
 * Usage context:
 *   - Employed in both login (authentication) and registration (to check for existing credentials).
 *   - Called whenever a user attempts to log in or register.
 *   - Helps prevent unauthorized access by verifying provided credentials.
 *   - Ensures memory management by freeing the loaded user list after use.
 */

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

/**
 * Prompt the user and safely read a line of text from stdin.
 *
 * @param prompt  The message to display before input (e.g., "Username").
 * @param buf     The buffer to receive the input.
 * @param maxlen  Size of buf, including space for the terminating null byte.
 *
 * Reads up to maxlen–1 characters using fgets. If the input exactly fills the buffer
 * without a newline, the rest of the line is flushed from stdin to prevent overflow.
 * Notifies the user if their input was too long and prompts again.
 * Strips any trailing newline before returning.
 *
 * Returns into buf:
 *  - A null-terminated string (possibly empty) on valid input.
 *  - An empty string if EOF or read error occurs (`fgets` returns NULL).
 *
 * This function loops until valid input that fits the buffer is entered.
 * It’s used to read usernames, passwords, and other short fields in login/registration.
 */
void get_string(const char *prompt, char *buf, int maxlen) {
    while (1) {
        printf("%s: ", prompt);
        if (!fgets(buf, maxlen, stdin)) {
            buf[0] = '\0';
            return;
        }
        size_t len = strlen(buf);

        // If input filled buffer and last char is not newline, flush rest of line
        if (len == maxlen - 1 && buf[len - 1] != '\n') {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            printf("Input too long! Limit to %d characters.\n", maxlen - 1);
            continue;
        }

        // Remove newline if present
        buf[strcspn(buf, "\n")] = '\0';
        return;
    }
}

/**
 * Display the main menu and prompt the user to select an option.
 *
 * Presents the user with choices for:
 *   1 – Register
 *   2 – Login
 *   0 – Exit
 *
 * @return An integer representing the user's selection: 0, 1, or 2.
 *
 * Behavior:
 *   - Prints the menu options.
 *   - Calls `get_int(0, 2)` to read and validate user input within the allowed range.
 *
 * Usage:
 *   - Invoked from the main loop.
 *   - Determines the next action: take user to registration, login, or exit.
 */
int main_prompt() {
    printf("Choose below:");
    printf("\n 1. Register\n 2. Login\n 0. Exit\nSelect: ");
    return get_int(0, 2);
}

/**
 * Display the admin menu and prompt for selection.
 *
 * Presents a menu of administrative actions to the user:
 *   1 – List student representatives
 *   2 – View votes
 *   3 – Publish results
 *   0 – Logout
 *
 * @return An integer corresponding to the chosen action (0–3).
 *
 * Behavior:
 *   - Outputs the admin menu options to stdout.
 *   - Uses `get_int(0, 3)` to validate and read the user's choice.
 *
 * Usage context:
 *   - Called from the main admin loop.
 *   - Directs application flow based on administrative selection.
 *   - Ensures logically restricted and safe input in managing election operations.
 */
int admin_prompt() {
    printf("\nAdmin Menu:\n1. Student Representatives list\n2. View Votes\n3. Publish Results\n0. Logout\nSelect: ");
    return get_int(0, 3);
}

/**
 * Display the representative menu and return a valid selection.
 *
 * Presents the representative with options to:
 *   1 – Submit or update manifesto
 *   0 – Logout
 *
 * @return An integer representing the user's selection (0 or 1).
 *
 * Behavior:
 *   - Prints the menu to stdout.
 *   - Uses `get_int(0, 1)` to enforce valid input within the given range.
 *
 * Usage context:
 *   - Called within the representative‑specific loop.
 *   - Enables representative users to manage their manifesto or logout.
 *   - Prevents invalid choices by continuously prompting until input is valid.
 */
int rep_prompt() {
    printf("\nRep Menu:\n  1. Submit/Update Manifesto\n  0. Logout\nSelect: ");
    return get_int(0, 1);
}

int student_prompt() {
    printf("\nStudent Menu:\n  1. View Manifestos\n  2. Cast Vote\n  3. View Results\n  0. Logout\nSelect: ");
    return get_int(0, 3);
}

/**
 * Prompt the user for an integer input within a specified range.
 *
 * Continuously prompts the user until a valid integer is entered
 * that lies between `min` and `max` (inclusive). Invalid inputs,
 * such as non-numeric characters or out-of-range values, are handled gracefully.
 *
 * @param min  The minimum acceptable value (inclusive).
 * @param max  The maximum acceptable value (inclusive).
 * @return     A validated integer input from the user within the specified range.
 *
 * Behavior:
 *   - Uses `scanf` to parse integer input.
 *   - Flushes invalid or excess characters from stdin to prevent infinite loops.
 *
 * Usage context:
 *   - Core utility in user input flow.
 *   - Called by main menu, admin menu, representative menu, and student menu to ensure
 *     user selections are safe and valid.
 */
int get_int(int min, int max) {
    int choice;
    while (scanf("%d", &choice) != 1 || choice < min || choice > max) {
        printf("Please enter a valid number (%d–%d): ", min, max);
        while (getchar() != '\n');
    }
    while (getchar() != '\n');
    return choice;
}

/**
 * Check whether a username already exists in the system.
 *
 * Iterates through a list of User records to determine if any user’s
 * `username` matches the given `uname`.
 *
 * @param users  Pointer to the first element in a User array.
 * @param count  Number of entries in the array.
 * @param uname  Null-terminated string to check for existence.
 * @return       `true` if a matching username is found, `false` otherwise.
 *
 * Usage:
 *   - Used during registration to ensure username uniqueness.
 *   - Prevents duplicate usernames, enforcing a unique identifier per user.
 */
bool username_exists(User *users, int count, const char *uname) {
    for (int i = 0; i < count; i++) {
        if (strcmp(users[i].username, uname) == 0) return true;
    }
    return false;
}

/**
 * Check whether a password meets minimum strength requirements.
 *
 * Validates a password against the following criteria:
 *   - At least 6 characters long
 *   - Contains at least one lowercase letter
 *   - Contains at least one uppercase letter
 *   - Contains at least one digit
 *
 * @param p  Null-terminated string representing the password to validate.
 * @return   `true` if the password satisfies all criteria; `false` otherwise.
 *
 * How it works:
 *   - Checks the length of `p` to ensure it is >= 6.
 *   - Iterates through each character to verify the presence of:
 *       * A lowercase letter
 *       * An uppercase letter
 *       * A digit
 *   - Returns `true` if all checks pass; otherwise, returns `false`.
 *
 * Usage context:
 *   - Used during user registration to enforce strong password policies.
 *   - Helps ensure account security by requiring diverse character types.
 */
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

/**
 * Display password creation guidelines to the user.
 *
 * Prints recommended criteria for strong password creation during registration:
 *  - Minimum of 8 characters (12+ is even more secure)
 *  - At least 1 lowercase letter, 1 uppercase letter, and 1 digit
 *  - Optional: use symbols for extra security
 *
 * @note According to NIST guidelines, longer passphrases (12–16+ chars) provide stronger security
 * than enforcing complexity rules alone :contentReference[oaicite:1]{index=1}.
 * This prompt educates users on both length and character diversity.
 *
 * Usage context:
 *   - Called from the registration flow before or after prompting for a password
 *   - Helps users understand why password strength matters and how to meet it
 */
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

/**
 * Validate a username against formatting rules.
 *
 * Checks if a username satisfies the following criteria:
 *   - Length is between 3 and USERNAME_LEN–1 characters.
 *   - First character is an alphabetic letter (A–Z, a–z).
 *   - Remaining characters are alphanumeric (A–Z, a–z, 0–9) or underscores ('_').
 *
 * @param s  Null-terminated string representing the username to validate.
 * @return   `true` if the username meets all criteria; `false` otherwise.
 *
 * How it works:
 *   - Verifies that the username length is within bounds.
 *   - Ensures the first character is alphabetic using `isalpha`.
 *   - Iterates through subsequent characters, rejecting any that are not alphanumeric or '_'.
 *   - Prints debug messages for invalid conditions to aid troubleshooting.
 *
 * Usage context:
 *   - Used during registration to enforce username format.
 *   - Helps maintain a consistent and secure user namespace.
 */
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

void logging_out(){
    printf("\n[Waiting] Logging out...\n====================\n\n");
}
