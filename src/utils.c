#include <stdio.h>
#include <string.h>
#include "utils.h"

int get_int(int min, int max);  // Private helper

void get_string(const char *prompt, char *buf, int maxlen) {
    printf("%s: ", prompt);
    fgets(buf, maxlen, stdin);
    buf[strcspn(buf, "\n")] = '\0';
}

int main_prompt() {
    printf("\n1) Register\n2) Login\n0) Exit\nSelect: ");
    return get_int(0, 2);
}

int admin_prompt() {
    printf("\nAdmin Menu:\n1) View Votes\n2) Publish Results\n0) Logout\nSelect: ");
    return get_int(0, 2);
}

int rep_prompt() {
    printf("\nRep Menu:\n1) Submit/Update Manifesto\n0) Logout\nSelect: ");
    return get_int(0, 1);
}

int student_prompt() {
    printf("\nStudent Menu:\n1) View Manifestos\n2) Cast Vote\n3) View Results\n0) Logout\nSelect: ");
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
