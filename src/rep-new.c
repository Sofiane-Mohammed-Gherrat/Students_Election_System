// rep.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rep.h"
#include "fileio.h"
#include "utils.h"

void Representative_actions() {
    printf("\nHello representative!!!\nYou can now:\n");
    printf(" • Register/log in to the system\n");
    printf(" • Submit/update your election manifesto\n");
}

void rep_menu(const User *current) {
    printf("👤 Representative: %s\n", current->username);
    Representative_actions();

    while (1) {
        int opt = rep_prompt();  // 0 Logout, 1 Submit/Update manifesto
        if (opt == 0) break;

        Manifesto *mfs = NULL;
        int mfCount = load_manifestos(&mfs);

        // Check if already has a manifesto
        int idx = -1;
        for (int i = 0; i < mfCount; i++) {
            if (strcmp(mfs[i].rep_username, current->username) == 0) {
                idx = i;
                break;
            }
        }

        // Show current manifesto if exists
        if (idx >= 0) {
            printf("\nYour current manifesto:\n%s\n", mfs[idx].manifesto);
        } else {
            printf("\nYou haven't submitted a manifesto yet.\n");
        }

        // Ask to update/submit
        char choice[10];
        get_string("Update/submit manifesto? (y/n)", choice, sizeof(choice));
        if (choice[0] == 'y' || choice[0] == 'Y') {
            char buffer[MANIFESTO_LEN];
            get_string("Enter your manifesto", buffer, MANIFESTO_LEN);

            if (idx >= 0) {
                // Update existing
                strcpy(mfs[idx].manifesto, buffer);
                printf("✅ Manifesto updated.\n");
            } else {
                // Add new
                mfs = realloc(mfs, (mfCount + 1) * sizeof *mfs);
                strcpy(mfs[mfCount].rep_username, current->username);
                strcpy(mfs[mfCount].manifesto, buffer);
                mfCount++;
                printf("✅ Manifesto submitted.\n");
            }

            save_manifestos(mfs, mfCount);
        } else {
            printf("Manifesto not updated.\n");
        }

        free(mfs);
    }
}