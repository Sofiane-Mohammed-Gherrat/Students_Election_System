#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileio.h"
#include "models.h"


//! Creates files if they don't exist
void ensure_file_exists(const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) {
        // File doesn't exist — create and close
        f = fopen(fname, "w");
        if (!f) {
            fprintf(stderr, "Error: Unable to create %s\n", fname);
            exit(EXIT_FAILURE);
        }
    }
    fclose(f);
}

// Load users
int load_users(User **out) {
    FILE *f = fopen(Users_Path, "r");
    if (!f) { *out = NULL; return 0; }
    User *arr = NULL; int cap = 0, cnt = 0;
    while (!feof(f)) {
        User u;
        if (fscanf(f, "%31s %31s %d\n", u.username, u.password, (int*)&u.role) != 3)
            break;
        if (cnt == cap) arr = realloc(arr, (cap = cap ? cap*2 : 4) * sizeof *arr);
        arr[cnt++] = u;
    }
    fclose(f); *out = arr; return cnt;
}

// Load representatives
int load_reps(User **outReps) {
    User *all = NULL;
    int n = load_users(&all);
    User *reps = malloc(n * sizeof(User));
    int cnt = 0;
    for (int i = 0; i < n; i++) {
        if (all[i].role == ROLE_REP) {
            reps[cnt++] = all[i];
        }
    }
    free(all);
    *outReps = reps;
    return cnt;
}

// Save users
int save_users(const User *arr, int count) {
    FILE *f = fopen(Users_Path, "w");
    if (!f) return -1;
    for (int i = 0; i < count; i++)
        fprintf(f, "%s %s %d\n", arr[i].username, arr[i].password, arr[i].role);
    fclose(f); return 0;
}

// Load manifestos
int load_manifestos(Manifesto **out) {
    FILE *f = fopen(Manifesto_Path, "r");
    if (!f) { *out = NULL; return 0; }
    Manifesto *arr = NULL; int cap = 0, cnt = 0;
    char buf[600];
    while (fgets(buf, sizeof buf, f)) {
        char *p = strchr(buf, '|');
        if (!p) continue;
        *p = 0;
        if (cnt == cap) arr = realloc(arr, (cap = cap ? cap*2 : 4) * sizeof *arr);
        strcpy(arr[cnt].rep_username, buf);
        strcpy(arr[cnt].manifesto, p+1);
        arr[cnt].manifesto[strcspn(arr[cnt].manifesto, "\n")] = 0;
        cnt++;
    }
    fclose(f); *out = arr; return cnt;
}

// Save manifestos
int save_manifestos(const Manifesto *arr, int count) {
    FILE *f = fopen(Manifesto_Path, "w");
    if (!f) return -1;
    for (int i = 0; i < count; i++)
        fprintf(f, "%s|%s\n", arr[i].rep_username, arr[i].manifesto);
    fclose(f); return 0;
}

// Load votes
int load_votes(Vote **out) {
    FILE *f = fopen(Votes_Path, "r");
    if (!f) { *out = NULL; return 0; }
    Vote *arr = NULL; int cap = 0, cnt = 0;
    while (!feof(f)) {
        Vote v;
        if (fscanf(f, "%31s %31s\n", v.student_username, v.rep_username) != 2)
            break;
        if (cnt == cap) arr = realloc(arr, (cap = cap ? cap*2 : 4) * sizeof *arr);
        arr[cnt++] = v;
    }
    fclose(f); *out = arr; return cnt;
}

// Save votes
int save_votes(const Vote *arr, int count) {
    FILE *f = fopen(Votes_Path, "w");
    if (!f) return -1;
    for (int i = 0; i < count; i++)
        fprintf(f, "%s %s\n", arr[i].student_username, arr[i].rep_username);
    fclose(f); return 0;
}

// Save results
int save_results(const Manifesto *mfs, const int *counts, int mfCount) {
    FILE *f = fopen(Results_Path, "w");
    if (!f) return -1;
    for (int i = 0; i < mfCount; i++)
        fprintf(f, "%s %d\n", mfs[i].rep_username, counts[i]);
    fclose(f); return 0;
}

