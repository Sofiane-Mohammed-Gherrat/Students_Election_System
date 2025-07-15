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

        printf("[SUCCESS] Admin account initialized.\n\n");
    }
}
void admin_actions(){
    printf("\nAs an admin you can:\n");
    printf("  • View a list of registered student representatives.\n");
    printf("  • View the total number of votes each representative has received.\n");
    printf("  • Publish and display the final election results.\n\n");
}

int Student_rep_list(void) {
    User *reps = NULL;
    int repCount = load_reps(&reps);  // load only reps

    if (repCount == 0) {
        printf("\n[WARNING] No registered student representatives found.\n");
        return 0;
    }
    else {
        printf("\n[SUCCESS] Representatives loaded successfully.\n");
        printf("=================================================\n\n");
        printf("Registered Student Representatives:\n");
        for (int i = 0; i < repCount; i++) {
            printf("  • %s\n", reps[i].username);
        }
    }

    free(reps);
    return repCount;
}

//! Show current vote counts for each representative
void Display_votes(void) {
    User *reps = NULL;
    int repCount = load_reps(&reps);

    Vote *votes = NULL;
    int voteCount = load_votes(&votes);

    printf("\n\nCurrent vote counts:\n");
    if (repCount == 0) {
        printf("[WARNING] No representatives found.\n");
        free(reps); free(votes);
        return;
    }

    int maxNameLen = 0;
    for (int i = 0; i < repCount; i++)
        if ((int)strlen(reps[i].username) > maxNameLen)
            maxNameLen = strlen(reps[i].username);

    for (int i = 0; i < repCount; i++) {
        int cnt = 0;
        for (int j = 0; j < voteCount; j++)
            if (strcmp(votes[j].rep_username, reps[i].username) == 0)
                cnt++;

        printf(" • %-*s : %*d votes\n",
                maxNameLen,
                reps[i].username,
               4,  // lets assume vote counts won't exceed 9999
                cnt);
    }

    free(reps); free(votes);
}


void publish_results(Manifesto *mfs, int mfCount, Vote *votes, int voteCount) {
    int *counts = calloc(mfCount, sizeof *counts);
    if (!counts) {
        fprintf(stderr, "[ERROR] Out of memory during tally.\n");
        return;
    }

    for (int i = 0; i < voteCount; i++) {
        for (int j = 0; j < mfCount; j++) {
            if (strcmp(votes[i].rep_username, mfs[j].rep_username) == 0) {
                counts[j]++;
                break;  // Found the candidate; no need to check further
            }
        }
    }

    save_results(mfs, counts, mfCount);
    printf("\n[SUCCESS] Results published.\n");
    free(counts);
}

//! Admin menu
// 0 Logout, 1) rep list, 2) view votes, 3) publish results
void admin_menu(const User *current) {
    printf("\n=================================");
    printf("\n      [Welcome] Admin: %s\n", current->username);
    printf("=================================\n");
    printf("[ADMIN] Admin menu loaded successfully.\n");

    //* what you can do
    admin_actions();

    while (1) {
        int opt = admin_prompt(); //* e.g. 0) Logout, 1) rep list, 2) view votes, 3) publish results
        if (opt == 0) {
            logging_out();
            break;
        }
        Manifesto *mfs; int mfCount = load_manifestos(&mfs);
        Vote *votes; int voteCount = load_votes(&votes);

        //! Rep list
        if (opt == 1) {
            Student_rep_list();
        } 
        //! vote count
        else if (opt == 2) {
            Display_votes();
        }
        //! publish results 
        else if (opt == 3) {
            publish_results(mfs, mfCount, votes, voteCount);
            // Mark results as published
            mark_results_published();
            // Display the status of results
            display_result_status();
    } else {
        printf("[Error] Invalid option. Please try again.\n");
        continue;
    }
    // Cleanup
        free(mfs);
        free(votes);
    }
}
