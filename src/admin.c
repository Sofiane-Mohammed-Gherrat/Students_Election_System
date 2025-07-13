#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "admin.h"
#include "fileio.h"
#include "utils.h"

// For first-run: create initial admin
void initial_admin_setup() {
    User *users = NULL;
    int count = load_users(&users);
    if (count == 0) {
        printf("\n=== Initial Admin Setup ===\n");
        printf("Create Admin Account\n");
        char uname[USERNAME_LEN], pass[PASS_LEN];

        strncpy(uname, INIT_ADMIN_USERNAME, USERNAME_LEN);
        strncpy(pass, INIT_ADMIN_PASSWORD, PASS_LEN);

        users = realloc(users, sizeof(User));
        strcpy(users[0].username, uname);
        strcpy(users[0].password, pass);
        users[0].role = ROLE_ADMIN;

        save_users(users, 1);
        free(users);

        printf("✅ Admin account initialized.\n\n");
    }
}

void admin_menu(const User *current) {
    printf("Welcome Admin %s\n", current->username);
    while (1) {
        int opt = admin_prompt(); // e.g. 0) Logout, 1) View, 2) Publish
        if (opt == 0) break;

        Manifesto *mfs; int mfCount = load_manifestos(&mfs);
        Vote *votes; int voteCount = load_votes(&votes);

        if (opt == 1) {
            printf("Current vote counts:\n");
            for (int i=0; i<mfCount; i++) {
                int cnt = 0;
                for (int j=0; j<voteCount; j++)
                    if (strcmp(votes[j].rep_username, mfs[i].rep_username)==0)
                        cnt++;
                printf(" - %s : %d votes\n", mfs[i].rep_username, cnt);
            }
        } else if (opt == 2) {
            int *counts = calloc(mfCount, sizeof *counts);
            for (int i=0; i<voteCount; i++)
                for (int j=0; j<mfCount; j++)
                    if (strcmp(votes[i].rep_username, mfs[j].rep_username)==0)
                        counts[j]++;
            save_results(mfs, counts, mfCount);
            printf("✅ Results published.\n");
            free(counts);
        }

        free(mfs);
        free(votes);
    }
}
