#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"
#include "fileio.h"
#include "utils.h"

void students_actions(); 
void students_actions(){
    printf("As a student, you could: \n");
    printf(" • Register and log in to the system.\n");
    printf(" • Vew the list of student representatives along with their manifestos.\n");
    printf(" • Cast a vote for 1 representative (1 vote per student).\n");
    printf(" • View the final election results once they are published by the School Admin.\n");
}
void student_menu(const User *current) {
    printf("👥 Student: %s\n", current->username);

    students_actions();

    while (1) {
        int opt = student_prompt();  // 0 Logout, 1 View reps, 2 Vote, 3 View results
        if (opt == 0) break;

        Manifesto *mfs = NULL;
        int mfCount = load_manifestos(&mfs);

        if (opt == 1) {
            printf("📄 Candidate Manifestos:\n");
            for (int i = 0; i < mfCount; i++) {
                printf(" - %s: %s\n", mfs[i].rep_username, mfs[i].manifesto);
            }
        } else if (opt == 2) {
            Vote *votes = NULL;
            int voteCount = load_votes(&votes);
            for (int i = 0; i < voteCount; i++) {
                if (strcmp(votes[i].student_username, current->username) == 0) {
                    printf("❌ You've already voted!\n");
                    free(votes);
                    goto student_continue;
                }
            }
            free(votes);

            char choice[USERNAME_LEN];
            get_string("Enter rep username to vote for", choice, USERNAME_LEN);

            Vote newVote;
            strcpy(newVote.student_username, current->username);
            strcpy(newVote.rep_username, choice);

            votes = NULL;
            voteCount = load_votes(&votes);
            votes = realloc(votes, (voteCount + 1) * sizeof *votes);
            votes[voteCount++] = newVote;
            save_votes(votes, voteCount);
            free(votes);
            printf("✅ Vote cast for %s!\n", choice);

        } else if (opt == 3) {
            Manifesto *resMfs = NULL;
            int *counts = NULL;
            int resCount = load_results(&resMfs, &counts);
            if (resCount == 0) {
                printf("⏳ Results not published yet.\n");
            } else {
                printf("📊 Final Election Results:\n");
                for (int i = 0; i < resCount; i++) {
                    printf(" - %s : %d votes\n", resMfs[i].rep_username, counts[i]);
                }
            }
            free(resMfs);
            free(counts);
        }

        student_continue:
        free(mfs);
    }
}
