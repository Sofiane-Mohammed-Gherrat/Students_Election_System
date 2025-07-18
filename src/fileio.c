#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileio.h"
#include "models.h"


/**
 * ? Ensure that a file exists by creating it if needed.
 *
 * Opens `fname` for reading; if it does not exist, creates an empty file.
 * On failure to create, prints to stderr and exits with failure status.
 *
 * @param fname  Path to the file to check or create.
 *
 * @note Used to initialize storage before reading or writing user, vote,
 * manifesto, or results data.
 */
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

/**
 * ? Load all users from disk into memory.
 *
 * Reads lines from `Users_Path`, parsing username, password, and role.
 * Dynamically reallocates as needed.
 *
 * @param[out] out  Destination pointer for the allocated User array.
 * @return           Number of users loaded (may be zero).
 *
 * @note Caller should free `*out` when done.
 */
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

/**
 * ? Load only users with ROLE_REP (student representatives).
 *
 * Filters the full user list and returns a compact array containing
 * only representatives.
 *
 * @param[out] outReps  Destination pointer for the allocated array.
 * @return               Number of representatives loaded.
 *
 * @note `*outReps` must be freed by the caller.
 */
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

/**
 * ? Save an array of users to disk.
 *
 * Opens `Users_Path` for writing, then writes each user's
 * username, password, and role.
 *
 * @param arr    Array of users to save.
 * @param count  Number of entries.
 * @return       0 on success; -1 on file open failure.
 */
int save_users(const User *arr, int count) {
    FILE *f = fopen(Users_Path, "w");
    if (!f) return -1;
    for (int i = 0; i < count; i++)
        fprintf(f, "%s %s %d\n", arr[i].username, arr[i].password, arr[i].role);
    fclose(f); return 0;
}

/**
 * ? Load all candidate manifestos.
 *
 * Reads each line formatted as "username|manifesto" from `Manifesto_Path`,
 * splitting on '|', stripping trailing newline from manifesto.
 *
 * @param[out] out  Destination pointer for allocated Manifesto array.
 * @return           Number of manifestos loaded.
 *
 * @note Caller must free `*out`.
 */
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

/**
 * ? Save an array of manifestos to disk.
 *
 *  Writes each record as "rep_username|manifesto\n" to `Manifesto_Path`.
 *
 * @param arr    Manifesto array.
 * @param count  Number of entries.
 * @return       0 on success; -1 on failure to open file.
 */
int save_manifestos(const Manifesto *arr, int count) {
    FILE *f = fopen(Manifesto_Path, "w");
    if (!f) return -1;
    for (int i = 0; i < count; i++)
        fprintf(f, "%s|%s\n", arr[i].rep_username, arr[i].manifesto);
    fclose(f); return 0;
}
/**
 * ? Load all votes from disk.
 *
 * Reads lines "student_username rep_username" into a dynamically allocated Vote array.
 *
 * @param[out] out  Destination pointer for allocated Vote array.
 * @return           Number of votes loaded.
 *
 * @note Caller must free `*out`.
 */
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

/**
 * ? Save a list of votes to disk.
 *
 * Serializes each Vote as "student_username rep_username\n" into `Votes_Path`.
 *
 * @param arr    Array of votes.
 * @param count  Number of votes.
 * @return       0 on success; -1 if file open fails.
 */
int save_votes(const Vote *arr, int count) {
    FILE *f = fopen(Votes_Path, "w");
    if (!f) return -1;
    for (int i = 0; i < count; i++)
        fprintf(f, "%s %s\n", arr[i].student_username, arr[i].rep_username);
    fclose(f); return 0;
}

/**
 * ? Save final vote tally results to disk.
 *
 * Writes each manifesto's rep_username and vote count separated by space.
 *
 * @param mfs      Array of Manifesto entries.
 * @param counts   Corresponding vote counts.
 * @param mfCount  Number of entries.
 * @return         0 on success; -1 on file open failure.
 */
int save_results(const Manifesto *mfs, const int *counts, int mfCount) {
    FILE *f = fopen(Results_Path, "w");
    if (!f) return -1;
    for (int i = 0; i < mfCount; i++)
        fprintf(f, "%s %d\n", mfs[i].rep_username, counts[i]);
    fclose(f); return 0;
}

/**
 * ? Load published results from disk.
 *
 * Reads each line formatted "rep_username count" from `Results_Path`,
 * generating separate arrays for manifestos and vote counts.
 *
 * @param[out] outMfs     Manifesto array (rep_username only).
 * @param[out] outCounts  Parallel array of vote counts.
 * @return                Number of records loaded (0 if none or file missing).
 *
 * @pre Caller must free both `*outMfs` and `*outCounts`.
 */
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

/**
 * ? Ensure every rep has a manifesto entry.
 *
 * - Loads current reps and manifestos.
 * - Syncs entries: carries over existing manifestos, and for absent reps,
 *   adds placeholder "Not yet submitted".
 * - Saves the updated list to disk.
 *
 * *Usage:
 *   - Keeps manifesto records consistent after reps register/unregister.
 */
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

/**
 * ? Display current result publication status.
 *
 * Reads the first line of `Vote_Updates_Path`:
 * - If it starts with "updated", prints that results have been published.
 * - Otherwise, indicates new votes exist and warns waiting for publishing.
 * - If file is missing or empty, displays status accordingly.
 * * Usage:
 *   - Called by admin to check if results are ready.
 *   - Helps avoid confusion about whether results are finalized.
 */
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

/**
 * ? Mark that results have been published.
 *
 * Overwrites `Vote_Updates_Path` with single line "updated",
 * signaling to other code that results are current.
 *
 * * Usage:
 *   - Called after admin publishes results.
 *   - Prevents confusion by indicating no new votes are pending.
 *   - Ensures future vote updates are appended correctly.
 */
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

/**
 * ? Record a vote update to track new votes.
 *
 * Appends a line "vote_cast_by:<username>" to `Vote_Updates_Path`.
 * If file begins with "updated", it is reset before appending.
 *
 * @param current  User who cast the vote (must not be NULL).
 *
 * *Usage:
 *   - Called after successful vote by a student.
 *   - Maintains new‑vote flag for admins to re‑publish results.
 */ 
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