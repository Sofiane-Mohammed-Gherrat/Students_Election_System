#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rep.h"
#include "fileio.h"
#include "utils.h"

void Representative_actions(){
    printf("\nAs a students' representative you can: \n");
    printf(" • Register/log in to the system.\n");
    printf(" • Submit/update their election manifesto.\n");
}

void rep_menu(const User *current) {
    printf("\n=================================================");
    printf("\n      [Welcome] Representative: %s\n", current->username);
    printf("=================================================\n");
    printf("[SUCCESS] Representative menu loaded successfully.\n");
    
    //* what you can do
    Representative_actions();

    while (1) {
        int opt = rep_prompt();  // 0 Logout, 1 Submit/Update manifesto

        if (opt == 0) {
            logging_out();
            break;
        }

        Manifesto *mfs = NULL;
        int mfCount = load_manifestos(&mfs);

        int idx = -1;
        for (int i = 0; i < mfCount; i++) {
            if (strcmp(mfs[i].rep_username, current->username) == 0) {
                idx = i;
                break;
            }
        }
        //* Get the manifesto content from the user
        if (idx >= 0) {
            if (strcmp(mfs[idx].manifesto, "Not yet submitted") == 0) {
                printf("\n\n[WARNING] You have not submitted a manifesto yet.\n\n");
            } else {
                printf("\n\nCurrent manifesto:\n%s\n", mfs[idx].manifesto);
            }
        }
        
        char buffer[MANIFESTO_LEN];
        get_string("Enter your manifesto (or press enter to keep current):\n> ", buffer, MANIFESTO_LEN);

        if (buffer[0] == '\0' && idx >= 0) {
                printf("\n[WARNING] No changes made to the manifesto.\n");
                continue; // No changes, prompt again
        }
        else if (idx >= 0 && buffer[0] != '\0') {
            strcpy(mfs[idx].manifesto, buffer);
            printf("\n[SUCCESS] Manifesto updated.\n");
        } else {
            mfs = realloc(mfs, (mfCount + 1) * sizeof *mfs);
            strcpy(mfs[mfCount].rep_username, current->username);
            strcpy(mfs[mfCount].manifesto, buffer);
            mfCount++;
            printf("\n[SUCCESS] Manifesto submitted.\n");
        }

        save_manifestos(mfs, mfCount);
        free(mfs);
    }
}