// Load results
int load_results(Manifesto **outMfs, int **outCounts) {
    FILE *f = fopen(Results_Path, "r");
    //! correction
    if (!f) {
    *outMfs = NULL;
    *outCounts = NULL;
    return 0;
    }
    Manifesto *mfs = NULL; int *cnts = NULL;
    int cap = 0, n = 0;
    while (!feof(f)) {
        char uname[32]; int c;
        if (fscanf(f, "%31s %d\n", uname, &c) != 2) break;
        if (n == cap) {
            cap = cap ? cap*2 : 4;
            mfs = realloc(mfs, cap * sizeof *mfs);
            cnts = realloc(cnts, cap * sizeof *cnts);
        }
        strcpy(mfs[n].rep_username, uname);
        cnts[n] = c;
        n++;
    }
    fclose(f); *outMfs = mfs; *outCounts = cnts; return n;
}

// Ensure every rep has an manifesto entry
void sync_manifestos_with_reps(void) {
    User *reps = NULL;
    int repCount = load_reps(&reps);

    Manifesto *mfs = NULL;
    int mfCount = load_manifestos(&mfs);

    Manifesto *updated = malloc((repCount) * sizeof(Manifesto));
    int updCount = 0;

    for (int i = 0; i < repCount; i++) {
        const char *repName = reps[i].username;
        int found = 0;
        for (int j = 0; j < mfCount; j++) {
            if (strcmp(mfs[j].rep_username, repName) == 0) {
                updated[updCount++] = mfs[j];
                found = 1;
                break;
            }
        }
        if (!found) {
            strcpy(updated[updCount].rep_username, repName);
            strcpy(updated[updCount].manifesto, "Not yet submitted");
            updCount++;
        }
    }

    save_manifestos(updated, updCount);

    free(reps);
    free(mfs);
    free(updated);
}

//! Display the status of results
// This function checks if the results file has been updated since the last vote
// and displays the status accordingly.
// It reads the first line of the results file and checks if it starts with "updated"
// to determine if the results have been published or if new votes have been recorded.
void display_result_status(void) {
    FILE *f = fopen(Vote_Updates_Path, "r");
    if (!f) {
        printf("[STATUS] No results file found.\n");
        return;
    }
    char line[256];
    if (fgets(line, sizeof line, f)) {
        if (strncmp(line, "updated", 7) == 0)
            printf("[STATUS] Results have been published (updated).\n");
        else
            printf("[STATUS]  New votes recorded since last update.\n[WARNING] Wait for admin to publish results.\n");
    } else {
        printf("[STATUS] File is empty or corrupted.\n");
    }
    fclose(f);
}

//! Mark results as published
// This function updates the results file to indicate that the results have been published.
// It writes "updated" to the file, which can be checked later to determine if the results have been published.
// This is useful for the admin to mark the results after they have been finalized
void mark_results_published(void) {
    FILE *f = fopen(Vote_Updates_Path, "w");
    if (!f) {
        perror("Error updating file");
        return;
    }
    fprintf(f, "updated\n"); // only line now in file
    fclose(f);
    printf("[ADMIN] Results marked as published. New votes will be cleared.\n");
}

//! Append vote update
// This function appends a vote update to the "votes_updates.txt" file.
// It writes the username of the user who cast the vote, allowing tracking of who voted.    
void append_vote_update(const User *current) {
    FILE *f = fopen(Vote_Updates_Path, "a+");
    if (!f) {
        perror("Error opening votes_updates.txt");
        return;
    }

    char first_line[256];
    rewind(f);
    if (fgets(first_line, sizeof first_line, f) &&
        strncmp(first_line, "updated", 7) == 0) {
        
        // File starts with "updated" → clear it
        freopen(Vote_Updates_Path, "w", f);
    } else {
        // Ensure writes append
        fseek(f, 0, SEEK_END);
    }

    fprintf(f, "vote_cast_by:%s\n", current->username);
    fclose(f);
}