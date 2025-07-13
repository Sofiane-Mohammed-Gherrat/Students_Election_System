#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rep.h"
#include "fileio.h"
#include "utils.h"

void Representative_actions();
void Representative_actions(){
    printf("As a students' representative you can: \n");
    printf(" • Register/log in to the system.\n");
    printf(" • Submit/update their election manifesto.\n");
}

void rep_menu(const User *current) {
    printf("👤 Representative: %s\n", current->username);

    Representative_actions();

    while (1) {
        int opt = rep_prompt();  // 0 Logout, 1 Submit/Update manifesto

        if (opt == 0) break;

        Manifesto *mfs = NULL;
        int mfCount = load_manifestos(&mfs);

        int idx = -1;
        for (int i = 0; i < mfCount; i++) {
            if (strcmp(mfs[i].rep_username, current->username) == 0) {
                idx = i;
                break;
            }
        }

        char buffer[MANIFESTO_LEN];
        get_string("Enter your manifesto", buffer, MANIFESTO_LEN);

        if (idx >= 0) {
            strcpy(mfs[idx].manifesto, buffer);
            printf("✅ Manifesto updated.\n");
        } else {
            mfs = realloc(mfs, (mfCount + 1) * sizeof *mfs);
            strcpy(mfs[mfCount].rep_username, current->username);
            strcpy(mfs[mfCount].manifesto, buffer);
            mfCount++;
            printf("✅ Manifesto submitted.\n");
        }

        save_manifestos(mfs, mfCount);
        free(mfs);
    }
}
