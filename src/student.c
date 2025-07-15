// student.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"
#include "fileio.h"
#include "utils.h"

void students_actions() {
    printf("\nAs a student, you can:\n");
    printf(" • Register and log in to the system\n");
    printf(" • View the list of student representatives with their manifestos\n");
    printf(" • Cast one vote for a representative\n");
    printf(" • View election results (when published by admin)\n");
}

void Display_manifestos(const Manifesto *mfs, int mfCount) {
    printf("=================================================\n");
    printf("\n Candidate Manifestos:\n\n");
    for (int i = 0; i < mfCount; i++) {
        printf("• %s:\n%s\n\n", mfs[i].rep_username, mfs[i].manifesto);
    }
}
int check_already_voted(const Vote *votes, int voteCount, const char *username) {
    for (int i = 0; i < voteCount; i++) {
        if (strcmp(votes[i].student_username, username) == 0) {
            printf("\n[ERROR] You've already voted!\n");
            return 1; // Already voted
        }
    }
    printf("\n[SUCCESS] You have not voted yet.\n");
    return 0; // Not voted yet
}
int validate_candidate(const User *current, const Manifesto *mfs, int mfCount) {
    if (current->role != ROLE_STUDENT) {
        printf("[WARNING] Only students can vote!\n");
        return 0; // Not a student
    }
    if (mfCount == 0) {
        printf("[WARNING] No candidates available to vote for.\n");
        return 0; // No candidates
    }
    return 1; // Valid candidate
}

void record_new_vote(const User *current, const char *choice) {
    Vote newVote;
    strcpy(newVote.student_username, current->username);
    strcpy(newVote.rep_username, choice);

    Vote *votes = NULL;
    int voteCount = load_votes(&votes);
    votes = realloc(votes, (voteCount + 1) * sizeof *votes);
    votes[voteCount++] = newVote;
    save_votes(votes, voteCount);
    free(votes);
    //! Append vote update to results file
    append_vote_update(current);
    printf("[SUCCESS] Vote cast for %s!\n", choice);
}
/* 
void results_summary(const Manifesto *resMfs, const int *counts, int resCount) {
    printf("=================================================\n");
    display_result_status();
    printf("[RESULTS] Election Results Summary:\n");
    for (int i = 0; i < resCount; i++) {
        printf("  • %s :    %d votes\n", resMfs[i].rep_username, counts[i]);
    }
} */


void student_menu(const User *current) {
    printf("\n=======================================");
    printf("\n      [Welcome] Student: %s\n", current->username);
    printf("=======================================\n");
    printf("[SUCCESS] Student menu loaded successfully.\n");
    
    //* what you can do
    students_actions();

    while (1) {
        int opt = student_prompt();  // 0 Logout, 1 View reps, 2 Vote, 3 View results
        if (opt == 0) {
            logging_out();
            break;
        }

        Manifesto *mfs = NULL;
        int mfCount = load_manifestos(&mfs);

        if (opt == 1) {
            // Display manifestos
            Display_manifestos(mfs, mfCount);
            if (mfCount == 0) {
                printf("[WARNING] No manifestos available. Please check back later.\n");
                continue; // No manifestos, prompt again
            }
            printf("[SUCCESS] Manifestos loaded successfully.\n");
        } 
        else if (opt == 2) {
            Vote *votes = NULL;
            int voteCount = load_votes(&votes);
            
            //! Check if already voted
            if (check_already_voted(votes, voteCount, current->username)) {
                free(votes);
                goto student_continue;
            }
            free(votes);

            //! Get vote choice
            char choice[USERNAME_LEN];
            printf("=================================================\n");
            get_string("\nEnter rep username to vote for", choice, USERNAME_LEN);
            choice[strcspn(choice, "\r\n")] = '\0'; // Remove trailing newline

            //! Validate candidate
            if (!validate_candidate(current, mfs, mfCount)) {
                free(mfs);
                continue; // Invalid candidate, prompt again
            }

            //! Record vote
            record_new_vote(current, choice);
        } 
        else if (opt == 3) {
            Manifesto *resMfs = NULL;
            int *counts = NULL;
            int resCount = load_results(&resMfs, &counts);
            Vote *votes = NULL;
            int voteCount = load_votes(&votes);


            // Compute longest username width
            int maxNameLen = 0;
            for (int i = 0; i < resCount; i++) {
                int len = strlen(resMfs[i].rep_username);
                if (len > maxNameLen) maxNameLen = len;
            }

            // Display each rep and their vote count
            printf("=================================================\n\n");
            display_result_status();
            printf("\nElection Results:\n");
            for (int i = 0; i < resCount; i++) {
                int cnt = 0;
                for (int j = 0; j < voteCount; j++) {
                    if (strcmp(votes[j].rep_username, resMfs[i].rep_username) == 0) {
                        cnt++;
                    }
                }
                printf(" • %-*s : %4d votes\n",
                    maxNameLen,
                    resMfs[i].rep_username,
                    cnt);
            }

            free(resMfs);
            free(counts);

        }
        else {
            printf("[ERROR] Invalid option! Please try again.\n");
            free(mfs);
            continue; // Invalid option, prompt again
        }
        student_continue:
        free(mfs);
    }
}